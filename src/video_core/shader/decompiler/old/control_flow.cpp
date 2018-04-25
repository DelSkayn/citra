
#include "video_core/shader/decompiler/control_flow.h"
#include <vector>

using nihstro::OpCode;
using nihstro::Instruction;
using nihstro::RegisterType;
using nihstro::SourceRegister;
using nihstro::SwizzlePattern;

namespace Pica{
namespace Shader{
namespace Decompiler{

    ControlFlow::ControlFlow(CodeArray * program_code,int entry_point){
        this->program_code = program_code;
        this->entry_point = entry_point;
        this->num_blocks = 0;
        this->error = ControlFlowError::None;
        this->blocks = {};
        this->block_order = {};
        this->first = nullptr;
    }

    std::string ControlFlow::ErrorToString(ControlFlow::ControlFlowError e){
        switch (e){
        case ControlFlowError::None:
            return "No error";
        case ControlFlowError::CallToAlreadyEncountered:
            return "A CALL jump jumped into code which can also be reached via regular control flow.";
        case ControlFlowError::NotSupportedInstruction:
            return "Flow control encountered unsupported flowcontroll instruction.";
        }
    }

    bool ControlFlow::Analyze(){
        BlockPointerArray pending = {};
        int num_pending = 0;
        CodeBlock * start_block = &blocks[num_blocks++];
        block_order[0] = start_block;
        this->first = start_block;
        start_block->first = this->entry_point;
        start_block->last = MAX_PROGRAM_CODE_LENGTH;
        start_block->dominator = nullptr;
        AnalyzeBlock(start_block,pending,num_pending);
        while(num_pending > 0){
            CodeBlock * pending_block = pending[--num_pending];
            if(pending_block->encountered){
                continue;
            }
            AnalyzeBlock(pending_block,pending,num_pending);
            if(this->error != ControlFlowError::None){
                return true;
            }
        }
        this->Finalize();
        return false;
    }

    void ControlFlow::AnalyzeBlock(CodeBlock * current_block,BlockPointerArray & pending,int & num_pending){
        for(int i = current_block->first;i <= current_block->last;i++){

            const Instruction instr = {(*program_code)[i]};

            CodeBlock * exit_block = nullptr;
            CodeBlock * branch_block = nullptr;
            CodeBlock * return_block = nullptr;

            int dest = instr.flow_control.dest_offset;
            int num = instr.flow_control.num_instructions;

            switch(instr.opcode.Value().EffectiveOpCode()){
            case OpCode::Id::JMPC:
            case OpCode::Id::JMPU:
                exit_block = block(i+1);
                branch_block = block(dest);

                current_block->exit = exit_block;
                current_block->branch = branch_block;

                pending[num_pending++] = exit_block;
                pending[num_pending++] = branch_block;
                current_block->encountered = true;
                return;
            case OpCode::Id::IFU:
            case OpCode::Id::IFC:
                exit_block = block(i+1);
                branch_block = block(dest);
                return_block = block(dest + num);

                current_block->exit = exit_block;
                current_block->branch = branch_block;

                exit_block->exit = return_block;
                branch_block->exit = return_block;

                pending[num_pending++] = exit_block;
                pending[num_pending++] = branch_block;
                pending[num_pending++] = return_block;
                current_block->encountered = true;
                return;
            case OpCode::Id::LOOP:
                this->error = ControlFlowError::NotSupportedInstruction;
                return;

            case OpCode::Id::CALLU:
            case OpCode::Id::CALLC:
                /*
                exit_block = block(i+1);
                branch_block = block(dest);
                if(branch_block->encountered){
                    this->error = ControlFlowError::CallToAlreadyEncountered;
                    return;
                }
                return_block = block(dest + num);

                current_block->exit = exit_block;
                current_block->branch = branch_block;
                branch_block->exit = exit_block;

                pending[num_pending++] = exit_block;
                pending[num_pending++] = branch_block;
                current_block->encountered = true;
                */
                this->error = ControlFlowError::NotSupportedInstruction;
                return;
            case OpCode::Id::CALL:
                /*
                exit_block = block(i+1);
                branch_block = block(dest);
                if(branch_block->encountered){
                    this->error = ControlFlowError::CallToAlreadyEncountered;
                    return;
                }
                return_block = block(dest + num);

                current_block->exit = branch_block;
                branch_block->exit = exit_block;

                pending[num_pending++] = exit_block;
                pending[num_pending++] = branch_block;
                current_block->encountered = true;
                */
                this->error = ControlFlowError::NotSupportedInstruction;
                return;
            case OpCode::Id::END:
                current_block->exit = nullptr;
                current_block->last = i;
                current_block->encountered = true;
                return;
            }
        }
    }

    //Find the block containing the given first instruction.
    //allocate a new block with the branch and exit of the block which previously contained the instruction
    //the old block now exits into the new block and ends the instruction before the given instruction.
    ControlFlow::CodeBlock * ControlFlow::block(unsigned first){
        CodeBlock * b = this->find(first);
        if(b->first == first){
            //The block split already exists no need to do anything.
            return b;
        }
        CodeBlock * new_b = this->allocate(b,first);
        new_b->last = b->last;
        b->last = first- 1;

        new_b->first = first;
        new_b->exit = b->exit;
        new_b->branch = b->branch;
        new_b->encountered = b->encountered;
        new_b->num_in = 0;
        new_b->dominator = nullptr;

        b->exit = new_b;
        b->branch = nullptr;
    }

    //Binary search for the block which currently contains the instruction
    ControlFlow::CodeBlock * ControlFlow::find(unsigned first){
        int current = num_blocks / 2;
        int left = 0;
        int right = num_blocks;
        for(;;){
            if(block_order[current]->last <= first){
                left = current+1;
                current = (right - left) / 2 + left;
            }else if(block_order[current]->first >= first){
                right = current;
                current = (right - left) / 2 + left;
            }else{
                return block_order[current];
            }
        }
    }

    // Allocates a new block and places it in the right order in block_order.
    //
    ControlFlow::CodeBlock * ControlFlow::allocate(CodeBlock * previous,unsigned first){
        CodeBlock * new_block = &this->blocks[num_blocks++];
        // place in the right order
        int i = 0;
        for(;i < num_blocks;i++){
            if(block_order[i] == previous){
                break;
            }
        }
        ASSERT(i < num_blocks);

        i += 1;
        CodeBlock * replace = new_block;
        for(;i < num_blocks;i++){
            CodeBlock * tmp = block_order[i];
            block_order[i] = replace;
            replace = tmp;
        }
        return new_block;
    }


    void ControlFlow::Finalize(){
        ControlFlow::ConnectIns(&blocks[0]);
    }

    void ControlFlow::ConnectIns(CodeBlock * block){
        CodeBlock * b = block->branch;
        if(b != nullptr){
            ASSERT(b->num_in < MAX_BLOCK_INPUTS);
            b->in[b->num_in++] = block;
            if(b->num_ins == 1){
                ConnectIns(b);
            }
        }
        b = block->exit;
        if(b != nullptr){
            ASSERT(b->num_in < MAX_BLOCK_INPUTS);
            b->in[b->num_in++] = block;
            if(b->num_ins == 1){
                ConnectIns(b);
            }
        }
    }
}// namespace
}// namespace
}// namespace

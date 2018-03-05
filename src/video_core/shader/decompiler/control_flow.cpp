
#include "video_core/shader/decompiler/control_flow.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

ShaderBlock::ShaderBlock(unsigned first
        ,unsigned last
        ,BlockType ty
        ,ShaderBlock * out
        ,ShaderBlock * branch){

    this->ty = ty;
    this->first = first;
    this->last = last;
    this->in = {};
    this->out = out;
    this->branch = branch;
    this->ty = BlockType::Sequential;
}

void ShaderBlock::PushIn(ShaderBlock * block){
    for(int i = 0;i < MAX_BLOCK_ENTRIES;i++){
        if(in[i] == nullptr){
            in[i] = block;
            return;
        }
    }
    ASSERT(false && "Over max block entries");
}

ControlFlow::ControlFlow(CodeArray & program_code,int entry_point){
    this->program_code = program_code;
    this->entry_point = entry_point;

    this->blocks[0] = ShaderBlock(entry_point,MAX_PROGRAM_CODE_LENGTH,nullptr,nullptr,nullptr);
    this->next_block_index = 1;
    this->program_counter = entry_point;
    error = ControlFlow::None;
}

bool ControlFlow::Analyze(){
}

bool ControlFlow::AnalyzeRange(unsigned start, unsigned end,ShaderBlock * in,ShaderBlock * out){
    for(int i = start;i < end;i++){
        ASSERT(program_counter >= current_block->in
                && program_counter <= current_block->out);

        const Instruction instr = {program_code[i]};

        unsigned dest = instr.flow_control.dest_offset;
        unsigned num = instr.flow_control.num_instructions;

        switch(instr.opcode.Value().EffectiveOpCode()){

            case OpCode::Id::JMPC:
            case OpCode::Id::JMPU:{
                ShaderBlock * seq_block = &blocks[next_block_index++];
                ShaderBlock * branch_block = &blocks[next_block_index++];
                in->last = i;
                in->branch = branch_block;
                in->out = seq_block;

                branch_block->first = program_code+dest;
                branch_block->PushIn(in);

                seq_block->first = i+1;
                seq_block->PushIn(in);


                AnalyzeRange(program_code+1
                        ,program_code+dest
                        ,seq_block
                        ,branch_block);

                in = branch_block;
            }
            case OpCode::Id::IFU:
            case OpCode::Id::IFC:{
                ShaderBlock * seq_block = &blocks[next_block_index++];
                ShaderBlock * else_block = &blocks[next_block_index++];
                ShaderBlock * = &blocks[next_block_index++];
            }
        }
    }
    in->out = out;
    out->PushIn(in);
}

ShaderBlock * ControlFlow::Split(int index){
    ASSERT(index < MAX_PROGRAM_CODE_LENGTH);
    if(index < entry_point){
        this->error = DecompileError::JmpTargetBeforeEntryPoint;
        return;
    }
    //loop for now
    for(int i = 0;i < next_block_index;i++){
        if(blocks[i].in == index){
            // Can this result in an error?
            return &blocks[i];
        }
        if(blocks[i].in < index && index <= blocks[i].out){
            Block new_block(index
                    ,blocks[i].last
                    ,blocks[i].out
                    ,blocks[i].branch);

            new_block.PushIn(&blocks[i]);
            new_block.ty = blocks[i].ty;

            blocks[next_block_index] = new_block;
            blocks[i].out = &blocks[next_block_index];
            next_block_index++;
            blocks[i].branch = nullptr;
            blocks[i].last = index-1;

            return blocks[i].out;
        }
    }
}

}// namespace
}// namespace
}// namespace

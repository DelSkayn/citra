
#include "video_core/shader/decompiler/control_flow.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

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
    this->blocks = {};
    this->blocks[0]->first = entry_point;
    this->blocks[0]->last = MAX_PROGRAM_CODE_LENGTH;
    this->next_block_index = 1;
}

void ControlFlow::Analyze(){
    AnalyzeBlock(&block[0]);
}

ShaderBlock * ControlFlow::First(){
    &block[0];
}

void ControlFlow::AnalyzeBlock(ShaderBlock * block){
    for(int i = block->first;i < block->last;i++){
        const Instruction instr = {program_code[i]};

        unsigned dest = instr.flow_control.dest_offset;
        unsigned num = instr.flow_control.num_instructions;

        switch(instr.opcode.Value().EffectiveOpCode()){
            case OpCode::Id::JMPC:
            case OpCode::Id::JMPU:{
                ShaderBlock * seq_block = &blocks[next_block_index++];
                ShaderBlock * branch_block = &blocks[next_block_index++];

                branch_block->first = i+dest+1;
                branch_block->PushIn(block);
                branch_block->last = block->last;
                branch_block->out = block->out;
                branch_block->branch = block->branch;

                seq_block->first = i+1;
                seq_block->PushIn(block);
                seq_block->last = i+dest;
                seq_block->out = branch_block;

                block->last = i;
                block->out = seq_block;
                block->branch = branch_block;
                block->ty = BlockType::If;

                AnalyzeBlock(seq_block);
                AnalyzeBlock(branch_block);
                return;
            }
            case OpCode::Id::IFU:
            case OpCode::Id::IFC:{
                ShaderBlock * seq_block = &blocks[next_block_index++];
                ShaderBlock * else_block = &blocks[next_block_index++];
                ShaderBlock * after_block = &blocks[next_block_index++];

                after_block->PushIn(else_block);
                after_block->PushIn(seq_block);
                after_block->first = i+dest+num+1;
                after_block->last = block->last;
                after_block->branch = block->branch;

                else_block->first = i+dest+1;
                // Assume inclusive since loop is inclusive
                else_block->last = i+dest+num;
                else_block->out = after_block;
                else_block->PushIn(block);

                seq_block->first = i+1;
                seq_block->last = i+dest;
                seq_block->out = after_block;
                seq_block->PushIn(block);

                block->last = i;
                block->out = seq_block;
                block->branch = else_block;
                block->ty = BlockType::IfElse;

                AnalyzeBlock(seq_block);
                AnalyzeBlock(else_block);
                AnalyzeBlock(after_block);
                return;
            }
            case OpCode::Id::CALL:{
                ShaderBlock * call_block = &blocks[next_block_index++];
                ShaderBlock * next_block = &blocks[next_block_index++];

                next_block->first = i+1;
                next_block->last = block->last;
                next_block->out = block->out;
                next_block->PushIn(block)

                call_block->first = i+dest;
                //Inclusive?
                call_block->last = i+dest+num;
                call_block->PushIn(block);
                call_block->out = next_block;

                block->out = call_block;
                block->last = i;
                AnalyzeBlock(call_block);
                AnalyzeBlock(next_block);
                return;
            }
            case OpCode::Id::CALLU:
            case OpCode::Id::CALLC:{
                ShaderBlock * call_block = &blocks[next_block_index++];
                ShaderBlock * next_block = &blocks[next_block_index++];

                next_block->first = i+1;
                next_block->last = block->last;
                next_block->out = block->out;
                next_block->PushIn(block)

                call_block->first = i+dest;
                //Inclusive?
                call_block->last = i+dest+num;
                call_block->PushIn(block);
                call_block->out = next_block;

                block->branch = call_block;
                block->out = next_block;
                block->last = i;
                block->ty = CondCall;
                AnalyzeBlock(call_block);
                AnalyzeBlock(next_block);
                return;
            }
            case OpCode::Id::LOOP:
                ShaderBlock * loop_block = &block[next_block_index++]
                ShaderBlock * body_block = &blocks[next_block_index++];
                ShaderBlock * after_block = &blocks[next_block_index++];

                after_block->first = i+dest+1;
                after_block->last = block->last;
                after_block->out = block->out;
                after_block->PushIn(block);

                body_block->first = i+1;
                body_block->last = i+dest;
                body_block->out = block;
                body_block->PushIn(block);

                loop_block->first = i;
                loop_block->last = i;
                loop_block->out = body_block;
                loop_block->branch_block = after_block;
                loop_block->PushIn(body_block);
                loop_block->PushIn(block);
                loop_block->ty = BlockType::For;

                block->out = loop_block;
                block->last = i-1;

                AnalyzeBlock(body_block);
                AnalyzeBlock(next_block);
                return;
            case OpCode::Id::END:
                block->last = i;
                block->out = nullptr;
                block->branch = nullptr;
                block->ty = BlockType::Sequential;
                return;
        }
    }
}

}// namespace
}// namespace
}// namespace

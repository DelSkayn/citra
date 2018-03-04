
#include <nihstro/shader_bytecode>
#include "video_core/shader/shader_gpu_compiler.h"


namespace Pica {

namespace Shader {

using nihstro::Instruction;
using nihstro::OpCode;

ShaderBlock::ShaderBlock(){
    this->branch = MAX_PROGRAM_CODE_LENGTH;
}

ShaderBlock::HasBranch{
    return this->branch != MAX_PROGRAM_CODE_LENGTH;
}

ControlFlow::ControlFlow(std::array<u32,MAX_PROGRAM_CODE_LENGTH> & program_code,int entry_point){
    if(entry_point != 0){
        this->blocks.push_back({.in = 0,.out = entry_point-1});
        this->current_block = 1;
    }else{
        this->current_block = 0;
    }
    this->blocks.push_back({.in = entry_point});
    this->program_code = program_code;
    this->program_counter = entry_point;
    error = ControlFlow::None;
}

bool ControlFlow::Analyze(){
    for(;;program_counter++){
        if(program_counter >= MAX_PROGRAM_CODE_LENGTH){
            error = ControlFlow::AnalysisRanOutsideOfShaderProgram;
            return false;
        }
        Instruction instr = {program_code[program_counter]};
        int dest = instr.flow_control.dest_offset;
        int num = instr.flow_control.num;
        switch(instr.opcode.Value().EffectiveOpCode()){
            case OpCode::Id::JMPC:
            case OpCode::Id::JMPU:
                Split(program_counter+1,program_counter+dest);//next block
                Split(program_counter+dest);//branch entry
                current_block++;
                break;
            case OpCode::Id::IFC:
            case OpCode::Id::IFU:
                if(dest < 0){
                    error = ControlFlowError::DstTargetBeforeIf;
                    return false;
                }
                Split(program_counter+1,program_counter + dest);//next block
                if(instr.flow_control.num_instructions == 0){
                    //same as jump
                    Split(program_counter+dest);//branch entry
                }else{
                    Split(program_counter+dest,program_counter+dest+num);//then branch entry
                    Split(program_counter+dest+num)//past else entry;
                }
                current_block++;
                break;
            case OpCode::END:
                block[current_block].out = program_counter;
                current_block++;
                if(current_block == block.size){
                    return true;
                }
            case OpCode::Id::BREAK:
            case OpCode::Id::BREAKC:
            case OpCode::Id::LOOP:
            case OpCode::Id::CALL:
            case OpCode::Id::CALLC:
            case OpCode::Id::CALLU:
                error = ControlFlowError::UnsupportedBranchInstruction;
                return false;
        }
    }
}

//Splits a block into two blocks so that the following holds
//  |                     |
//  |branch = branch_instr|
//  |---------------------|<- index-1;
//             |
//             |
//             |
//  |---------------------|<- index;
//  |                     |
//  |branch = old_branch  |
//
bool ControlFlow::Split(int index,int branch_instr = MAX_PROGRAM_CODE_LENGTH){
    ASSERT(index < MAX_PROGRAM_CODE_LENGTH);
    //for now just loop
    for(int i = 0;i < blocks.size;i++){
        if(blocks[i].in == index){
            // Can this result in an error?
            return;
        }
        if(blocks[i].in > index){
            // We can probably assume that i-1 will be >= 0
            ShaderBlock block = {
                .in = index,
                .out = blocks[i-1].out,
                .branch = blocks[i-1].branch,
            };
            blocks[i-1].out = index-1;
            blocks[i-1].branch = branch_instr;
            blocks.insert(blocks.begin()+i,block);
            return;
        }
    }
    //no in was larger then index so it must be the last block
    //which should be split
    int size = blocks.size;
    blocks[size-1].
    ShaderBlock block = {
        .in = index,
        .out = blocks[size-1].out,
        .branch = blocks[size-1].branch,
    };
    blocks[size-1].out = index-1;
    blocks[size-1].branch = branch_instr;
    blocks.push_back(block);
}

} // namespace

} // namespace

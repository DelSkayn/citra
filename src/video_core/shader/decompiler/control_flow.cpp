
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

    ControlFlow::ControlFlow(CodeArray * program_code,unsigned entry_point,unsigned end = MAX_PROGRAM_CODE_LENGTH){
        this->program_code = program_code;
        this->entry_point = entry_point;
        this->blocks = {0};
        this->num_blocks = 1;
        this->blocks[0].first = entry_point;
        this->blocks[0].last = end;
    }

    void ControlFlow::analyze(){
        create_blocks();
        connect_blocks();
    }

    void ControlFlow::create_blocks(){
        for(int i = entry_point;i < MAX_PROGRAM_CODE_LENGTH;i++){
            const Instruction instr = {program_code[i]};
            int dest = instr.flow_control.dest_offset;
            int num = instr.flow_control.num_instructions;
            switch(instr.opcode.Value().EffectiveOpCode()){
            case OpCode::Id::JMPC:
            case OpCode::Id::JMPU:
                split(dest);
                break;
            case OpCode::Id::IFU:
            case OpCode::Id::IFC:
                split(dest);
                split(dest+num);
                break;
            case OpCode::Id::LOOP:
                split(dest + 1);
                break;
            case OpCode::Id::END:
                split(i+1);
                break;
            case OpCode::Id::CALLU:
            case OpCode::Id::CALLC:
            case OpCode::Id::CALL:
                ProcCall call;
                call.instr = i;
                //INCLUSIVE ?
                call.flow = ControlFlow(program_code,dest,dest+num+1);
                this->proc_calls.push(call);
                this->proc_calls.last().flow.analyze();
                break;
            }
        }
    }

    void ControlFlow::connect_blocks(){
        int current_block = 0;
        for(int i = entry_point;i < MAX_PROGRAM_CODE_LENGTH;i++){

            if(blocks[current_block].last < i){
                current_block++;
            }

            const Instruction instr = {program_code[i]};
            int dest = instr.flow_control.dest_offset;
            int num = instr.flow_control.num_instructions;

            switch(instr.opcode.Value().EffectiveOpCode()){
            case OpCode::Id::JMPC:
            case OpCode::Id::JMPU:{
                int dest_block = find(dest);
                edge(current_block,current_block+1);
                edge(current_block,dest_block);
                break;
            }
            case OpCode::Id::IFU:
            case OpCode::Id::IFC:{
                int else_block = find(dest);
                int exit_block = find(dest + num);
                int if_block = current_block+1;

                edge(current_block,if_block);
                edge(if_block,exit_block);
                edge(current_block,else_block);
                edge(else_block,exit_block);
                break;
            }
            case OpCode::Id::LOOP:{
                int body_block = find(dest + 1) - 1;
                edge(current_block,current_block+1);
                edge(body,current_block);
                edge(current_block,body+1);
                break;
            }
            case OpCode::Id::END:{
                exit(current_block);
                break;
            }
            case OpCode::Id::CALLU:
            case OpCode::Id::CALLC:
            case OpCode::Id::CALL:
                //TODO
                ASSERT(false);
            }

            //Add an edge if there is no other control flow with other ideas.
            if(blocks[current_block].first == i
                && out[current_block-1].size() == 0){
                edge(current_block-1,current_block);
            }
        }
    }

    void ControlFlow::edge(int from,int to){
        out[from].push(to);
        in[to].push(from);
    }

    void ControlFlow::exit(int from){
        out[from].push(MAX_PROGRAM_CODE_LENGTH);
    }

    int ControlFlow::find(int in){
        int current = num_blocks / 2;
        int left = 0;
        int right = num_blocks;
        for(;;){
            if(blocks[current].last < in){
                left = current;
            }else if(blocks[current].first > in){
                right = current+1;
            }else{
                return current;
            }
            current = (right - left) / 2 + left;
        }
    }

    void ControlFlow::split(int in){
        ASSERT(in >= entry_point);
        int current = find(in);

        if(blocks[current].first == in){
            return;
        }

        CodeBlock new_block;
        new_block.first = in;
        new_block.last = blocks[current].last;
        blocks[current].last = in - 1;
        num_blocks += 1;
        for(int i = current + 1;i < num_blocks;i++){
            CodeBlock tmp = blocks[i];
            blocks[i] = new_block;
            new_block = tmp;
        }
    }

    bool ControlFlow::is_if(int index){
    }

}// namespace
}// namespace
}// namespace

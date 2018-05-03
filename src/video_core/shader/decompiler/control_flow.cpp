
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

    ControlFlow::ControlFlow(){}

    ControlFlow::ControlFlow(std::array<unsigned,MAX_PROGRAM_CODE_LENGTH> * program_code
                             , unsigned entry_point
                             , unsigned end){
        this->program_code = program_code;
        this->entry_point = entry_point;
        this->blocks = {0};
        this->num_blocks = 1;
        this->blocks[0].first = entry_point;
        this->blocks[0].last = end;
    }

    void ControlFlow::build(){
        create_blocks();
        connect_blocks();
    }

    void ControlFlow::create_blocks(){
        for(int i = entry_point;i < MAX_PROGRAM_CODE_LENGTH;i++){
            const Instruction instr = {(*program_code)[i]};
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
                //TODO
                ASSERT(false);
                ProcCall call;
                call.instr = i;
                //INCLUSIVE ?
                call.flow = ControlFlow(program_code,dest,dest+num+1);
                this->proc_calls.push_back(call);
                this->proc_calls.back().flow.build();
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

            const Instruction instr = {(*program_code)[i]};
            int dest = instr.flow_control.dest_offset;
            int num = instr.flow_control.num_instructions;

            switch(instr.opcode.Value().EffectiveOpCode()){
            case OpCode::Id::JMPC:
            case OpCode::Id::JMPU:{
                int dest_block = find(dest);
                edge(current_block,current_block+1,{TagType::Not,i});
                edge(current_block,dest_block,{TagType::Equal,i});
                break;
            }
            case OpCode::Id::IFU:
            case OpCode::Id::IFC:{
                int else_block = find(dest);
                int exit_block = find(dest + num);
                int if_block = current_block+1;

                edge(current_block,if_block,{TagType::Equal,i});
                edge(if_block,exit_block);
                edge(current_block,else_block,{TagType::Not,i});
                edge(else_block,exit_block);
                break;
            }
            case OpCode::Id::LOOP:{
                int body_block = find(dest + 1) - 1;
                edge(current_block,current_block+1,{TagType::Equal,i});
                edge(body_block,current_block);
                edge(current_block,body_block+1,{TagType::Not,i});
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

    void ControlFlow::edge(int from,int to,Tag tag){
        auto size = out[from].size();
        for(auto i = tags[from].size()-1;i < size; i++){
            tags[from].push_back(Tag{ TagType::None, 0});
        }
        tags[from].push_back(tag);
        out[from].push_back(to);
        in[to].push_back(from);
    }

    void ControlFlow::edge(int from,int to){
        out[from].push_back(to);
        in[to].push_back(from);
    }

    void ControlFlow::exit(int from){
        out[from].push_back(MAX_PROGRAM_CODE_LENGTH);
    }

    unsigned ControlFlow::find(unsigned in){
        unsigned current = num_blocks / 2;
        unsigned left = 0;
        unsigned right = num_blocks;
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

    void ControlFlow::split(unsigned in){
        ASSERT(in >= entry_point);
        unsigned current = find(in);

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

    void ControlFlow::pre_order(std::functional<void (unsigned,CodeBlock&)> func){
        std::array<bool,MAX_PROGRAM_CODE_LENGTH> reached = {false};
        pre_order_impl(find(entry_point),reached,func);
    }

    void ControlFlow::pre_order_impl(unsinged block
            , std::array<bool,MAX_PROGRAM_CODE_LENGTH> & reached
            , std::functional<void (CodeBlock&)> func){

        reached[block] = true;
        func(block,blocks[block]);
        for(unsigned out: out[block]){
            if(!reached[block]){
                pre_order_impl(out,reached,func);
            }
        }
    }

    Tag ControlFlow::tag(unsigned block,unsigned index){
        if(index >= tags[block].size()){
            return {TagType::None, 0};
        }
        return tags[block][index];
    }


}// namespace
}// namespace
}// namespace

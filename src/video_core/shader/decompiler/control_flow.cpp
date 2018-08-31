
#include "video_core/shader/decompiler/control_flow.h"
#include "video_core/shader/decompiler/code_visitor.h"
#include <iostream>

using nihstro::OpCode;
using nihstro::Instruction;
using nihstro::RegisterType;
using nihstro::SourceRegister;
using nihstro::SwizzlePattern;

namespace Pica{
namespace Shader{
namespace Decompiler{

ControlFlow::ControlFlow(){
}

ControlFlow::~ControlFlow(){
    for(Rc<Node> & node: this->nodes_by_index){
        node->out.clear();
    }
}

void ControlFlow::build(ProgramArray & program, unsigned first, unsigned last,ProcMap<ControlFlow> & proc){
    this->build_blocks(program,first,last);
    this->build_edges(program,first,last,proc);
}


void ControlFlow::split_at(unsigned instr){
    ASSERT(instr < PROGRAM_LEN);
    std::cout << "SPLIT: " << instr << std::endl;

    unsigned prev_index = *find_index(instr);
    auto prev = this->nodes_by_index[prev_index];
    // dont split if we already have a split at the place.
    if (prev->first == instr){
        return;
    }
    Rc<Node> n = std::make_shared<Node>();
    n->first = instr;
    n->last = prev->last;
    prev->last = instr-1;

    auto it = nodes_by_index.begin();
    it += prev_index + 1;
    nodes_by_index.insert(it,n);
}

void ControlFlow::edge(Rc<Node> from,Rc<Node> to,Option<Cond> cond = boost::none){
    ASSERT(from->out.size() < 2);
    std::cout << "EDGE: from " << from->first
              << " to " << to->first << std::endl;

    Rc<Edge> edge = std::make_shared<Edge>();
    edge->cond = cond;
    edge->from = from;
    edge->to = to;
    from->out.push_back(edge);
    to->in.push_back(edge);
}

Option<Rc<ControlFlow::Node>> ControlFlow::find(unsigned instr) const{
    return this->nodes_by_index[*find_index(instr)];
}

Option<unsigned> ControlFlow::find_index(unsigned instr) const{
    //binary search
    unsigned first = 0;
    unsigned last = this->nodes_by_index.size() - 1;
    while(first <= last){
        unsigned m = (last + first) / 2.0;
        if(this->nodes_by_index[m]->first <= instr
           && this->nodes_by_index[m]->last >= instr){

            return m;
        }else if(this->nodes_by_index[m]->first > instr){
            last = m - 1;
        }else{

            first = m + 1;
        }
    }
    return boost::none;
}

bool ControlFlow::Node::is_end(){
    return this->first == PROGRAM_LEN;
}

void ControlFlow::build_blocks(ProgramArray & program,unsigned first,unsigned last){
    std::cout << "BUILD BLOCKS" << std::endl;
    if(first > 0){
        Rc<Node> pre = std::make_shared<Node>();
        pre->first = 0;
        pre->last = first - 1;
        nodes_by_index.push_back(pre);
    }

    Rc<Node> start = std::make_shared<Node>();
    start->first = first;
    start->last = last;
    this->start = start;
    nodes_by_index.push_back(start);

    Rc<Node> end = std::make_shared<Node>();
    end->first = PROGRAM_LEN;
    end->last = PROGRAM_LEN;
    this->end = end;
    nodes_by_index.push_back(end);

    CodeVisitor visitor = CodeVisitor(program,first,last);
    Option<unsigned> it = visitor.next_index();
    while(it){
        unsigned i = *it;
        std::cout << i << std::endl;
        const Instruction instr = {program[i]};
        const unsigned dest = instr.flow_control.dest_offset;
        const unsigned num = instr.flow_control.num_instructions;
        switch(instr.opcode.Value().EffectiveOpCode()){
        case OpCode::Id::JMPC:
        case OpCode::Id::JMPU:
            std::cout << "JMP" << std::endl;
            split_at(i+1);
            split_at(dest);
            break;
        case OpCode::Id::IFU:
        case OpCode::Id::IFC:
            if(num == 0){
                std::cout << "IF" << std::endl;
            }else{
                std::cout << "IF ELSE" << std::endl;
            }
            split_at(i+1);
            split_at(dest);
            if(num != 0){
                split_at(dest+num);
            }
            break;
        case OpCode::Id::LOOP:
            std::cout << "LOOP" << std::endl;
            split_at(i);
            split_at(i+1);
            split_at(dest + 1);
            break;
        case OpCode::Id::END:
            std::cout << "END" << std::endl;
            split_at(i+1);
            break;
        case OpCode::Id::CALLU:
        case OpCode::Id::CALLC:
        case OpCode::Id::CALL:
            std::cout << "CALL" << std::endl;
            break;
        }
        it = visitor.next_index();
    }
}

void ControlFlow::build_edges(ProgramArray & program,unsigned first,unsigned last,ProcMap<ControlFlow> & proc){
    std::cout << "BUILD EDGES" << std::endl;
    CodeVisitor visitor = CodeVisitor(program,first,last);
    Option<unsigned> it = visitor.next_index();
    auto cur = start;
    while(it){
        unsigned i = *it;
        if(cur->first < i || cur->last > i){
            cur = *find(i);
        }
        const Instruction instr = {program[i]};
        const unsigned dest = instr.flow_control.dest_offset;
        const unsigned num = instr.flow_control.num_instructions;
        switch(instr.opcode.Value().EffectiveOpCode()){
        case OpCode::Id::JMPC:
        case OpCode::Id::JMPU:{
            std::cout << "JMP" << std::endl;
            auto next = *find(i+1);
            auto dst = *find(dest);
            Cond cond = {i,true};
            edge(cur,dst,cond);
            cond.true_edge = false;
            edge(cur,next,cond);
            break;
        }
        case OpCode::Id::IFU:
        case OpCode::Id::IFC:{
            auto next = *find(i+1);
            auto dst = *find(dest);
            if(num != 0){
                std::cout << "IF" << std::endl;
                Cond cond = {i,true};
                edge(cur,next,cond);
                cond.true_edge = false;
                edge(cur,dst,cond);
                break;
            }
            std::cout << "IF ELSE" << std::endl;
            auto next_last = *find(dest - 1);
            auto after = *find(dest+num);
            Cond cond = {i,true};
            edge(cur,next,cond);
            cond.true_edge = false;
            edge(cur,dst,cond);
            edge(next_last,after);
            break;
        }
        case OpCode::Id::LOOP:{
            std::cout << "LOOP" << std::endl;
            auto next = *find(i+1);
            // +1 because loop is inclusive to dest
            auto next_last = *find(dest + 1);
            // +2 because previous
            auto after = *find(dest + 2);
            Cond cond = {i,true};
            edge(next_last,cur);
            edge(cur,next,cond);
            cond.true_edge = false;
            edge(cur,after,cond);
            //TODO
            break;
        }
        case OpCode::Id::END:{
            std::cout << "END" << std::endl;
            edge(cur,this->end);
            break;
        }
        case OpCode::Id::CALLU:
        case OpCode::Id::CALLC:
        case OpCode::Id::CALL:{
            std::cout << "CALL" << std::endl;
            ControlFlow flow;
            flow.build(program,dest,dest+num,proc);
            Region region = {dest,dest+num};
            proc.insert(ProcPair<ControlFlow>(region,flow));
            break;
        }
        }
        it = visitor.next_index();
    }

    auto prev = this->nodes_by_index[0];
    for(int i = 1;i < this->nodes_by_index.size();i++){
        if (prev->out.size() == 0){
            edge(prev,this->nodes_by_index[i]);
            prev = this->nodes_by_index[i];
        }
    }
}

}// namespace
}// namespace
}// namespace

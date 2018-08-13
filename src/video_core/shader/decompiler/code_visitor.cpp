
#include <nihstro/shader_bytecode.h>

using nihstro::OpCode;

namespace Pica{
namespace Shader{
namespace Decompiler{

CodeVisitor::CodeVisitor(ProgramArray & array,unsigned first, unsigned last){
    this->stack.push_back({first,last});
    this->reached = {false};
    this->array = array;
}

Option<Instruction> CodeVisitor::next(){
    auto res = this->next_index();
    if(res){
        return this->array[*res];
    }
    return boost::none;
}

Option<unsigned> CodeVisitor::next_index(){
    while(stack.size() > 0){
        Region & r = stack.back();
        if(r.first > r.last){
            stack.pop_back();
            continue;
        }
        unsigned i = r.first;
        if(this->reached[i]){
            continue;
        }
        const Instruction instr = {array[i]};
        switch(instr.opcode.Value().EffectiveOpCode()){
        case OpCode::Id::JMPC:
        case OpCode::Id::JMPU:{
            Region cur = stack.back();
            stack.pop_back();
            Region r = {dest,cur.end};
            stack.push_back(r);
            r = {i+1,dest};
            stack.push_back(r);
            break;
        }
        case OpCode::Id::IFU:
        case OpCode::Id::IFC:{
            Region cur = stack.back();
            stack.pop_back();
            Region r = {dest+num,cur.end};
            stack.push_back(r);
            r = {dest,dest+num};
            stack.push_back(r);
            r = {i+1,dest};
            stack.push_back(r);
            break;
        }
        case OpCode::Id::LOOP:{
            Region cur = stack.back();
            stack.pop_back();
            Region r = {dest+1,cur.end};
            stack.push_back(r);
            r = {i+1,dest+1};
            stack.push_back(r);
            break;
        }
        case OpCode::Id::END:
            stack.pop_back();
            break;
        }
        return i;
    }
    return boost::none;
}

}}}

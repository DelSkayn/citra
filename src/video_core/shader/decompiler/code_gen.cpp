
#include <iostream>
#include "video_core/shader/decompiler/code_gen.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

using nihstro::OpCode;

using Node = ASTree::Node;
using Seq = ASTree::Seq;
using If = ASTree::If;
using IfElse = ASTree::IfElse;
using While = ASTree::While;
using DoWhile = ASTree::DoWhile;

void SourceWriter::push_stack(){
    prefix.push_back(' ');
    prefix.push_back(' ');
}
void SourceWriter::pop_stack(){
    prefix.pop_back();
    prefix.pop_back();
}
void SourceWriter::write(std::string src){
    src += prefix + src + "\n";
}

bool CodeGen::generate_proc(Region r,ASTree & tree){
    SourceWriter writer;
    writer.write("void proc_" + std::to_string(r.first) + "_" + std::to_string(r.last) + "(){");
    writer.push_stack();
    if(!generate_block(tree.start,writer)){
        return false;
    }
    writer.pop_stack();
    writer.write("}");
    proc_src.push_back(writer.src);
    return true;
}

bool CodeGen::generate_main(ASTree & tree){
    SourceWriter writer;
    writer.write("void main(){");
    writer.push_stack();
    if(!generate_block(tree.start,writer)){
        return false;
    }
    writer.pop_stack();
    writer.write("}");
    main_src = writer.src;
    return true;
}

bool CodeGen::generate_block(Rc<Node> & block, SourceWriter & writer){
    ASSERT(!block->is_end());

    Rc<Seq> seq = std::dynamic_pointer_cast<Seq>(block);
    if(seq){
        if(!generate_block(seq->head,writer)){
            return false;
        }
        return generate_block(seq->tail,writer);
    }

    Rc<If> n_if = std::dynamic_pointer_cast<If>(block);
    if(n_if){
        return generate_if(n_if,writer);
    }

    Rc<IfElse> if_else = std::dynamic_pointer_cast<IfElse>(block);
    if(if_else){
        return generate_if_else(if_else,writer);
    }

    Rc<While> n_while = std::dynamic_pointer_cast<While>(block);
    if(n_while){
        return generate_while(n_while,writer);
    }

    Rc<DoWhile> do_while = std::dynamic_pointer_cast<DoWhile>(block);
    if(do_while){
        return generate_do_while(do_while,writer);
    }

    return false;
}

bool CodeGen::generate_if(const Rc<If> & node,SourceWriter & writer){
    return false;
}

bool CodeGen::generate_if_else(const Rc<IfElse> & node,SourceWriter & writer){
    return false;
}

bool CodeGen::generate_while(const Rc<While> & node,SourceWriter & writer){
    return false;
}

bool CodeGen::generate_do_while(const Rc<DoWhile> & node,SourceWriter & writer){
    return false;
}

bool CodeGen::generate_instruction(const Instruction & instr,SourceWriter & writer){
    bool res = false;

    switch (instr.opcode.Value().EffectiveOpCode()){
    case OpCode::Id::ADD:{
        writer.write("//ADD");
        res = false;
    }
    case OpCode::Id::DP3:{
        writer.write("//DP3");
        res = false;
    }
    case OpCode::Id::DP4:{
        writer.write("//DP4");
        res = false;
    }
    case OpCode::Id::DPH:{
        writer.write("//DPH");
        res = false;
    }
    case OpCode::Id::DST:{
        writer.write("//DST");
        res = false;
    }
    case OpCode::Id::EX2:{
        writer.write("//EX2");
        res = false;
    }
    case OpCode::Id::LG2:{
        writer.write("//LG2");
        res = false;
    }
    case OpCode::Id::LIT:{
        writer.write("//LIT");
        res = false;
    }
    case OpCode::Id::MUL:{
        writer.write("//MUL");
        res = false;
    }
    case OpCode::Id::SGE:{
        writer.write("//SGE");
        res = false;
    }
    case OpCode::Id::SLT:{
        writer.write("//SLT");
        res = false;
    }
    case OpCode::Id::FLR:{
        writer.write("//FLR");
        res = false;
    }
    case OpCode::Id::MAX:{
        writer.write("//MAX");
        res = false;
    }
    case OpCode::Id::MIN:{
        writer.write("//MIN");
        res = false;
    }
    case OpCode::Id::RCP:{
        writer.write("//RCP");
        res = false;
    }
    case OpCode::Id::RSQ:{
        writer.write("//RSQ");
        res = false;
    }
    case OpCode::Id::MOVA:{
        writer.write("//MOVA");
        res = false;
    }
    case OpCode::Id::MOV:{
        writer.write("//MOV");
        res = false;
    }
    case OpCode::Id::DPHI:{
        writer.write("//DPHI");
        res = false;
    }
    case OpCode::Id::DSTI:{
        writer.write("//DSTI");
        res = false;
    }
    case OpCode::Id::SGEI:{
        writer.write("//SGEI");
        res = false;
    }
    case OpCode::Id::SLTI:{
        writer.write("//SLTI");
        res = false;
    }
    case OpCode::Id::BREAK:{
        writer.write("break;");
        writer.write("//BREAK");
        res = true;
    }
    case OpCode::Id::NOP:{
        writer.write("//NOP");
        res = true;
    }
    case OpCode::Id::END:{
        /// TODO: Handle function calls which end shader execution
        writer.write("return;");
        writer.write("//END");
        res = true;
    }
    case OpCode::Id::BREAKC:{
        writer.write("//BREAKC");
        res = false;
    }
    case OpCode::Id::CALL:{
        writer.write("//CALL");
        res = true;
    }
    case OpCode::Id::CALLC:{
        writer.write("//CALLC");
        res = true;
    }
    case OpCode::Id::CALLU:{
        writer.write("//CALLU");
        res = true;
    }
    case OpCode::Id::IFU:{
        writer.write("//IFU");
        res = true;
    }
    case OpCode::Id::IFC:{
        writer.write("//IFC");
        res = true;
    }
    case OpCode::Id::LOOP:{
        writer.write("//LOOP");
        res = true;
    }
    case OpCode::Id::EMIT:{
        writer.write("//EMIT");
        res = false;
    }
    case OpCode::Id::SETEMIT:{
        writer.write("//SETEMIT");
        res = false;
    }
    case OpCode::Id::JMPC:{
        writer.write("//JMPC");
        res = true;
    }
    case OpCode::Id::JMPU:{
        writer.write("//JMPU");
        res = true;
    }
    case OpCode::Id::CMP:{
        writer.write("//CMP");
        res = false;
    }
    case OpCode::Id::MADI:{
        writer.write("//MADI");
        res = false;
    }
    case OpCode::Id::MAD:{
        writer.write("//MAD");
        res = false;
    }
    default:
        res = false;
    }
    if(!res){
        std::cout << "UNMACHTED INSTRUCTION:" <<  instr.opcode.Value().GetInfo().name << std::endl;
    }
    return res;
}

}}}

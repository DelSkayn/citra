#pragma once

#include <nihstro/shader_bytecode.h>
#include "video_core/shader/decompiler/decompiler.h"
#include "video_core/shader/decompiler/structurizer.h"

using nihstro::Instruction;
using nihstro::RegisterType;
using nihstro::SourceRegister;
using nihstro::DestRegister;

namespace Pica{
namespace Shader{
namespace Decompiler{

class SourceWriter{
    std::string prefix;
 public:
    std::string src;

    void push_stack();
    void pop_stack();

    void write(std::string src);
};

class SymbolTable{
public:
    std::string get_src(const SourceRegister & reg);
    std::string set_dst(const DestRegister & reg, std::string val);
};

class CodeGen{
    std::vector<std::string> proc_src;
    std::string main_src;
    bool generate_block(Rc<ASTree::Node> & node,SourceWriter & writer);
    bool generate_if(const Rc<ASTree::If> & node,SourceWriter & writer);
    bool generate_if_else(const Rc<ASTree::IfElse> & node,SourceWriter & writer);
    bool generate_while(const Rc<ASTree::While> & node,SourceWriter & writer);
    bool generate_do_while(const Rc<ASTree::DoWhile> & node,SourceWriter & writer);
    bool generate_instruction(const Instruction & instr,SourceWriter & writer);

public:
    CodeGen() = default;
    bool generate_proc(Region r,ASTree & tree);
    bool generate_main(ASTree & tree);
};

}}}

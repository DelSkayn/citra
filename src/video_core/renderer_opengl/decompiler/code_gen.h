#pragma once

#include <nihstro/shader_bytecode.h>
#include "video_core/renderer_opengl/decompiler/decompiler.h"
#include "video_core/renderer_opengl/decompiler/structurizer.h"

using nihstro::Instruction;
using nihstro::RegisterType;
using nihstro::SourceRegister;
using nihstro::DestRegister;
using nihstro::SwizzlePattern;

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
    std::array<bool,16> used_temps;
    template<typename Func>
    std::string get_src(const SourceRegister & reg,Func f){
        std::string name;
        switch (reg.GetRegisterType()){
        case RegisterType::Input:
            //TODO: handle inputs;
            ASSERT(false);
            name = "";
            break;
        case RegisterType::Temporary:
            used_temps[reg.GetIndex()] = true;
            name = "vs_temp_reg_" + std::to_string(reg.GetIndex());
            break;
        case RegisterType::FloatUniform:
            name = "vs_regs.f[" + std::to_string(reg.GetIndex()) + "]";
            break;
        default:
            ASSERT_MSG(false,"Encountered unused register type");
        }

        std::string map;
        for(int i = 0; i < 4;i++){
            map.push_back("xyzw"[(uint32_t)f(i)]);
        }
        return name + "." + map;
    }
public:
    std::string get_src1(const SourceRegister & reg,const SwizzlePattern & p);
    std::string get_src2(const SourceRegister & reg,const SwizzlePattern & p);
    std::string get_src3(const SourceRegister & reg,const SwizzlePattern & p);
    std::string set_dest(const DestRegister & reg, const SwizzlePattern & p, std::string value);
    std::string generate_temporary_definitions();
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

    static std::string get_common_code();

public:
    CodeGen() = default;
    bool generate_proc(Region r,ASTree & tree);
    bool generate_main(ASTree & tree);
};

}}}

#pragma once

#include <nihstro/shader_bytecode.h>
#include "video_core/regs_rasterizer.h"
#include "video_core/renderer_opengl/decompiler/decompiler.h"
#include "video_core/renderer_opengl/decompiler/structurizer.h"

using nihstro::DestRegister;
using nihstro::Instruction;
using nihstro::RegisterType;
using nihstro::SourceRegister;
using nihstro::SwizzlePattern;

using VSOutputAttributes = Pica::RasterizerRegs::VSOutputAttributes;

namespace Pica {
namespace Shader {
namespace Decompiler {

class SourceWriter {
    std::string prefix;

public:
    std::string src;

    void push_stack();
    void pop_stack();

    void write(std::string src);
};

class SymbolTable {
    std::array<bool, 16> used_temps;
    std::array<bool, 8> used_inputs;
    std::array<bool, 7> used_outputs;
    const VSOutputAttributes* out_map;

    template <typename Func>
    std::string get_src(const SourceRegister& reg, Func f, bool negate_src) {
        std::string name;
        switch (reg.GetRegisterType()) {
        case RegisterType::Input:
            used_inputs[reg.GetIndex()] = true;
            name = "vs_input_reg_" + std::to_string(reg.GetIndex());
            break;
        case RegisterType::Temporary:
            used_temps[reg.GetIndex()] = true;
            name = "vs_temp_reg_" + std::to_string(reg.GetIndex());
            break;
        case RegisterType::FloatUniform:
            name = "f[" + std::to_string(reg.GetIndex()) + "]";
            break;
        default:
            ASSERT_MSG(false, "Encountered unused register type");
        }

        std::string map;
        for (int i = 0; i < 4; i++) {
            map.push_back("xyzw"[(uint32_t)f(i)]);
        }
        std::string res = name + "." + map;
        return negate_src ? "-(" + res + ")" : res;
    }

public:
    std::string get_src1(const SourceRegister& reg, const SwizzlePattern& p);
    std::string get_src2(const SourceRegister& reg, const SwizzlePattern& p);
    std::string get_src3(const SourceRegister& reg, const SwizzlePattern& p);
    std::string get_dest(const DestRegister& reg);
    std::string set_dest(std::string reg, const SwizzlePattern& p, std::string value,
                         unsigned dst_comp, unsigned src_comp);
    std::string generate_variables();
    std::string generate_output_map();
    SymbolTable(const VSOutputAttributes out[7]);
};

class CodeGen {
    std::vector<std::string> proc_src;
    std::string main_src;
    const ProgramArray& program;
    const SwizzleArray& swizzle;

    bool generate_block(Rc<ASTree::Node>& node, SourceWriter& writer);
    bool generate_region(Rc<ASTree::Node>& node, SourceWriter& writer);
    bool generate_if(const Rc<ASTree::If>& node, SourceWriter& writer);
    bool generate_if_else(const Rc<ASTree::IfElse>& node, SourceWriter& writer);
    bool generate_while(const Rc<ASTree::While>& node, SourceWriter& writer);
    bool generate_do_while(const Rc<ASTree::DoWhile>& node, SourceWriter& writer);
    bool generate_instruction(const Instruction& instr, const SwizzlePattern& p,
                              SourceWriter& writer);

    std::string generate_condition(const Instruction& instr, const SwizzlePattern& swizzle,
                                   bool top) const;

    static std::string get_common_code();

    SymbolTable& table;

public:
    CodeGen(const ProgramArray& program, const SwizzleArray& swizzle, SymbolTable& table);
    bool generate_proc(Region r, ASTree& tree);
    bool generate_main(ASTree& tree);
    std::string finalize();
};

} // namespace Decompiler
} // namespace Shader
} // namespace Pica

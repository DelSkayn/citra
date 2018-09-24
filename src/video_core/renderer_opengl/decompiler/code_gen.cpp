
#include <iostream>
#include "video_core/renderer_opengl/decompiler/code_gen.h"

namespace Pica {
namespace Shader {
namespace Decompiler {

using nihstro::OpCode;

using Node = ASTree::Node;
using Seq = ASTree::Seq;
using If = ASTree::If;
using IfElse = ASTree::IfElse;
using While = ASTree::While;
using DoWhile = ASTree::DoWhile;
using Semantic = Pica::RasterizerRegs::VSOutputAttributes::Semantic;

void SourceWriter::push_stack() {
    prefix.push_back(' ');
    prefix.push_back(' ');
}
void SourceWriter::pop_stack() {
    prefix.pop_back();
    prefix.pop_back();
}
void SourceWriter::write(std::string src) {
    this->src += prefix + src + "\n";
}

std::string CodeGen::get_common_code() {
    return R"(
#version 330 core

out vec4 primary_color;
out vec2 texcoord[3];
out float texcoord0_w;
out vec4 normquat;
out vec3 view;

vec4 vs_out_reg[7];

layout (std140) uniform vs_regs{
    ivec4 i[4];
    vec4 f[96];
    bool b[16];
};

bvec2 COND = bvec2(false,false);
)";
}

SymbolTable::SymbolTable(const VSOutputAttributes* out) : out_map(out) {
    used_inputs.fill(false);
    used_temps.fill(false);
}

std::string SymbolTable::get_src1(const SourceRegister& reg, const SwizzlePattern& p) {
    return get_src(reg, [&](int comp) { return p.GetSelectorSrc1(comp); }, (bool)p.negate_src1 != false);
}

std::string SymbolTable::get_src2(const SourceRegister& reg, const SwizzlePattern& p) {
    return get_src(reg, [&](int comp) { return p.GetSelectorSrc2(comp); }, (bool)p.negate_src2 != false);
}

std::string SymbolTable::get_src3(const SourceRegister& reg, const SwizzlePattern& p) {
    return get_src(reg, [&](int comp) { return p.GetSelectorSrc3(comp); }, (bool)p.negate_src3 != false);
}

std::string SymbolTable::get_dest(const DestRegister& reg) {
    switch (reg.GetRegisterType()) {
    case RegisterType::Output:
        used_outputs[reg.GetIndex()] = true;
        return "vs_out_reg[" + std::to_string(reg.GetIndex()) + "]";
        break;
    case RegisterType::Temporary:
        used_temps[reg.GetIndex()] = true;
        return "vs_temp_reg_" + std::to_string(reg.GetIndex());
        break;
    default:
        ASSERT(false);
    }
}

std::string SymbolTable::set_dest(std::string reg, const SwizzlePattern& p, std::string value,
                                  unsigned dst_comp = 4, unsigned src_comp = 4) {
    unsigned num_dst_comps = 0;
    std::string map = "";
    for (int i = 0; i < dst_comp; i++) {
        if (p.DestComponentEnabled(i)) {
            map.push_back("xyzw"[i]);
            num_dst_comps++;
        }
    }
    if (num_dst_comps == 0) {
        return "";
    }
    return src_comp == 1 ? num_dst_comps == 1
                               ? reg + "." + map + " = " + value + ";"
                               : reg + "." + map + " = vec" + std::to_string(num_dst_comps) + "(" +
                                     value + ");"
                         : reg + "." + map + " = (" + value + ")." + map + ";";
}

std::string SymbolTable::generate_variables() {
    std::string defs;
    for (int i = 0; i < used_temps.size(); i++) {
        if (used_temps[i]) {
            defs += "vec4 vs_temp_reg_" + std::to_string(i) + " = vec4(0.0,0.0,0.0,1.0);\n";
        }
    }
    defs += "\n";
    for (int i = 0; i < used_inputs.size(); i++) {
        std::cout << used_inputs[i] << std::endl;
        if (used_inputs[i]) {
            std::string i_str = std::to_string(i);
            defs += "layout (location = " + i_str + ") in vec4 vs_input_reg_" + i_str + ";\n";
        }
    }
    return defs;
}

std::string SymbolTable::generate_output_map() {
    std::string out = "";
    auto gen_out_name = [&](Semantic sem) {
        switch (sem) {
        case Semantic::POSITION_X:
            return "gl_Position.x";
        case Semantic::POSITION_Y:
            return "gl_Position.y";
        case Semantic::POSITION_Z:
            return "gl_Position.z";
        case Semantic::POSITION_W:
            return "gl_Position.w";
        case Semantic::QUATERNION_X:
            return "normquat.x";
        case Semantic::QUATERNION_Y:
            return "normquat.y";
        case Semantic::QUATERNION_Z:
            return "normquat.z";
        case Semantic::QUATERNION_W:
            return "normquat.w";
        case Semantic::COLOR_R:
            return "primary_color.r";
        case Semantic::COLOR_G:
            return "primary_color.g";
        case Semantic::COLOR_B:
            return "primary_color.b";
        case Semantic::COLOR_A:
            return "primary_color.a";
        case Semantic::TEXCOORD0_U:
            return "texcoord[0].x";
        case Semantic::TEXCOORD0_V:
            return "texcoord[0].y";
        case Semantic::TEXCOORD1_U:
            return "texcoord[1].x";
        case Semantic::TEXCOORD1_V:
            return "texcoord[1].y";
        case Semantic::TEXCOORD2_U:
            return "texcoord[2].x";
        case Semantic::TEXCOORD2_V:
            return "texcoord[2].y";
        case Semantic::TEXCOORD0_W:
            return "texcoord0_w";
        case Semantic::VIEW_X:
            return "view.x";
        case Semantic::VIEW_Y:
            return "view.y";
        case Semantic::VIEW_Z:
            return "view.z";
        default:
            return "";
        }
    };

    for (int i = 0; i < 7; i++) {
        if (used_outputs[i]) {
            auto& map = out_map[i];
            std::string name_x = gen_out_name(map.map_x.Value());
            if (name_x != "") {
                out += name_x + " = vs_out_reg[" + std::to_string(i) + "].x;\n";
            }
            std::string name_y = gen_out_name(map.map_y.Value());
            if (name_y != "") {
                out += name_y + " = vs_out_reg[" + std::to_string(i) + "].y;\n";
            }
            std::string name_z = gen_out_name(map.map_z.Value());
            if (name_z != "") {
                out += name_z + " = vs_out_reg[" + std::to_string(i) + "].z;\n";
            }
            std::string name_w = gen_out_name(map.map_w.Value());
            if (name_w != "") {
                out += name_w + " = vs_out_reg[" + std::to_string(i) + "].w;\n";
            }
        }
    }
    return out;
}

CodeGen::CodeGen(const ProgramArray& program, const SwizzleArray& swizzle, SymbolTable& table)
    : program(program), swizzle(swizzle), table(table) {}

bool CodeGen::generate_proc(Region r, ASTree& tree) {
    SourceWriter writer;
    writer.write("void proc_" + std::to_string(r.first) + "_" + std::to_string(r.last) + "(){");
    writer.push_stack();
    if (!generate_block(tree.start, writer)) {
        return false;
    }
    writer.pop_stack();
    writer.write("}");
    proc_src.push_back(writer.src);
    std::cout << "proc src: \n" << writer.src << std::endl;
    return true;
}

bool CodeGen::generate_main(ASTree& tree) {
    SourceWriter writer;
    writer.write("void run_shader(){");
    writer.push_stack();
    if (!generate_block(tree.start, writer)) {
        return false;
    }
    writer.pop_stack();
    writer.write("}");
    main_src = writer.src;
    std::cout << "main src:\n" << main_src << std::endl;
    return true;
}

bool CodeGen::generate_block(Rc<Node>& block, SourceWriter& writer) {
    ASSERT(!block->is_end());

    Rc<Seq> seq = std::dynamic_pointer_cast<Seq>(block);
    if (seq) {
        if (!generate_block(seq->head, writer)) {
            return false;
        }
        return generate_block(seq->tail, writer);
    }

    Rc<If> n_if = std::dynamic_pointer_cast<If>(block);
    if (n_if) {
        return generate_if(n_if, writer);
    }

    Rc<IfElse> if_else = std::dynamic_pointer_cast<IfElse>(block);
    if (if_else) {
        return generate_if_else(if_else, writer);
    }

    Rc<While> n_while = std::dynamic_pointer_cast<While>(block);
    if (n_while) {
        return generate_while(n_while, writer);
    }

    Rc<DoWhile> do_while = std::dynamic_pointer_cast<DoWhile>(block);
    if (do_while) {
        return generate_do_while(do_while, writer);
    }

    return generate_region(block, writer);
}

bool CodeGen::generate_region(Rc<Node>& node, SourceWriter& writer) {
    std::cout << "FIRST " << node->first << "SECOND " << node->last << std::endl;
    for (unsigned i = node->first; i <= node->last; i++) {
        const Instruction instr = {program[i]};
        auto opcode = instr.opcode.Value().EffectiveOpCode();
        bool is_mad = opcode == OpCode::Id::MAD || opcode == OpCode::Id::MADI;
        const SwizzlePattern swizzle = {is_mad ? this->swizzle[instr.common.operand_desc_id]
                                               : this->swizzle[instr.mad.operand_desc_id]};
        if (!generate_instruction(instr, swizzle, writer)) {
            return false;
        }
    }
    return true;
}

bool CodeGen::generate_if(const Rc<If>& node, SourceWriter& writer) {
    if (!generate_region((Rc<Node>&)node, writer)) {
        return false;
    };
    const Instruction instr = {program[node->cond.instr]};
    auto opcode = instr.opcode.Value().EffectiveOpCode();
    bool is_mad = opcode == OpCode::Id::MAD || opcode == OpCode::Id::MADI;
    const SwizzlePattern swizzle = {is_mad ? this->swizzle[instr.common.operand_desc_id]
                                           : this->swizzle[instr.mad.operand_desc_id]};
    writer.write("if(" + generate_condition(instr, swizzle, node->cond.true_edge) + "){");
    writer.push_stack();
    if (!generate_block(node->body, writer)) {
        return false;
    }
    writer.pop_stack();
    writer.write("}");
    return true;
}

bool CodeGen::generate_if_else(const Rc<IfElse>& node, SourceWriter& writer) {
    if (!generate_region((Rc<Node>&)node, writer)) {
        return false;
    };
    const Instruction instr = {program[node->cond.instr]};
    auto opcode = instr.opcode.Value().EffectiveOpCode();
    bool is_mad = opcode == OpCode::Id::MAD || opcode == OpCode::Id::MADI;
    const SwizzlePattern swizzle = {is_mad ? this->swizzle[instr.common.operand_desc_id]
                                           : this->swizzle[instr.mad.operand_desc_id]};
    writer.write("if(" + generate_condition(instr, swizzle, node->cond.true_edge) + "){");
    writer.push_stack();
    if (!generate_block(node->true_body, writer)) {
        return false;
    }
    writer.pop_stack();
    writer.write("}else{");
    writer.push_stack();
    if (!generate_block(node->false_body, writer)) {
        return false;
    }
    writer.pop_stack();
    writer.write("}");
    return true;
}

bool CodeGen::generate_while(const Rc<While>& node, SourceWriter& writer) {
    return false;
}

bool CodeGen::generate_do_while(const Rc<DoWhile>& node, SourceWriter& writer) {
    return false;
}

bool CodeGen::generate_instruction(const Instruction& instr, const SwizzlePattern& p,
                                   SourceWriter& writer) {

    auto opcode = instr.opcode.Value();
    auto eff_opcode = opcode.EffectiveOpCode();
    bool res = true;
    const bool is_inverted = (0 != (opcode.GetInfo().subtype & OpCode::Info::SrcInversed));

    bool is_mad = eff_opcode == OpCode::Id::MAD || eff_opcode == OpCode::Id::MADI;

    auto src1_reg = is_mad ? instr.mad.GetSrc1(is_inverted) : instr.common.GetSrc1(is_inverted);
    auto src2_reg = is_mad ? instr.mad.GetSrc2(is_inverted) : instr.common.GetSrc2(is_inverted);
    auto dst_reg = is_mad ? instr.mad.dest.Value() : instr.common.dest.Value();
    auto dst = table.get_dest(dst_reg);
    std::cout << dst << std::endl;

    auto src1 = table.get_src1(src1_reg, p);
    auto src2 = table.get_src2(src2_reg, p);

    switch (instr.opcode.Value().EffectiveOpCode()) {
    case OpCode::Id::ADD: {
        writer.write("//ADD");
        writer.write(table.set_dest(dst, p, src1 + " + " + src2));
        break;
    }
    case OpCode::Id::DP3: {
        writer.write("//DP3");
        // TODO DP3 should only use 3 components?
        writer.write(table.set_dest(dst, p, "dot( " + src1 + ", " + src2 + ")", 4, 1));
        break;
    }
    case OpCode::Id::DP4: {
        writer.write("//DP4");
        writer.write(table.set_dest(dst, p, "dot( " + src1 + ", " + src2 + ")", 4, 1));
        break;
    }
    case OpCode::Id::DPH: {
        writer.write("//DPH");
        res = false;
        break;
    }
    case OpCode::Id::DST: {
        writer.write("//DST");
        res = false;
        break;
    }
    case OpCode::Id::EX2: {
        writer.write("//EX2");
        writer.write(table.set_dest(dst, p, "exp2( " + src1 + ")"));
        break;
    }
    case OpCode::Id::LG2: {
        writer.write("//LG2");
        writer.write(table.set_dest(dst, p, "log2( " + src1 + ")"));
        break;
    }
    case OpCode::Id::LIT: {
        writer.write("//LIT");
        res = false;
        break;
    }
    case OpCode::Id::MUL: {
        writer.write("//MUL");
        writer.write(table.set_dest(dst, p, src1 + " * " + src2));
        break;
    }
    case OpCode::Id::SGE: {
        writer.write("//SGE");
        writer.write(table.set_dest(dst, p, "step( " + src1 + ", " + src2 + ")"));
        break;
    }
    case OpCode::Id::SLT: {
        writer.write("//SLT");
        writer.write(table.set_dest(dst, p, "1.0f - step( " + src1 + ", " + src2 + ")"));
        break;
    }
    case OpCode::Id::FLR: {
        writer.write("//FLR");
        writer.write(table.set_dest(dst, p, "floor( " + src1 + ")"));
        break;
    }
    case OpCode::Id::MAX: {
        writer.write("//MAX");
        writer.write(table.set_dest(dst, p, "max( " + src1 + ", " + src2 + ")"));
        break;
    }
    case OpCode::Id::MIN: {
        writer.write("//MIN");
        writer.write(table.set_dest(dst, p, "min( " + src1 + ", " + src2 + ")"));
        break;
    }
    case OpCode::Id::RCP: {
        writer.write("//RCP");
        writer.write(table.set_dest(dst, p, "1.0f / ( " + src1 + ")"));
        break;
    }
    case OpCode::Id::RSQ: {
        writer.write("//RSQ");
        writer.write(table.set_dest(dst, p, "1.0f / sqrt( " + src1 + ")"));
        break;
    }
    case OpCode::Id::MOVA: {
        writer.write("//MOVA");
        res = false;
        break;
    }
    case OpCode::Id::MOV: {
        writer.write("//MOV");
        writer.write(table.set_dest(dst, p, src1));
        break;
    }
    case OpCode::Id::DPHI: {
        writer.write("//DPHI");
        res = false;
        break;
    }
    case OpCode::Id::DSTI: {
        writer.write("//DSTI");
        res = false;
        break;
    }
    case OpCode::Id::SGEI: {
        writer.write("//SGEI");
        res = false;
        break;
    }
    case OpCode::Id::SLTI: {
        writer.write("//SLTI");
        res = false;
        break;
    }
    case OpCode::Id::BREAK: {
        writer.write("//BREAK");
        writer.write("break;");
        break;
    }
    case OpCode::Id::NOP: {
        writer.write("//NOP");
        break;
    }
    case OpCode::Id::END: {
        /// TODO: Handle function calls which end shader execution
        writer.write("//END");
        writer.write("return;");
        break;
    }
    case OpCode::Id::BREAKC: {
        writer.write("//BREAKC");
        writer.write("if(" + generate_condition(instr, p, true) + ") { break;} ");
        break;
    }
    case OpCode::Id::CALL: {
        writer.write("//CALL");
        break;
    }
    case OpCode::Id::CALLC: {
        writer.write("//CALLC");
        break;
    }
    case OpCode::Id::CALLU: {
        writer.write("//CALLU");
        break;
    }
    case OpCode::Id::IFU: {
        writer.write("//IFU");
        break;
    }
    case OpCode::Id::IFC: {
        writer.write("//IFC");
        break;
    }
    case OpCode::Id::LOOP: {
        writer.write("//LOOP");
        break;
    }
    case OpCode::Id::EMIT: {
        writer.write("//EMIT");
        res = false;
        break;
    }
    case OpCode::Id::SETEMIT: {
        writer.write("//SETEMIT");
        res = false;
        break;
    }
    case OpCode::Id::JMPC: {
        writer.write("//JMPC");
        break;
    }
    case OpCode::Id::JMPU: {
        writer.write("//JMPU");
        break;
    }
    case OpCode::Id::CMP: {
        writer.write("//CMP");
        res = false;
        break;
    }
    case OpCode::Id::MADI: {
        writer.write("//MADI");
        std::cout << "MADI" << std::endl;
        res = false;
        break;
    }
    case OpCode::Id::MAD: {
        writer.write("//MAD");
        auto src3_reg = instr.mad.GetSrc3(is_inverted);
        auto src3 = table.get_src3(src3_reg, p);
        writer.write(table.set_dest(dst, p, src3 + " + (" + src2 + " * " + src1 + ")"));
        break;
    }
    default:
        res = false;
    }
    if (!res) {
        std::cout << "UNMACHTED INSTRUCTION: " << instr.opcode.Value().GetInfo().name << std::endl;
    }
    return res;
}

std::string CodeGen::generate_condition(const Instruction& instr, const SwizzlePattern& swizzle,
                                        bool top) const {

    using FlowOp = Instruction::FlowControlType::Op;
    using CompareOp = Instruction::Common::CompareOpType::Op;

    std::string source = top ? "(" : "!(";
    switch (instr.opcode.Value().EffectiveOpCode()) {
    case OpCode::Id::BREAKC:
    case OpCode::Id::JMPC:
    case OpCode::Id::IFC:
    case OpCode::Id::CALLC: {
        std::string x_str = instr.flow_control.refx.Value() ? "COND.x" : "!COND.x";
        std::string y_str = instr.flow_control.refx.Value() ? "COND.y" : "!COND.y";
        switch (instr.flow_control.op) {
        case FlowOp::JustX:
            source += x_str;
            break;
        case FlowOp::JustY:
            source += y_str;
            break;
        case FlowOp::Or:
        case FlowOp::And: {
            std::string and_or = instr.flow_control.op == FlowOp::Or ? "any" : "all";
            std::string bvec;
            if (instr.flow_control.refx.Value() && instr.flow_control.refy.Value()) {
                bvec = "COND";
            } else if (!instr.flow_control.refx.Value() && !instr.flow_control.refy.Value()) {
                bvec = "not(COND)";
            } else {
                bvec = "bvec2(" + x_str + ", " + y_str + ")";
            }
            source += and_or + "(" + bvec + ")";
        } break;
        }
        break;
    }
    case OpCode::Id::JMPU:
    case OpCode::Id::IFU:
    case OpCode::Id::CALLU: {
        source += std::string("b[") + std::to_string(instr.flow_control.bool_uniform_id) +
                  std::string("]");
        break;
    }
    default:
        ASSERT(false);
        return "";
    }
    source += ")";
    return source;
}

std::string CodeGen::finalize() {
    std::string res = get_common_code();
    res += table.generate_variables();

    for (auto proc : proc_src) {
        res += proc;
        res += "\n";
    }
    res += main_src;
    res += "void main(){\n";
    res += "run_shader();\n";
    res += table.generate_output_map();
    res += "}";

    std::cout << "FINAL SHADER:\n" << res << std::endl;
    return res;
}

} // namespace Decompiler
} // namespace Shader
} // namespace Pica

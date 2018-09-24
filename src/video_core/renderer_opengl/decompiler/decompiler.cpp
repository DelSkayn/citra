#include <nihstro/shader_bytecode.h>
#include "video_core/renderer_opengl/decompiler/code_gen.h"
#include "video_core/renderer_opengl/decompiler/control_flow.h"
#include "video_core/renderer_opengl/decompiler/decompiler.h"
#include "video_core/renderer_opengl/decompiler/structurizer.h"
#include "video_core/shader/shader.h"

using nihstro::Instruction;
using nihstro::OpCode;
using nihstro::RegisterType;
using nihstro::SourceRegister;
using nihstro::SwizzlePattern;

namespace Pica {
namespace Shader {
namespace Decompiler {

Option<std::string> Decompiler::decompile(const ProgramArray& program, const SwizzleArray& swizzle,
                                          unsigned entry_point, const VSOutputAttributes out[7]) {
    ASSERT(entry_point < PROGRAM_LEN);

    ProcMap<ControlFlow> proc_map;
    ControlFlow flow;
    SymbolTable table(out);
    CodeGen gen(program, swizzle, table);
    flow.build(program, entry_point, PROGRAM_LEN, proc_map);
    for (auto it = proc_map.begin(); it != proc_map.end(); ++it) {
        ASTree tree(it->second);
        tree.print();
        if (!gen.generate_proc(it->first, tree)) {
            return boost::none;
        }
    }
    ASTree tree(flow);
    if (!Structurizer::structurize(tree)) {
        return boost::none;
    }
    tree.print();
    if (!gen.generate_main(tree)) {
        return boost::none;
    }

    return gen.finalize();
}

bool Region::operator==(const Region& other) const {
    return this->first == other.first && this->last == other.last;
}

std::string Decompiler::instruction_to_string(const Instruction& instr) {

    const unsigned dest = instr.flow_control.dest_offset;
    const unsigned num = instr.flow_control.num_instructions;

    switch (instr.opcode.Value().EffectiveOpCode()) {
    case OpCode::Id::ADD:
        return "ADD";
    case OpCode::Id::DP3:
        return "DP3";
    case OpCode::Id::DP4:
        return "DP4";
    case OpCode::Id::DPH:
        return "DPH";
    case OpCode::Id::DST:
        return "DST";
    case OpCode::Id::EX2:
        return "EX2";
    case OpCode::Id::LG2:
        return "LG2";
    case OpCode::Id::LIT:
        return "LIT";
    case OpCode::Id::MUL:
        return "MUL";
    case OpCode::Id::SGE:
        return "SGE";
    case OpCode::Id::SLT:
        return "SLT";
    case OpCode::Id::FLR:
        return "FLR";
    case OpCode::Id::MAX:
        return "MAX";
    case OpCode::Id::MIN:
        return "MIN";
    case OpCode::Id::RCP:
        return "RCP";
    case OpCode::Id::RSQ:
        return "RSQ";
    case OpCode::Id::MOVA:
        return "MOVA";
    case OpCode::Id::MOV:
        return "MOV";
    case OpCode::Id::DPHI:
        return "DPHI";
    case OpCode::Id::DSTI:
        return "DSTI";
    case OpCode::Id::SGEI:
        return "SGEI";
    case OpCode::Id::SLTI:
        return "SLTI";
    case OpCode::Id::BREAK:
        return "BREAK";
    case OpCode::Id::NOP:
        return "NOP";
    case OpCode::Id::END:
        return "END";
    case OpCode::Id::BREAKC:
        return "BREAKC";
    case OpCode::Id::CALL:
        return "CALL\t" + std::to_string(dest) + "\t" + std::to_string(num);
    case OpCode::Id::CALLC:
        return "CALLC\t" + std::to_string(dest) + "\t" + std::to_string(num);
    case OpCode::Id::CALLU:
        return "CALLU\t" + std::to_string(dest) + "\t" + std::to_string(num);
    case OpCode::Id::IFU:
        return "IFU\t" + std::to_string(dest) + "\t" + std::to_string(num);
    case OpCode::Id::IFC:
        return "IFC\t" + std::to_string(dest) + "\t" + std::to_string(num);
    case OpCode::Id::LOOP:
        return "LOOP\t" + std::to_string(dest) + "\t" + std::to_string(num);
    case OpCode::Id::EMIT:
        return "EMIT";
    case OpCode::Id::SETEMIT:
        return "SETEMIT";
    case OpCode::Id::JMPC:
        return "JMPC\t" + std::to_string(dest);
    case OpCode::Id::JMPU:
        return "JMPU\t" + std::to_string(dest);
    case OpCode::Id::CMP:
        return "CMP";
    case OpCode::Id::MADI:
        return "MADI";
    case OpCode::Id::MAD:
        return "MAD";
    default:
        return "invalid instruction";
    }
}

} // namespace Decompiler
} // namespace Shader
} // namespace Pica

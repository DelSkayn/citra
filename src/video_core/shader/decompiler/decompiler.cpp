#include <array>
#include <cstring>
#include "core/core.h"
#include "video_core/shader/shader.h"
#include "video_core/shader/decompiler/decompiler.h"
#include "video_core/shader/decompiler/dominates.h"
#include "video_core/shader/decompiler/control_flow.h"

using nihstro::OpCode;
using nihstro::Instruction;
using nihstro::RegisterType;
using nihstro::SourceRegister;
using nihstro::SwizzlePattern;

namespace Pica{
namespace Shader{
namespace Decompiler{

    Decompiler::Decompiler(std::array<unsigned,MAX_PROGRAM_CODE_LENGTH> * program_code
                           , std::array<unsigned,MAX_SWIZZLE_DATA_LENGTH> * swizzle_data
                           , int entry_point){
        this->program_code = program_code;
        this->swizzle_data = swizzle_data;
        this->entry_point = entry_point;
    }

    bool Decompiler::Decompile(){
        ASSERT(this->program_code != nullptr);
        ASSERT(this->swizzle_data != nullptr);
        ControlFlow flow(program_code,entry_point);
        flow.build();
        Dominates dominate = Dominates();
        dominate.build(flow);
    }

    /*
    std::string Decompiler::DecompileBlock(ControlFlow::CodeBlock * block){
        ASSERT(block != nullptr);
        std::string source = DecompileRegion(block->first,block->last);
        if(block->branch == nullptr){
            if(block->exit != nullptr){
                source += DecompileBlock(block->exit);
            }
        }else{
        }
        return source;
    }

    std::string Decompiler::DecompileRegion(unsigned first,unsigned last){
        std::string source = "";
        for(int i = first;i <= last;i++){
            const Instruction instr = {(*program_code)[i]};
            const SwizzlePattern swizzle = {(*swizzle_data)[instr.common.operand_desc_id]};
            source += DecompileInstruction(instr,swizzle);
        }
        return source;
    }

    std::string Decompiler::DecompileInstruction(Instruction instr,SwizzlePattern swizzle){
        std::string source = "";
        switch (instr.opcode.Value().EffectiveOpCode()){
        case OpCode::Id::ADD:
            source += R"(ADD)";
            source += "\n";
            break;
        case OpCode::Id::DP3:
            source += R"(DP3)";
            source += "\n";
            break;
        case OpCode::Id::DP4:
            source += R"(DP4)";
            source += "\n";
            break;
        case OpCode::Id::DPH:
            source += R"(DPH)";
            source += "\n";
            break;
        case OpCode::Id::DST:
            source += R"(DST)";
            source += "\n";
            break;
        case OpCode::Id::EX2:
            source += R"(EX2)";
            source += "\n";
            break;
        case OpCode::Id::LG2:
            source += R"(LG2)";
            source += "\n";
            break;
        case OpCode::Id::LIT:
            source += R"(LIT)";
            source += "\n";
            break;
        case OpCode::Id::MUL:
            source += R"(MUL)";
            source += "\n";
            break;
        case OpCode::Id::SGE:
            source += R"(SGE)";
            source += "\n";
            break;
        case OpCode::Id::SLT:
            source += R"(SLT)";
            source += "\n";
            break;
        case OpCode::Id::FLR:
            source += R"(FLR)";
            source += "\n";
            break;
        case OpCode::Id::MAX:
            source += R"(MAX)";
            source += "\n";
            break;
        case OpCode::Id::MIN:
            source += R"(MIN)";
            source += "\n";
            break;
        case OpCode::Id::RCP:
            source += R"(RCP)";
            source += "\n";
            break;
        case OpCode::Id::RSQ:
            source += R"(RSQ)";
            source += "\n";
            break;
        case OpCode::Id::MOVA:
            source += R"(MOVA)";
            source += "\n";
            break;
        case OpCode::Id::MOV:
            source += R"(MOV)";
            source += "\n";
            break;
        case OpCode::Id::DPHI:
            source += R"(DPHI)";
            source += "\n";
            break;
        case OpCode::Id::DSTI:
            source += R"(DSTI)";
            source += "\n";
            break;
        case OpCode::Id::SGEI:
            source += R"(SGEI)";
            source += "\n";
            break;
        case OpCode::Id::SLTI:
            source += R"(SLTI)";
            source += "\n";
            break;
        case OpCode::Id::BREAK:
            source += R"(BREAK)";
            source += "\n";
            break;
        case OpCode::Id::NOP:
            source += R"(NOP)";
            source += "\n";
            break;
        case OpCode::Id::END:
            source += R"(END)";
            source += "\n";
            break;
        case OpCode::Id::BREAKC:
            source += R"(BREAKC)";
            source += "\n";
            break;
        case OpCode::Id::CALL:
            source += R"(CALL)";
            source += "\n";
            break;
        case OpCode::Id::CALLC:
            source += R"(CALLC)";
            source += "\n";
            break;
        case OpCode::Id::CALLU:
            source += R"(CALLU)";
            source += "\n";
            break;
        case OpCode::Id::IFU:
            source += R"(IFU)";
            source += "\n";
            break;
        case OpCode::Id::IFC:
            source += R"(IFC)";
            source += "\n";
            break;
        case OpCode::Id::LOOP:
            source += R"(LOOP)";
            source += "\n";
            break;
        case OpCode::Id::EMIT:
            source += R"(EMIT)";
            source += "\n";
            break;
        case OpCode::Id::SETEMIT:
            source += R"(SETEMIT)";
            source += "\n";
            break;
        case OpCode::Id::JMPC:
            source += R"(JMPC)";
            source += "\n";
            break;
        case OpCode::Id::JMPU:
            source += R"(JMPU)";
            source += "\n";
            break;
        case OpCode::Id::CMP:
            source += R"(CMP)";
            source += "\n";
            break;
        case OpCode::Id::MADI:
            source += R"(MADI)";
            source += "\n";
            break;
        case OpCode::Id::MAD:
            source += R"(MAD)";
            source += "\n";
            break;
        default:
            source += R"(UNKNOWN INSTRUCTION!!
)";
        }
        return source;
    }
    */

}
}
}

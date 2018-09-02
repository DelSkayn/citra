#include <array>
#include <cstring>
#include "core/core.h"
#include "video_core/shader/shader.h"
#include "video_core/shader/decompiler/decompiler.h"
#include "video_core/shader/decompiler/control_flow.h"

using nihstro::OpCode;
using nihstro::Instruction;
using nihstro::RegisterType;
using nihstro::SourceRegister;
using nihstro::SwizzlePattern;

namespace Pica{
namespace Shader{
namespace Decompiler{

    Decompiler::Decompiler(CodeArray * program_code,SwizzleArray * swizzle_data,int entry_point){
        this->program_code = program_code;
        this->swizzle_data = swizzle_data;
        this->entry_point = entry_point;
        this->error = Error::None;
    }

    bool Decompiler::Decompile(){
        ASSERT(this->program_code != nullptr);
        ASSERT(this->swizzle_data != nullptr);
        ControlFlow flow(program_code,entry_point);
        if(flow.Analyze()){
            LOG_DEBUG(HW_GPU,"ControlFlow encountered an error while decompiling shader: %s."
                      , ControlFlow::ErrorToString(flow.error));
            return true;
        }
        ControlFlow::CodeBlock * first = flow.first;
        GlslControlFlow flow[MAX_PROGRAM_CODE_LENGTH];
        unsigned flow_count = 0;
        LOG_DEBUG(HW_GPU,"source:\n%s",DecompileBlock(first).c_str());
        if(this->error != Error::None){
            return true;
        }
        return false;
    }

    bool Decompiler::GenerateGlslFlow(CodeBlock & block){
        if (block == nullptr){
            return;
        }

        if(block->exit == nullptr){
            LOG_WARN(HW_GPU,"found block with nullptr exit should not happen");
            return;
        }

        if(block->exit == )

        CodeBlock exit = *block->exit;
        CodeBlock branch = *block->branch;

        bool is_if
            = branch->exit == nullptr
            && branch->branch == nullptr;




    }

    GlslControlFlow Decompiler::Recognize(CodeBlock * block){
        bool is_sequential
            = block->branch == nullptr
            && block->exit != nullptr;

        if(is_sequential)
            return GlslControlFlow::Sequential;

        //
        // |
        // 0
        //
        bool is_exiting
            = block->branch == nullptr
            && block->exit == nullptr;

        if(is_sequential)
            return GlslControlFlow::Exiting;

        CodeBlock * branch = block->branch;
        CodeBlock * exit = block->exit;

        //
        //   |
        //   0
        //  / \
        // 0   0
        //  \ /
        //   0
        //   |
        //
        bool is_if_else
            = branch->branch == nullptr
            && exit->branch == nullptr
            && branch->exit == exit->exit;
        if(is_if_else)
            return GlslControlFlow::IfElse;

        //
        // |
        // 0
        // |\
        // 0 )
        // |/
        // 0
        // |
        //
        bool is_if
            = exit->branch == nullptr
            && exit->exit == branch;
        if(is_if)
            return GlslControlFlow::IfElse;

        //
        //   |
        //   0
        //  /|\
        //  \| |
        //   0 |
        //    /
        //   0
        //   |
        //
        bool is_while
            = exit->branch == nullptr
            && exit->exit == block;

        if(is_while)
            return GlslControlFlow::While;
    }

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

}
}
}

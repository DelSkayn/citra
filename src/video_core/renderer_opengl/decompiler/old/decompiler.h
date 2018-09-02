#pragma once

#include <array>
#include <nihstro/shader_bytecode.h>
#include "video_core/shader/shader.h"
#include "video_core/shader/decompiler/control_flow.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

    typedef std::array<u32,MAX_PROGRAM_CODE_LENGTH> CodeArray;
    typedef std::array<u32, MAX_SWIZZLE_DATA_LENGTH> SwizzleArray;

    class Decompiler{
        enum Error{
            UnknownInstruction,
            None,
        };

        enum GlslControlType{
            If,
            IfElse,
            While,
            Sequential,
            Exiting,
            Unknown,
        }

        struct SyntaxTree{
            unsigned first;
            unsigned end;
            GlslControlFlow * flow;
        }

        struct SyntaxNode{
            GlslControlType type;

            Scope first;
            Scope second;

            SyntaxNode * next;
        }

        CodeArray * program_code;
        SwizzleArray * swizzle_data;

        int entry_point;
        Decompiler();

        std::string DecompileBlock(ControlFlow::CodeBlock * block);
        std::string DecompileRegion(unsigned first, unsigned last);
        std::string DecompileInstruction(nihstro::Instruction instr, nihstro::SwizzlePattern swizzle);

        IsDisjoint()
    public:
        Error error;


        Decompiler(CodeArray * program_code,SwizzleArray * swizzle_data,int entry_point);
        bool Decompile();
    };

}
}
}

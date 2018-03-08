#pragma once

#include "video_core/shader/decompiler/decompiler.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

constexpr unsigned MAX_BLOCK_ENTRIES = 4;

class ControlFlow{
    public:

        enum BlockType{
            // Does not branch;
            Sequential = 0,
            If,
            IfElse,
            For,
            CondCall,
        }

        struct ShaderBlock{
            unsigned first;
            unsigned last;

            std::array<ShaderBlock *,MAX_BLOCK_ENTRIES> in;
            ShaderBlock * out;
            ShaderBlock * branch;
            BlockType ty;
            int flag;

            void PushIn(ShaderBlock * block);
        }


        ControlFlow(CodeArray & program_code,int entry_point);
        void Analyze();
        ShaderBlock * First();
    private:
        void AnalyzeBlock(ShaderBlock * block);

        int next_block_index;
        std::array<ShaderBlock,MAX_PROGRAM_CODE_LENGTH> blocks;
        CodeArray & program_code;
}

}// namespace
}// namespace
}// namespace

#pragma once

#include "video_core/shader/decompiler/decompiler.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

constexpr unsigned MAX_BLOCK_ENTRIES = 4;

class ControlFlow{
    public:

        enum BlockType{
            If,
            IfElse,
            For,
            // Does not branch;
            Sequential;
        }

        struct ShaderBlock{
            unsigned first;
            unsigned last;

            std::array<ShaderBlock *,MAX_BLOCK_ENTRIES> in;
            ShaderBlock * out;
            ShaderBlock * branch;
            BlockType ty;

            ShaderBlock(unsigned first ,unsigned last,BlockType ty ,ShaderBlock * out,ShaderBlock * branch = nullptr);
            void PushIn(ShaderBlock * block);
        }

        bool Analyze();
        ShaderBlock * First();

        DecompileError error;
        ControlFlow(CodeArray & program_code,int entry_point);

    private:
        void Split(int index,int branch_instr = MAX_PROGRAM_CODE_LENGTH);
        ShaderBlock * Alloc(ShaderBlock);
        int entry_point;
        int program_counter;

        int next_block_index;
        std::array<ShaderBlock,MAX_PROGRAM_CODE_LENGTH> blocks;

        ShaderBlock * current_block;
        CodeArray * program_code;
}

}// namespace
}// namespace
}// namespace

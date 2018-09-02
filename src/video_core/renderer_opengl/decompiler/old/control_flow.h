#pragma once

#include <array>
#include "video_core/shader/shader.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

//
// # ControlFlow
//
// THis part of the code creates a graph that represents the code paths the program might take.
// A block is a single part of the code in which no branching takes place and is always executed as a whole.
// Thus code blocks start with an incoming instruction from a separate part of the program. // End and with the end of the program or a branching instruction.
//
// # Code flow in PICA
//
// Pica has five instructions which impact control flow.
//
// * JMP
// * IF
// * CALL
// * LOOP
// * END
//
// JMP allows jumping to a specific part of the code.
// Either always or conditional.
//
// IF Also jumps to a specific part of the code but also allows specifying a number of instructions after which
// the program will jump back
//
//  # Edge cases
//
//  1) What if a program jumps over a returning instruction?
//
//      Interperter:
//          Continues executing instructions as if the jump back has never taken place.
//
//      We:
//          Trow error and fallback.
//
//  2) What if we have a JMP with retreating edge.
//
//      Interperter:
//          Handles it normaly since it has no problems with jumping code
//
//      We:
//          3 options:
//              1) Have some optimization which rewrites retreating edges.
//                 Convert it to something like a do while loop.
//              2) Treat backward edges as if there a different part of the code.
//              3) Dont handle it and fallback.
//
//
//

class ControlFlow{
    static const unsigned MAX_BLOCK_INPUT = 4;
    typedef std::array<u32,MAX_PROGRAM_CODE_LENGTH> CodeArray;
    public:
        enum ControlFlowError{
            None,
            CallToAlreadyEncountered,
            NotSupportedInstruction,
        };

        struct CodeBlock{
            // A pointer to the block to which this block can jump to.
            // Can be null if the codeblock does not branch
            CodeBlock * branch;

            // The pointer to the "normal" next block.
            // Will only be null if the codeblock exits the program in this block.
            CodeBlock * exit;

            // The index of the first instruction which is part of this block
            unsigned first;

            // The index of the last instruction which is part of this block
            unsigned last;

            CodeBlock * in[MAX_BLOCK_INPUTS];
            unsigned num_in;

            bool encountered;
        };

        typedef std::array<CodeBlock,MAX_PROGRAM_CODE_LENGTH> BlockArray;
        typedef std::array<CodeBlock *,MAX_PROGRAM_CODE_LENGTH> BlockPointerArray;

        CodeBlock * first;
        ControlFlowError error;

        ControlFlow(CodeArray * program_code,int entry_point);

        bool Analyze();

        static std::string ErrorToString(ControlFlowError error);
    private:
        ControlFlow();

        void Finalize();
        void AnalyzeBlock(CodeBlock * start,BlockPointerArray & pending,int & num_pending);


        CodeBlock * block(unsigned first);
        CodeBlock * find(unsigned first);
        CodeBlock * allocate(CodeBlock * previous,unsigned first);

        static ConnectIns(CodeBlock * block);


        int num_blocks;
        BlockArray blocks;
        BlockPointerArray block_order;

        int entry_point;
        CodeArray * program_code;
};

}// namespace
}// namespace
}// namespace

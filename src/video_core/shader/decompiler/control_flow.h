#pragma once

#include <array>
#include <vector>
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
    public:
        struct CodeBlock{
            unsigned first;
            unsigned last;
        };

        struct ProcCall;

        ControlFlow(std::array<unsigned,MAX_PROGRAM_CODE_LENGTH> * program_code
                    , unsigned entry_point
                    , unsigned end = MAX_PROGRAM_CODE_LENGTH);
        void build();
        int find(int in);
        std::array<CodeBlock,MAX_PROGRAM_CODE_LENGTH> blocks;
        std::array<std::vector<unsigned>,MAX_PROGRAM_CODE_LENGTH> in;
        std::array<std::vector<unsigned>,MAX_PROGRAM_CODE_LENGTH> out;
        std::vector<ProcCall> proc_calls;
        int entry_point;
        unsigned num_blocks;
    private:
        ControlFlow();

        void create_blocks();
        void connect_blocks();
        void edge(int from,int to);
        void exit(int from);
        void split(int in);


        std::array<unsigned,MAX_PROGRAM_CODE_LENGTH> * program_code;
};

struct ControlFlow::ProcCall{
    unsigned instr;
    ControlFlow flow;
};

}// namespace
}// namespace
}// namespace

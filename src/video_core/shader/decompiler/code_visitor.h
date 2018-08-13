#pragma once

#include <vector>
#include <array>
#include "video_core/shader/decompiler/decompiler.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

class CodeVisitor{
    ProgramArray & array;
    std::vector<Region> stack;
    std::array<bool,PROGRAM_LEN> reached;


    ControlFlow();
public:
    CodeVisitor(ProgramArray & array,unsigned first,unsigned last);
    Option<Instruction> next();
};

}}}

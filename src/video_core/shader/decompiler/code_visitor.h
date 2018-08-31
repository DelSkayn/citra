#pragma once

#include <vector>
#include <array>
#include <nihstro/shader_bytecode.h>
#include "video_core/shader/decompiler/decompiler.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

using Instruction = nihstro::Instruction;

class CodeVisitor{
    ProgramArray & array;
    std::vector<Region> stack;
    std::array<bool,PROGRAM_LEN> reached;


    CodeVisitor();
public:
    CodeVisitor(ProgramArray & array,unsigned first,unsigned last);
    Option<Instruction> next();
    Option<unsigned> next_index();
};

}}}

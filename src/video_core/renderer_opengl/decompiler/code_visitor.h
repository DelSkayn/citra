#pragma once

#include <vector>
#include <array>
#include <nihstro/shader_bytecode.h>
#include "video_core/renderer_opengl/decompiler/decompiler.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

using Instruction = nihstro::Instruction;

class CodeVisitor{
    const ProgramArray & array;
    std::vector<Region> stack;
    std::array<bool,PROGRAM_LEN> reached;


    CodeVisitor();
public:
    CodeVisitor(const ProgramArray & array,unsigned first,unsigned last);
    Option<Instruction> next();
    Option<unsigned> next_index();
};

}}}

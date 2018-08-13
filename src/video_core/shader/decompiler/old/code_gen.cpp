#pragma once

#include "video_core/shader/decompiler/decompiler.h"
#include "video_core/shader/decompiler/structurizer.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

class SymbolTable{
private:
    std::array<Option<std::string>,MAX_INPUT_REG> inputs;
    std::array<Option<std::string>,MAX_OUTPUT_REG> outputs;
public:
    std::string set_dest();
    std::string get_src();
}

class CodGenerator{
public:
    bool gen_main(ASTree & tree);
    bool gen_function(Region r,ASTree & tree);
private:
    std::vector<std::string> function_src;
    std::string main_src;
}


}}}

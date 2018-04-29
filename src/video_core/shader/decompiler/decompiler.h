#pragma once

#include <array>
#include <nihstro/shader_bytecode.h>
#include "video_core/shader/shader.h"
#include "video_core/shader/decompiler/control_flow.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

    class Decompiler{
        std::array<unsigned,MAX_PROGRAM_CODE_LENGTH> * program_code;
        std::array<unsigned,MAX_SWIZZLE_DATA_LENGTH> * swizzle_data;
        int entry_point;

    public:
        Decompiler(std::array<unsigned,MAX_PROGRAM_CODE_LENGTH> * program_code
                   , std::array<unsigned,MAX_SWIZZLE_DATA_LENGTH> * swizzle_data
                   , int entry_point);
        bool Decompile();
    };

}
}
}

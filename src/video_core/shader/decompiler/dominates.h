

#pragma once

#include <array>
#include "video_core/shader/shader.h"
#include "video_core/shader/decompiler/control_flow.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

class Dominates{
    std::array<std::vector<unsigned>,MAX_PROGRAM_CODE_LENGTH> dominates;
    Dominates();
 public:
    class DominatesBuilder{
        class Data{
            std::array<unsigned,MAX_PROGRAM_CODE_LENGTH> dominates;
            unsigned len;
        };
        std::array<std::vector<unsigned>,MAX_PROGRAM_CODE_LENGTH> dominates;
        void build(ControlFlow & flow);
        void push(int block,int dom);
    };

    void calc(ControlFlow & flow);
}

}
}
}

#pragma once

#include <array>
#include <vector>
#include "video_core/shader/decompiler/control_flow.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

class PostOrder{
        void build_impl(ControlFlow & flow,unsigned block,std::array<bool,MAX_PROGRAM_CODE_LENGTH> & reached);
    public:
        void build(ControlFlow & flow);
        std::array<unsigned,MAX_PROGRAM_CODE_LENGHT> order;
        std::array<unsigned,MAX_PROGRAM_CODE_LENGHT> order_index;
        unsigned len;
        PostOrder();
};

}
}
}

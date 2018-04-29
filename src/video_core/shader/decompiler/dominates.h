

#pragma once

#include <array>
#include "video_core/shader/shader.h"
#include "video_core/shader/decompiler/control_flow.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

class Dominates{
    std::array<unsigned,MAX_PROGRAM_CODE_LENGTH> dominated_by;
    unsigned start_block;
    struct DominatesBuilder{
        std::array<unsigned,MAX_PROGRAM_CODE_LENGTH> dominated_by;
        std::array<unsigned,MAX_PROGRAM_CODE_LENGTH> post_order;
        std::array<unsigned,MAX_PROGRAM_CODE_LENGTH> post_order_index;
        unsigned post_order_len;
        unsigned start_block;

        void gen_post_order(ControlFlow & flow);
        void gen_post_order_impl(ControlFlow & flow,unsigned block,std::array<bool,MAX_PROGRAM_CODE_LENGTH> & reached);
        unsigned intersect(unsigned n1,unsigned n2);
        void build(ControlFlow & flow);
    };

 public:
    Dominates();
    void build(ControlFlow & flow);

    bool is_dominated_by(unsigned block,unsigned dom);

    unsigned immediate_dom(unsigned block);
};
}
}
}

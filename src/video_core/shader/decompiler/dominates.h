

#pragma once

#include <array>
#include "video_core/shader/shader.h"
#include "control_flow.h"
#include "post_order.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

class Dominates{
    std::array<unsigned,MAX_PROGRAM_CODE_LENGTH> dominated_by;
    unsigned dom_len;
    std::array<unsigned,MAX_PROGRAM_CODE_LENGTH> pre_dominated_by;
    unsigned pre_dom_len;
    unsigned start_block;

    unsigned intersect(unsigned n1,unsigned n2);
    void build(ControlFlow & flow,PostOrder & order);

 public:
    Dominates();

    bool is_dominated_by(unsigned block,unsigned dom);
    bool is_pre_dominated_by(unsigned block,unsigned dom);

    unsigned immediate_dom(unsigned block);
    unsigned immediate_pred_dom(unsigned block);
};
}
}
}

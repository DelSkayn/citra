#include "post_order.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

PostOrder::PostOrder(){
    this->order = {0};
    this->len = 0;
}

void PostOrder::build(ControlFlow & flow){
    int start_block = flow.find(flow.entry_point);
    std::array<bool,MAX_PROGRAM_CODE_LENGTH> reached = {false};
    build_impl(flow,start_block,reached);
    for(int i = 0;i < len;i++){
        order_index[order[i]] = i;
    }
}

void PostOrder::build_impl(ControlFlow & flow,unsigned block,std::array<bool,MAX_PROGRAM_CODE_LENGTH> & reached){
    if(reached[block]){
        return;
    }
    reached[block] = true;
    for(int i = 0;i < flow.out[block].size();i++){
        unsigned reach_block = flow.out[block][i];
        if(reach_block != MAX_PROGRAM_CODE_LENGTH){
            build_impl(flow,reach_block,reached);
        }
    }
    order[len++] = block;
}

}
}
}

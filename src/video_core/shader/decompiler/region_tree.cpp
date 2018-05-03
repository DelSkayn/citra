
#include "region_tree.h"
#include <functional>

namespace Pica{
namespace Shader{
namespace Decompiler{
using ControlFlow;

RegionTree::RegionTree(){
    this->nodes = {0};
    this->node_len = 0;
}

void RegionTree::build(ControlFlow & flow
        , Dominates & dom){

    unsigned current = flow.find(flow.entry_point);
    Reached reached = {false};
    this->root = build_impl(reached,this->regions,current,MAX_PROGRAM_CODE_LENGTH);
}

RegionTree::Region RegionTree::build_impl(ControlFlow & flow, Dominates & dom,Reached & reached,unsigned current,unsigned last){
    reached[current] = true;
    unsigned pre_dom = dom.immediate_pred_dom(current);
    if(dom.immediate_dom(pre_dom) == current){
        //We have a region;
        Region new_region;
        new_region.first = current;
        new_region.last = pre_dom;
        for(unsigned out: flow.out[current]){
            new_region.push_back(build_impl(reached,new_region.regions,out,pre_dom));
        }
        return new_region;
    }

    for(unsigned out: flow.out[current]){
        if(out != MAX_PROGRAM_CODE_LENGTH
            && out != last;
            && !reached[out]){

            return build_impl(reached,region,out,last);
        }
    }
}


}
}
}

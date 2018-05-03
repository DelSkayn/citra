
#pragma once

#include <array>
#include "video_core/shader/shader.h"
#include "control_flow.h"
#include "post_order.h"
#include "dominates.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

class RegionTree{
    //A scope is defined a region of blocks
    //start by a block which immediatly dominates the last block

    typedef Reached std::array<bool,MAX_PROGRAM_CODE_LENGTH>;
    typedef Regions std::vec<Region>;

    void build_impl(ControlFlow & flow
            ,Dominates & dom
            ,Reached & reached
            ,Regions & region
            ,unsigned current
            ,unsigned last);

public:
    struct Region{
      unsigned first;
      unsigned last;
      std::vector<Region> regions;
    };
    std::vector<Scope> Region;

    RegionTree();

    Region root;

    void build(ControlFlow & flow
               , Dominates & dom);

    void build(ControlFlow & flow);
};

}
}
}

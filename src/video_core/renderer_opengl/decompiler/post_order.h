
#pragma once

#include<unordered_set>

#include "video_core/renderer_opengl/decompiler/decompiler.h"
#include "video_core/renderer_opengl/decompiler/control_flow.h"


namespace Pica{
namespace Shader{
namespace Decompiler{

class PostOrder{
    PostOrder();
    void build(Rc<ControlFlow::Node> & node,std::unordered_set<unsigned> & reached);
public:
    PostOrder(ControlFlow & flow);
    std::vector<Rc<ControlFlow::Node>> order;
};

}}}

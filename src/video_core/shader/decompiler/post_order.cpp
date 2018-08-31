
#include "video_core/shader/decompiler/decompiler.h"
#include "video_core/shader/decompiler/post_order.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

PostOrder::PostOrder(ControlFlow & flow){
    std::unordered_set<unsigned> reached;
    this->build(flow.start,reached);
}

void PostOrder::build(Rc<ControlFlow::Node> & node,std::unordered_set<unsigned> & reached){
    if (reached.find(node->first) == reached.end()){
        return;
    }
    reached.insert(node->first);
    for(auto out: node->out){
        build(out->to,reached);
    }
    this->order.push_back(node);
}

}}}

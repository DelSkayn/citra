
#include "scope_tree.h"

namespace Pica{
namespace Shader{
namespace Decompiler{
using ControlFlow;

ScopeTree::ScopeTree(){
    this->nodes = {0};
    this->node_len = 0;
}

void ScopeTree::build(ControlFlow & flow
           , Dominates & dom
           , std::array<unsigned,MAX_PROGRAM_CODE_LENGTH> * code){

    unsigned block = flow.find(flow.first);

}

void ScopeTree::build_scope(Scope & scope,ControlFlow & flow,Dominates & dominates,unsigned block){
    
}

}
}
}


#pragma once

#include <array>
#include "video_core/shader/shader.h"
#include "video_core/shader/decompiler/control_flow.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

class ScopeTree{
public:

    struct Scope{
        ScopeType ty;
        ControlFlow::CodeBlock * start;
        ControlFlow::CodeBlock * end;
        std::vector<Scope> scopes;
    };


    ScopeTree();

    std::vector<Scope> scopes;

    void build(ControlFlow & flow
               , Dominates & dom
               , std::array<unsigned,MAX_PROGRAM_CODE_LENGTH> * code);
};

}
}
}

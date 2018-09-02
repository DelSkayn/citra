
#pragma once

#include <array>
#include "video_core/shader/shader.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

    class SyntaxTree{
    public:
        enum NodeType{
            If,
            IfElse,
            While,
            Seq,
            Exiting,
            Composite,
            Unknown,
        };

        struct Scope{
            unsigned start;
            unsigned end;
        };

        struct If{
            Node * body;
            unsigned instr;
        };

        struct IfElse{
            Node * body;
            Node * else_body;
            unsigned instr;
        };

        struct While{
            Node * body;
            unsigned instr;
        };

        struct Sequential{
            unsigned first;
            unsigned last;
        };

        struct Exiting{
            unsigned instr;
        }

        union NodeData{
            If _if;
            IfElse _if_else;
            While _while;
            Sequential _seq;
            Exiting _exit;
        };

        struct Node{
            NodeType
            NodeData data;
            Node * next
        };

        Node * First();

        bool Build(ControlFlow & control_flow);
    private:
        std::array<CodeBlock,MAXPROGRAM_CODE_LENGTH> nodes;
        unsigned num_nodes;
    };
}
}
}

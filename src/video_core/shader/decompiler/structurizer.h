#pragma once

#include "video_core/shader/decompiler/decompiler.h"
#include "video_core/shader/decompiler/control_flow.h"


namespace Pica{
namespace Shader{
namespace Decompiler{

class ASTree: ControlFlow{
public:
    struct Seq: ControlFlow::Node;
    struct If: ControlFlow::Node;
    struct IfElse: ControlFlow::Node;
    struct While: ControlFlow::Node;
    struct DoWhile: ControlFlow::Node;

    ASTree(ControlFlow flow);
    virtual ~ASTree()
private:
    ASTree();
};

class Structurizer{
public:
    static bool structurize(ASTree & tree);
};

struct ASTree::Seq: ControlFlow::Node{
    Rc<Node> head;
    Rc<Node> tail;
}

struct ASTree::If: ControlFlow::Node{
    Cond condition;
    Rc<Node> body;
};

struct ASTree::IfElse: ControlFlow::Node{
    Cond condition;
    Rc<Node> true_body;
    Rc<Node> false_body;
}

struct ASTree::While: ControlFlow::Node{
    Cond condition;
    Rc<Node> body;
}

struct ASTree::DoWhile: ControlFlow::Node{
    Cond condition;
    Rc<Node> body;
}

}}}

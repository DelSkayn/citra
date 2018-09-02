#pragma once

#include "video_core/renderer_opengl/decompiler/decompiler.h"
#include "video_core/renderer_opengl/decompiler/control_flow.h"


namespace Pica{
namespace Shader{
namespace Decompiler{

class ASTree: public ControlFlow{
public:
    struct Seq;
    struct If;
    struct IfElse;
    struct While;
    struct DoWhile;

    ASTree(ControlFlow & flow);
    virtual ~ASTree() = default;
    void print();
private:
    static void print_node(Rc<Node> node,unsigned scope);
    ASTree();
};

class Structurizer{
public:
    static bool structurize(ASTree & tree);
private:
    static bool match_node(Rc<ASTree::Node> node);
    static bool is_if(Rc<ASTree::Edge> next, Rc<ASTree::Edge> jump);
    static void match_seq(Rc<ASTree::Node> first, Rc<ASTree::Node> sec);
    static void match_if(Rc<ASTree::Edge> next, Rc<ASTree::Edge> jump);
    static void match_if_else(Rc<ASTree::Edge> left, Rc<ASTree::Edge> right);
};

struct ASTree::Seq: ControlFlow::Node{
    Rc<Node> head;
    Rc<Node> tail;
};

struct ASTree::If: ControlFlow::Node{
    Cond cond;
    Rc<Node> body;
};

struct ASTree::IfElse: ControlFlow::Node{
    Cond cond;
    Rc<Node> true_body;
    Rc<Node> false_body;
};

struct ASTree::While: ControlFlow::Node{
    Cond cond;
    Rc<Node> body;
};

struct ASTree::DoWhile: ControlFlow::Node{
    Cond cond;
    Rc<Node> body;
};

}}}

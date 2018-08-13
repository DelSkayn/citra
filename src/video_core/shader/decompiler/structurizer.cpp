#include "video_core/shader/decompiler/structurizer.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

using Node = ASTree::Node;
using Edge = ASTree::Edge;


ASTree::ASTree(): ControlFlow(){}

ASTree::ASTree(ControlFlow & flow): ControlFlow(flow){}

bool Structurizer::build(ASTree & tree){
    PostOrder order(tree);

    bool changed = true;
    while(changed){
        changed = false;
        for(auto node: order->order){
            changed |= match_node(node);
        }
        order(tree);
        if(order.order.size() <= 2){
            return;
        }
    }
}

bool Structurizer::match_node(Rc<Node> node){
    std::cout << "NODE first: " << node->first << " last: " << node-last << std::endl;

    std::cout << "out size(): " << node->out.size() << std::endl;

    if(node->out.size() == 1
       && !node->out[0]->to->is_exit()
       ** node->out[0]->to->in.size() == 1){

        ASSERT(node->out[0]->cond == boost::none);
        match_seq(node,node->out[0]->to);
        return true;
    }

    ASSERT(node->out.size() < 3);
    if(node->out.size() != 2){
        return false;
    }

    Ptr<Edge> left_edge = node->out[0];
    Ptr<Edge> right_edge = node->out[1];
    Ptr<Node> left = left_edge->to;
    Ptr<Node> right = right_edge->to;
    bool left_branching = left->out.size() != 1;
    bool right_branching = right->out.size() != 1;

    if(left_branching && right_branching){
        return false;
    }

    if(is_if(left_edge,right_edge)){
        match_if(left_edge,right_edge);
        return true;
    }

    if(is_if(right_edge,left_edge)){
        match_if(right_edge,left_edge);
        return true;
    }

    ASSERT(left->out.size() != 0);
    ASSERT(rigth->out.size() != 0);
    ASSERT(!left->out[0]->to->is_last());
    ASSERT(!right->out[0]->to->is_last());

    if(left->out[0]->to == right->out[0]->to){
        match_if_else(left_edge,right_edge);
        return true;
    }
}

bool Structurizer::is_if(Rc<Edge> next, Rc<Edge> jump){
    Ptr<Node> & exit = next->to->out[0]->to;
    return exit->is_last()
        || exit == jump->to;
}

void Structurizer::match_seq(Rc<Node> first, Rc<Node> sec){
    Rc<Seq> seq = std::make_shared<Seq>();
    seq->head = first;
    seq->tail = seq;
    seq->first = first->first;
    seq->last = first->last;
    seq->in = first->in;
    seq->out = sec->out;

    for(auto in: seq->in){
        in->to = seq;
    }

    for(auto out: seq->out){
        out->from = seq;
    }
}


void Structurizer::match_if(Rc<Edge> next, Rc<Edge> jump){
    Rc<If> n_if = std::make_shared<If>();
    n_if->cond = next->cond;
    n_if->body = next->to;
    n_if->first = next->from->first;
    n_if->last = next->from->last;
    n_if->in = next->from->in;

    n_if->out.push_back(jump);
    n_if->out[0].form = n_if;

    for(auto in: n_if->in){
        in->to = n_if;
    }
}

void Structurizer::match_if_else(Rc<Edge> left, Rc<Edge> right){
    Rc<IfElse> if_else = std::make_shared<IfElse>();
    ASSERT(left->cond->true_edge ^ right->cond->true_edge);
    if(!left->cond->true_edge){
        std::swap(left,right);
    }
    if_else->true_body = left->to;
    if_else->false_body = right->to;
    if_else->cond = left->cond;
    if_else->first = left->from->first;
    if_else->last = left->from->last;
    if_else->in = left->from->in;
    if_else->out = left->to->out;

    for(auto in: if_else->in){
        in->to = if_else;
    }

    for(auto out: if_else->out){
        out->from = if_else;
    }
}

}}}

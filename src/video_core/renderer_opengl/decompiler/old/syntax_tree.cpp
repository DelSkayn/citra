
#include "syntax_tree.h"

using ControlFlow;

SyntaxTree::SyntaxTree(){
    this->nodes = {0};
    this->num_nodes = 0;
}

bool SyntaxTree::Build(ControlFlow & control_flow){
    ASSERT(control_flow.first != nullptr);
    CodeBlock first = *control_flow.first;
    Node * first = &this->nodes[num_nodes++];
    BuildBlock(block,first);
}

bool SyntaxTree::BuildBlock(CodeBlock & block,Node * parent){
    //Possibly move to some other place
    Node * node = &this->nodes[num_nodes++];

    for(;;){
        if(block->exit == nullptr
           && block->branch == nullptr){
            if(block->first != block->last){
                node->type = Sequential;
                node->data._seq.first = block->first;
                node->data._seq.last = block->last-1;

                Node * next = &this->nodes[num_nodes++];
                node->next = next;
                node = next;
            }
            node->type = NodeType::Exiting;
            node->data._exit.instr = block->last;
            return false;
        }

        if(block->exit != nullptr
           && block->branch == nullptr){
            node->type = Sequential;
            node->data._seq.first = block->first;
            node->data._seq.last = block->last;
            Node * next = &this->nodes[num_nodes++];
            node->next = next;
            node = next;
            block = *block->last;
            continue;
        }

        if(block->exit == nullptr){
            return true;
        }

        CodeBlock * exit = block->exit;
        CodeBlock * branch = block->branch;

        //Composite control flow
        if(exit->branch != nullptr){
            //Do Something
        }

        //Composite control flow
        if(branch->branch != nullptr){
            //Do Something
        }

        //If
        if(exit->exit == branch){
            node->type = NodeType::If;
        }

        //If Else
        if(exit->exit == branch->exit){
            node->type = NodeType::IfElse;
        }

        //While
        if(exit->exit == &(*block)
           && branch != nullptr){
            node->type = NodeType::While;
        }
    }
}

Node * SyntaxTree::Reduce(CodeBlock * block,Node ** prev){

    if(block->exit != nullptr
       && block->branch == nullptr){
        Node * node = &nodes[num_nodes++];
        node->type = NodeType::Seq;
        node.data._seq.first = block->first;
        node.data._seq.first = block->last;

        block->exit = exit->exit;
        block->branch = exit->branch;
        block->first = exit->first;
        block->last = exit->block->last;
        return node;
    }

    //How do we deal with exiting
    if(block->exit == nullptr
       && block->branch == nullptr){

        Node * seq = &nodes[num_nodes++];
        node->type = NodeType::Exiting;
        seq->data._seq.first = block->first;
        seq->data._seq.last = block->last;
        prev->next = seq;
        return nullptr;
    }

    CodeBlock * exit = block->exit;
    CodeBlock * branch = block->branch;

    CodeBlock exit_obj = *block->exit;
    CodeBlock branch_obj = *block->branch;
    Node ** exit;

    //what to de with composites
    if(exit->branch != nullptr){
        Reduce(CodeBlock * block)
        return nullptr;
    }

    //if
    if(exit->exit == branch || exit->exit == nullptr){
        if(block->first != block->last){
            Node * seq = &nodes[num_nodes++];
            seq->ty = NodeType::Seq;
            seq->data._seq.first = block->first;
            seq->data._seq.last = block->last - 1;
            prev->next = seq;
            prev = seq;
        }

        Node * body = &nodes[num_nodes++];
        body->type = NodeType::Seq;
        body->data._seq.start = exit->start;
        body->data._seq.last = exit->last;

        Node * node = &nodes[num_nodes++];
        node->type = NodeType::If;
        node->data._if.body = body;
        node->data._if.instr = block->last;
        prev->next = node;

        block->exit = branch->exit;
        block->branch = branch->branch;

        block->first = branch->first;
        block->last = branch->last;
        return node;
    }

    if(branch->branch != nullptr){
        return nullptr;
    }


    //if else
    if(exit->exit == branch->exit){
        if(block->first != block->last){
            Node * seq = &nodes[num_nodes++];
            seq->ty = NodeType::Seq;
            seq->data._seq.first = block->first;
            seq->data._seq.last = block->last - 1;
            prev->next = seq;
            prev = seq;
        }

        Node * if_body = &nodes[num_nodes++];
        body->type = NodeType::Seq;
        body->data._seq.start = exit->start;
        body->data._seq.last = exit->last;

        Node * else_body = &nodes[num_nodes++];
        body->type = NodeType::Seq;
        body->data._seq.start = branch->start;
        body->data._seq.last = branch->last;

        Node * node = &nodes[num_nodes++];
        node->type = NodeType::IfElse;
        node->data._if_else.body = if_body;
        node->data._if_else.else_body = else_body;
        node->data._if_else.instr = block->last;
        prev->next = node;

        block->exit = exit->exit->exit;
        block->branch = exit->exit->branch;
        block->first = exit->exit->first;
        block->last = exit->exit->last;
        return node;
    }

    //while
    if(exit->exit == block){
        if(block->first != block->last){
            Node * seq = &nodes[num_nodes++];
            seq->ty = NodeType::Seq;
            seq->data._seq.first = block->first;
            seq->data._seq.last = block->last;
            prev->next = seq;
            prev = seq;
        }

        Node * body = &nodes[num_nodes++];
        node->type = NodeType::Seq;
        body->data._seq.first = exit->first;
        body->data._seq.last = exit->last;

        Node * node = &nodes[num_nodes++];
        node->type = NodeType::While;
        node->data._while.body = body;
        node->data._while.instr = block->last;
        prev->next = node;

        block->exit = exit->exit->exit;
        block->branch = exit->exit->branch;
        block->first = exit->exit->first;
        block->last = exit->exit->last;
        return node;
    }

    return nullptr;
}

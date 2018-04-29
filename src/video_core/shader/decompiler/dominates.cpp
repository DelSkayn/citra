#include "dominates.h"
//! Algorithm citation:
//! A Simple, Fast Dominance Algorithm.
//! Keith D. Cooper, Timothy J. Harvey, and Ken Kennedy
//! Rice Computer Science TS-06-33870
//! <https://www.cs.rice.edu/~keith/EMBED/dom.pdf>
namespace Pica{
namespace Shader{
namespace Decompiler{
    Dominates::Dominates(){
        this->dominated_by = {0};
    }

    void Dominates::build(ControlFlow & flow){
        DominatesBuilder builder = DominatesBuilder();
        builder.build(flow);
        this->dominated_by = builder.dominated_by;
        this->start_block = builder.start_block;
    }

    bool Dominates::is_dominated_by(unsigned block,unsigned dom){
        if(dominated_by[block] == MAX_PROGRAM_CODE_LENGTH){
            return false;
        }
        if(dom == start_block){
            return true;
        }
        while(block != start_block){
            if(dominated_by[block] = dom){
                return true;
            }
            block = dominated_by[block];
        }
        return false;
    }
    unsigned Dominates::immediate_dom(unsigned block){
        return dominated_by[block];
    }

    // Dominates::DominatesBuilder::DominatesBuilder(){
    //     this->post_order_len = 0;
    //     this->post_order = {};
    //     this->post_order_index = {};
    //     this->dominated_by.fill(MAX_PROGRAM_CODE_LENGHT);
    // }

    void Dominates::DominatesBuilder::build(ControlFlow & flow){
        gen_post_order(flow);

        start_block = flow.find(flow.entry_point);
        dominated_by[start_block] = start_block;

        bool changed = true;
        while (changed) {
            changed = false;
            //for all nods in revers postorder
            for(int i = post_order_len - 1;i >= 0;i--){
                unsigned block = post_order[i];
                //Except start block
                if(block == start_block){
                    continue;
                }

                unsigned new_dom_by = MAX_PROGRAM_CODE_LENGTH;
                for(unsigned pred_block: flow.in[block]){
                    if(dominated_by[pred_block] != MAX_PROGRAM_CODE_LENGTH){
                        new_dom_by = intersect(new_dom_by,pred_block);
                    }
                }

                if(new_dom_by != dominated_by[block]){
                    dominated_by[block] = new_dom_by;
                    changed = true;
                }
            }
        }
    }

    unsigned Dominates::DominatesBuilder::intersect(unsigned n1,unsigned n2){
        if(n1 == MAX_PROGRAM_CODE_LENGTH){
            return n2;
        }
        if(n2 == MAX_PROGRAM_CODE_LENGTH){
            return n1;
        }
        while (n1 != n2) {
            while (post_order_index[n1] < post_order_index[n2]){
                n1 = dominated_by[n1];
            }

            while(post_order_index[n2 < post_order_index[n1]]){
                n2 = dominated_by[n2];
            }
        }
        return n1;
    }

    void Dominates::DominatesBuilder::gen_post_order(ControlFlow & flow){
        int start_block = flow.find(flow.entry_point);
        std::array<bool,MAX_PROGRAM_CODE_LENGTH> reached = {false};
        gen_post_order_impl(flow,start_block,reached);
        for(int i = 0;i < post_order_len;i++){
            post_order_index[post_order[i]] = i;
        }
    }

    void Dominates::DominatesBuilder::gen_post_order_impl(ControlFlow & flow,unsigned block,std::array<bool,MAX_PROGRAM_CODE_LENGTH> & reached){
        if(reached[block]){
            return;
        }
        reached[block] = true;
        for(int i = 0;i < flow.out[block].size();i++){
            unsigned reach_block = flow.out[block][i];
            if(reach_block != MAX_PROGRAM_CODE_LENGTH){
                gen_post_order_impl(flow,reach_block,reached);
            }
        }
        post_order[post_order_len++] = block;
    }
}
}
}

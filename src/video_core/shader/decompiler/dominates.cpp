#include "dominates.h"
//! Algorithm citation:
//! A Simple, Fast Dominance Algorithm.
//! Keith D. Cooper, Timothy J. Harvey, and Ken Kennedy
//! Rice Computer Science TS-06-33870
//! <https://www.cs.rice.edu/~keith/EMBED/dom.pdf>
namespace Pica{
namespace Shader{
namespace Decompiler{

    template<T>
    void contains(std::vector<T> & vec, T t){
        for(int i = 0;i < vec.size();i++){
            if(vec[i] == t)
                return true;
        }
        return false;
    }

    Dominates::Dominates(){
        this->dominated_by = {0};
    }

    void Dominates::build(ControlFlow & flow,PostOrder & post_order){
        DominatesBuilder builder = DominatesBuilder();
        builder.build(flow,post_order);
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

    bool Dominates::is_pre_dominated_by(unsigned block,unsigned dom){
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
            block = pre_dominated_by[block];
        }
        return false;
    }
    unsigned Dominates::immediate_pre_dom(unsigned block){
        return pre_dominated_by[block];
    }

    void Dominates::build(ControlFlow & flow,PostOrder & order){
        start_block = flow.find(flow.entry_point);
        dominated_by[start_block] = start_block;

        //find dominators
        bool changed = true;
        while (changed) {
            changed = false;
            //for all nods in revers postorder
            for(int i = order.len - 1;i >= 0;i--){
                unsigned block = order.order[i];
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

        //find predominators
        //first find all exiting blocks which are predominated by
        //themselfs
        std::vector<unsigned> exit_blocks;
        for(int i = 0;i < flow.num_blocks;i++){
            for(int j = 0;j < flow.out[i].size();j++){
                if(flow.out[i][j] == MAX_PROGRAM_CODE_LENGTH){
                    pre_dominated_by[i] = i;
                    exit_blocks.push(i);
                    break;
                }
            }
        }

        //then dominator calculation as previous
        //We kinda assume that reversed post order of control flow from entry to exit
        //is the same as post order of all exits to entry. Which might be true but
        //I have not proved anything.
        //Anyway the algorithm only seems to need post order as a optimization as it keeps
        //going as long as there are changes.
        changed = true;
        while (changed) {
            changed = false;
            //for all nods in revers postorder
            for(int i = 0;i < order.len;i++){
                unsigned block = order.order[i];
                //Except start block
                if(contains(exit_blocks,i)){
                    continue;
                }

                unsigned new_dom_by = MAX_PROGRAM_CODE_LENGTH;
                for(unsigned pred_block: flow.out[block]){
                    if(pre_dominated_by[pred_block] != MAX_PROGRAM_CODE_LENGTH){
                        new_dom_by = intersect(new_dom_by,pred_block);
                    }
                }

                if(new_dom_by != pre_dominated_by[block]){
                    pre_dominated_by[block] = new_dom_by;
                    changed = true;
                }
            }
        }

    }

    unsigned Dominates::intersect(unsigned n1,unsigned n2){
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
}
}
}

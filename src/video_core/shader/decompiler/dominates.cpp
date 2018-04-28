#inlude "dominates.cpp"

namespace Pica{
namespace Shader{
namespace Decompiler{
    Dominates::Dominates(){
        this->dominates = {0};
    }

    void Dominates::calc(ControlFlow & flow){
        std::array<Data,MAX_PROGRAM_CODE_LENGTH> dom;
        unsigned len = 1;

        for(int i = 1;i < MAX_PROGRAM_CODE_LENGTH;i++){
            dom[i].len = 0;
        }

        int entry_point = flow.find(flow.entry_point);
        dom[entry_point].dom[0] = 0;
        dom[entry_point].len += 1;
    }

    void Dominates::DominatesBuilder::push(int block,int dom){
        dom[block].dom[dom[block].len++] = dom;
    }
    
    void Dominates::DominatesBuilder::build(ControlFlow & flow){
        int entry_block = flow.find(flow.entry_point);
        push(entry_block,entry_block);
        for(i = 0;i < flow.num_blocks;i++){
            if(i != entry_block){
                for(j = 0;j < flow.num_blocks;j++){
                    push(i,j)
                }
            }
        }
    }
}
}
}

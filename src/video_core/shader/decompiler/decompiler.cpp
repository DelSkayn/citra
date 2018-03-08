#pragma once

#include <array>
#include "video_core/shader/shader.h"
#include "video_core/shader/decompiler/control_flow.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

typedef CodeArray std::array<u32,MAX_PROGRAM_CODE_LENGTH>

Decompiler::Decompiler(CodeArray & program_code,int entry_point){
    this->program_code = program_code;
    this->entry_point = entry_point;
}

std::string DecompileBlock(ShaderBlock * block){
    if(block == nullptr){
        return "";
    }
    if(block->flag == 1){
        return "";
    }else{
        block->flag = 0;
    }
    switch (block->ty){
        case BlockType::Sequential:
            return DecompileCodeBlock(block->first,block->last)
                + DecompileBlock(block->out);
        case BlockType::If:
            std::string after = DecompileBlock(block->branch);
            std::string src;
            src += DecompileCodeBlock(block->first,block->last);
            src += "{";
            src += DecompileBlock(block->seq_block);
            src += "}";
            return
    }
}

bool Decompiler::Decompile(){
    ControlFlow flow(program_code,entry_point);
    flow.Analyze();
    ShaderBlock * first = flow.First();

}

}
}
}

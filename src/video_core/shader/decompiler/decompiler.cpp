#include <nihstro/shader_bytecode.h>
#include "video_core/shader/shader.h"
#include "video_code/shader/decompiler/decompiler.h"
#include "video_code/shader/decompiler/control_flow.h"
#include "video_code/shader/decompiler/structurizer.h"

using nihstro::OpCode;
using nihstro::Instruction;
using nihstro::RegisterType;
using nihstro::SourceRegister;
using nihstro::SwizzlePattern;

namespace Pica{
namespace Shader{
namespace Decompiler{

bool Decompiler::Decompile(ProgramCode & program, SwizzleArray & swizzle, unsigned entry_point){
    ASSERT(this->program_code != nullptr);
    ASSERT(this->swizzle_data != nullptr);
    ASSERT(this->entry_point < PROGRAM_LEN);

    ProcMap<ControlFlow> proc_map();
    ControlFlow flow();
    CodeGen gen();
    flow.build(program,swizzle,entry_point,proc_map);
    for(auto it = proc_map.begin();it != proc_map.end();++it){
        ASTree tree(it->second);
        if(!gen.generate_function(it->first,tree)){
            return false;
        }
    }
    ASTree tree(flow);
    if(!Structurizer::structurize(flow)){
        return false;
    }
    if(!gen.generate_main(tree)){
        return false;
    }
}

bool ControlFlow::Region::operator==(const Region & other) const {
    return this->start == other.start && this->end == other.end;
}

}}}

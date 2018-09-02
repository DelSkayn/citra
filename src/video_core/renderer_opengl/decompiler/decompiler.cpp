#include <nihstro/shader_bytecode.h>
#include "video_core/shader/shader.h"
#include "video_core/renderer_opengl/decompiler/decompiler.h"
#include "video_core/renderer_opengl/decompiler/control_flow.h"
#include "video_core/renderer_opengl/decompiler/structurizer.h"
#include "video_core/renderer_opengl/decompiler/code_gen.h"

using nihstro::OpCode;
using nihstro::Instruction;
using nihstro::RegisterType;
using nihstro::SourceRegister;
using nihstro::SwizzlePattern;

namespace Pica{
namespace Shader{
namespace Decompiler{

Option<std::string> Decompiler::decompile(const ProgramArray & program, const SwizzleArray & swizzle, unsigned entry_point){
    ASSERT(entry_point < PROGRAM_LEN);

    ProcMap<ControlFlow> proc_map;
    ControlFlow flow;
    CodeGen gen;
    flow.build(program,entry_point,PROGRAM_LEN,proc_map);
    for(auto it = proc_map.begin();it != proc_map.end();++it){
        ASTree tree(it->second);
        tree.print();
        if(!gen.generate_proc(it->first,tree)){
            return boost::none;
        }
    }
    ASTree tree(flow);
    if(!Structurizer::structurize(tree)){
        return boost::none;
    }
    tree.print();
    if(!gen.generate_main(tree)){
        return boost::none;
    }

    return boost::none;
}

bool Region::operator==(const Region & other) const {
    return this->first == other.first && this->last == other.last;
}

}}}

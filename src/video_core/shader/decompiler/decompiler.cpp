#pragma once

#include <array>
#include "video_core/shader/shader.h"

namespace Pica{
namespace Shader{
namespace Decompiler{

typedef CodeArray std::array<u32,MAX_PROGRAM_CODE_LENGTH>

enum DecompileError{
    AnalysisRanOutsideOfShaderProgram,
    DstTargetBeforeIf,
    JmpTargetBeforeEntryPoint,
    UnsupportedBranchInstruction,
    None,
}



class Decompiler{
    DecompileError error;
    CodeArray & program_code;
    int entry_point;
public:
    Decompiler(CodeArray & program_code,int entry_point);
}

}
}
}

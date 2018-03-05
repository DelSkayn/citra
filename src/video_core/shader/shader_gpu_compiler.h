#pragma once

#include <memory>
#include <climits>
#include <string>
#include <vector>
#include <nihstro/shader_bytecode>
#include "video_core/shader/shader.h"



namespace Pica {

namespace Shader {

using nihstro::Instruction;

/*
enum GlslType{
    Float,
    Vector3,
    Vector4,
    Bool,
    Int,
}

struct SymbolTableEntry{
    std::string name;
    GlslType ty;
}

struct SymbolTable{
    std::array<SymbolTableEntry, MAX_VARIABLE_SLOTS> inputs;
    std::array<SymbolTableEntry, MAX_VARIABLE_SLOTS> temps;
    std::array<SymbolTableEntry, MAX_VARIABLE_SLOTS> uniforms;
}
*/

enum CompileError{
    AnalysisRanOutsideOfShaderProgram,
    DstTargetBeforeIf,
    JmpTargetBeforeEntryPoint,
    UnsupportedBranchInstruction,
    None,
}

std::string CompilerErrorToString(CompileError e);



class GpuShaderCompiler{
    ShaderSetup & setup;
    int entry_point;
public:
    CompileError error;
    GpuShaderCompiler(ShaderSetup & setup,int entry_point);
    bool CompileShader();
};

} // namespace

} // namespace

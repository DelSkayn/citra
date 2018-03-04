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

enum ControlFlowError{
    AnalysisRanOutsideOfShaderProgram,
    DstTargetBeforeIf,
    UnsupportedBranchInstruction,
    None,
}

struct ShaderBlock{
    //std::vector<Instruction> instructions;
    unsigned in;
    unsigned out;
    unsigned branch;

    ShaderBlock();

    bool HasBranch();
}

class ControlFlow{
    public:
        ControlFlowError error;
        std::vector<ShaderBlock> blocks;
        ControlFlow(GpuShaderCompiler & compiler);

    private:

        void Split(int index);
        int entry_point;
        int program_counter;
        int current_block;
        std::array<u32,MAX_PROGRAM_CODE_LENGTH> * program_code;
}

class GpuShaderCompiler{
public:
    GpuShaderCompiler(ShaderSetup & setup,int entry_point);
    void CompileShader();
};

} // namespace

} // namespace

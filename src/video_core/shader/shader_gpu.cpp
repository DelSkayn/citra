
#include <nihstro/shader_bytecode.h>

#include "common/logging/log.h"
#include "common/hash.h"
#include "video_core/shader/debug_data.h"
#include "video_core/shader/shader.h"
#include "video_core/shader/shader_gpu.h"
#include "video_core/shader/shader_jit_x64.h"
#include "video_core/shader/shader_interpreter.h"
#include "video_core/shader/decompiler/decompiler.h"
#include "video_core/video_core.h"

namespace Pica {

namespace Shader {

GpuShaderEngine::GpuShaderEngine(){
#ifdef ARCHITECTURE_x86_64
    if(VideoCore::g_shader_jit_enabled){
        this->fallback = std::make_unique<JitX64Engine>();
        return;
    }
#endif
    this->fallback = std::make_unique<InterpreterEngine>();
}

void GpuShaderEngine::CompileShader(ShaderSetup& setup, unsigned int entry_point){
    LOG_DEBUG(HW_GPU, "Compiling new shader");
    Decompiler::Decompiler decompiler(&setup.program_code,&setup.swizzle_data,entry_point);
    decompiler.Decompile();
    setup.engine_data.tried_gpu_compilation = true;
}

void GpuShaderEngine::SetupBatch(ShaderSetup& setup, unsigned int entry_point) {
    ASSERT(entry_point < MAX_PROGRAM_CODE_LENGTH);
    setup.engine_data.entry_point = entry_point;

    u64 code_hash = Common::ComputeHash64(&setup.program_code, sizeof(setup.program_code));
    u64 swizzle_hash = Common::ComputeHash64(&setup.swizzle_data, sizeof(setup.swizzle_data));

    u64 cache_key = code_hash ^ swizzle_hash;
    auto iter = cache.find(cache_key);
    if (iter != cache.end()) {
        setup.engine_data.cached_shader = iter->second.get();
    } else {
        auto shader = std::make_unique<GpuShader>();
        this->CompileShader(setup,entry_point);
        setup.engine_data.cached_shader = shader.get();
        cache.emplace_hint(iter, cache_key, std::move(shader));
    }
    this->fallback->SetupBatch(setup,entry_point);
}
void GpuShaderEngine::Run(const ShaderSetup& setup, UnitState& state) const {
    this->fallback->Run(setup,state);
}

GpuShaderEngine::~GpuShaderEngine() = default;

} // namespace

} // namespace

#include "video_core/shader/debug_data.h"
#include "video_core/shader/shader.h"
#include "video_core/shader/shader_gpu.h"
#include "video_core/shader/shader_jit_x64.h"
#include "video_core/shader/shader_interpreter.h"
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

void GpuShaderEngine::SetupBatch(ShaderSetup& setup, unsigned int entry_point) {
    this->fallback->SetupBatch(setup,entry_point);
}
void GpuShaderEngine::Run(const ShaderSetup& setup, UnitState& state) const {
    this->fallback->Run(setup,state);
}

GpuShaderEngine::~GpuShaderEngine() = default;

} // namespace

} // namespace

#pragma once

#include <memory>
#include "video_core/shader/debug_data.h"
#include "video_core/shader/shader.h"

namespace Pica {

namespace Shader {

class GpuShaderEngine final : public ShaderEngine {
    struct GpuShader{
    };

    std::unique_ptr<ShaderEngine> fallback;
    std::unordered_map<u64, std::unique_ptr<GpuShader>> cache;
public:
    GpuShaderEngine();
    ~GpuShaderEngine() override;
    void SetupBatch(ShaderSetup& setup, unsigned int entry_point) override;
    void Run(const ShaderSetup& setup, UnitState& state) const override;

    void CompileShader(ShaderSetup& setup, unsigned int entry_point);
};

} // namespace

} // namespace

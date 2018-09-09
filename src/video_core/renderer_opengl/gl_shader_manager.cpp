
#include <iostream>
#include "video_core/renderer_opengl/decompiler/decompiler.h"
#include "video_core/renderer_opengl/gl_shader_manager.h"

namespace GLShader {

std::shared_ptr<PicaShader> GLShaderManager::get(const Pica::State& state,
                                                 const Pica::Shader::ShaderSetup& vs,
                                                 bool& cached) {

    auto config = PicaShaderConfig::BuildFromState(state);
    auto shader = this->shaders.find(config);
    if (shader == shaders.end()) {
        cached = false;
        auto n_shader = std::make_shared<PicaShader>();

        auto source = Pica::Shader::Decompiler::Decompiler::decompile(
            vs.program_code, vs.swizzle_data, config.state.entry_point,
            config.state.vs_output_attributes);
        if (source) {
            n_shader->bypass_shader.Create(source->c_str(), GenerateFragmentShader(config).c_str());
            n_shader->is_bypass = true;
        } else {
            std::cout << "Failed to compile shader." << std::endl;
            n_shader->is_bypass = false;
        }
        n_shader->shader.Create(GenerateVertexShader().c_str(),
                                GenerateFragmentShader(config).c_str());
        shaders.insert(std::make_pair(config, n_shader));
        return n_shader;
    }
    cached = true;
    return shader->second;
}

} // namespace GLShader

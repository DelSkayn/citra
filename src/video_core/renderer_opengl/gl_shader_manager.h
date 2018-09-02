#pragma once

#include <memory>
#include <unordered_map>
#include "video_core/shader/shader.h"
#include "video_core/renderer_opengl/gl_shader_gen.h"
#include "video_core/renderer_opengl/gl_resource_manager.h"

namespace GLShader{

/// OpenGL shader generated for a given Pica register state
struct PicaShader {
    /// OpenGL shader resource
    OGLShader shader;
    bool is_bypass;
};

class GLShaderManager{
 private:
    std::unordered_map<GLShader::PicaShaderConfig, std::shared_ptr<PicaShader>> shaders;
 public:
    GLShaderManager() = default;
    std::shared_ptr<PicaShader> get(const Pica::State & state,const Pica::Shader::ShaderSetup & vs,bool & created);
};

}

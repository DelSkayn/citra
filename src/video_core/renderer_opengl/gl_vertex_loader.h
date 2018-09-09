#pragma once

class GLVertexLoader{
 private:
 public:
    bool setup(PipelineRegs & regs);
    void bind();
    unsigned vertex_size();
    void * vertex_data();
    unsigned index_size();
    void * index_data();

};

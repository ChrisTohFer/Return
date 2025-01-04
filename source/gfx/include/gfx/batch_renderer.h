#pragma once

#include "gfx_forward.h"
#include "maths/maths.h"

#include <vector>

namespace gfx
{

    class BatchRenderer
    {
    public:
        void add_instance(const VertexArray&, const ShaderProgram&, const Texture*, const maths::Matrix44& transform);
        void add_light();
        
        void draw_all(float time, const maths::Matrix44& camera_view, const maths::Matrix44& camera_projection) const;
        void clear(bool all = false);

        //additional information to add:
        //texture, time, lighting info, camera transform
    private:
        struct TextureBatch
        {
            const Texture* texture;
            std::vector<maths::Matrix44> transforms;
        };
        struct VertexArrayBatch
        {
            const VertexArray* vao;
            std::vector<TextureBatch> texture_batches;
        };
        struct ShaderBatch
        {
            const ShaderProgram* program;
            std::vector<VertexArrayBatch> vao_batches;
        };

        std::vector<ShaderBatch> m_batches;
    };

}
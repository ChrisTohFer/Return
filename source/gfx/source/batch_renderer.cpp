#include "batch_renderer.h"

#include "shader.h"
#include "texture.h"
#include "vertex_array_object.h"

#include <algorithm>

namespace gfx
{
    void BatchRenderer::add_instance(const VertexArray& vao, const ShaderProgram& program, const Texture* texture, const maths::Matrix44& transform)
    {
        ShaderBatch* sbatch = nullptr;
        VertexArrayBatch* abatch = nullptr;
        TextureBatch* tbatch = nullptr;

        auto find_shader_batch = std::find_if(m_batches.begin(), m_batches.end(), [&program](auto& elem)
        {
            return elem.program == &program;
        });

        if(find_shader_batch != m_batches.end())
            sbatch = &(*find_shader_batch);
        else
        {
            m_batches.push_back({&program});
            sbatch = &m_batches.back();
        }

        auto& abatches = sbatch->vao_batches;
        auto find_vao_batch = std::find_if(abatches.begin(), abatches.end(), [&vao](auto& elem)
        {
            return elem.vao == &vao;
        });

        if(find_vao_batch != abatches.end())
            abatch = &(*find_vao_batch);
        else
        {
            abatches.push_back({&vao});
            abatch = &abatches.back();
        }

        auto& tbatches = abatch->texture_batches;
        auto find_texture_batch = std::find_if(tbatches.begin(), tbatches.end(), [texture](auto& elem)
        {
            return elem.texture == texture;
        });

        if(find_texture_batch != tbatches.end())
            tbatch = &(*find_texture_batch);
        else
        {
            tbatches.push_back({texture});
            tbatch = &tbatches.back();
        }

        tbatch->transforms.push_back(transform);
    }

    void BatchRenderer::add_light()
    {
    }

    void BatchRenderer::draw_all(float time, const maths::Matrix44& camera_view, const maths::Matrix44& camera_projection) const
    {
        auto camera = camera_projection * camera_view;
        for(auto& sbatch : m_batches)
        {
            assert(sbatch.program != nullptr);
            sbatch.program->use();

            //set lighting/other global uniforms
            set_uniform(sbatch.program->uniform_location("camera"), camera);
            set_uniform(sbatch.program->uniform_location("time"), time);
            set_uniform(sbatch.program->uniform_location("tex"), 0);
            
            for(auto& abatch : sbatch.vao_batches)
            {
                abatch.vao->use();
                for(auto& tbatch : abatch.texture_batches)
                {
                    if(tbatch.texture)
                        tbatch.texture->use();
                    else
                        unbind_texture();
                    
                    for(auto& transform : tbatch.transforms)
                    {
                        set_uniform(sbatch.program->uniform_location("transform"), transform);
                        abatch.vao->draw_triangles();
                    }
                }
            }
        }
    }

    void BatchRenderer::clear(bool all)
    {
        if(all)
        {
            m_batches.clear();
            return;
        }
        
        //only clear the transforms as the other data will likely be re-used frame to frame
        for(auto& batch : m_batches)
        {
            for(auto& abatch : batch.vao_batches)
            {
                for(auto& tbatch : abatch.texture_batches)
                {
                    tbatch.transforms.clear();
                }
            }
        }
    }
}
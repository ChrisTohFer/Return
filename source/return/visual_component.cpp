#include "visual_component.h"

#include "scene.h"

#include "maths/maths.h"
#include "gfx/debug_lines.h"
#include "gfx/graphics_manager.h"

#include "imgui/imgui.h"

namespace re
{
    file::FileOut& operator<<(file::FileOut& f, const std::unique_ptr<VisualComponent>& vc)
    {
        f << (vc != nullptr);
        if (vc == nullptr)
        {
            return f;
        }

        auto type = vc->type();
        f << type;

        switch (type)
        {
        case VisualComponentType::VAO:    static_cast<VAOComponent*>(vc.get())->write(f); break;
        case VisualComponentType::Sphere: static_cast<SphereComponent*>(vc.get())->write(f); break;
        case VisualComponentType::Cube:   static_cast<CubeComponent*>(vc.get())->write(f); break;
        }

        return f;
    }

    file::FileIn& operator>>(file::FileIn& f, std::unique_ptr<VisualComponent>& vc)
    {
        bool valid;
        f >> valid;
        if (!valid)
        {
            return f;
        }

        VisualComponentType type;
        f >> type;

        switch (type)
        {
        case VisualComponentType::VAO:
        {
            auto component = std::make_unique<VAOComponent>();
            component->read(f);
            vc = std::move(component);
            break;
        }
        case VisualComponentType::Sphere:
        {
            auto component = std::make_unique<SphereComponent>();
            component->read(f);
            vc = std::move(component);
            break;
        }
        case VisualComponentType::Cube:
        {
            auto component = std::make_unique<CubeComponent>();
            component->read(f);
            vc = std::move(component);
            break;
        }
        }

        return f;
    }

    void VAOComponent::draw(
        const maths::Matrix44& transform,
        [[maybe_unused]] const maths::Matrix44& camera,
        [[maybe_unused]] const Scene& scene,
        [[maybe_unused]] gfx::BatchRenderer& batch_renderer) const
    {
        if(m_vao == nullptr || m_program == nullptr) return;
#if 1
        batch_renderer.add_instance(*m_vao, *m_program, m_texture, transform);
#else
        auto& vao = *m_vao;
        auto& program = *m_program;
        
        //texture
        if(m_texture)
        {
            m_texture->use();
        }
        else
        {
            gfx::unbind_texture();
        }

        //program
        program.use();
        gfx::set_uniform(program.uniform_location("tex"), 0);
        gfx::set_uniform(program.uniform_location("time"), scene.time());
        gfx::set_uniform(program.uniform_location("transform"), transform);
        gfx::set_uniform(program.uniform_location("camera"), camera);
        gfx::set_uniform(program.uniform_location("light_direction"), scene.directional_light().direction);
        gfx::set_uniform(program.uniform_location("light_colour"), scene.directional_light().colour);
        gfx::set_uniform(program.uniform_location("ambient_colour"), scene.ambient_light().colour);

        //vao
        vao.use();
        vao.draw_triangles();
#endif
    }

    void VAOComponent::edit(const Scene& scene)
    {
        auto& manager = scene.gfx_manager();
        auto vaos = manager.vertex_array_names();
        auto programs = manager.shader_program_names();
        auto textures = manager.texture_names();

        if (ImGui::Button("Clear vao"))
        {
            m_vao = nullptr;
            m_vao_name = "";
        }
        ImGui::SameLine();
        if (ImGui::BeginCombo("VAO", m_vao_name.c_str()))
        {
            for (auto& vao : vaos)
            {
                const bool selected = vao == m_vao_name;
                if (ImGui::Selectable(vao.c_str(), selected) && !selected)
                {
                    m_vao_name = vao;
                    m_vao = manager.vertex_array(vao.c_str());
                }
            }
            ImGui::EndCombo();
        }
        if (ImGui::Button("Clear program"))
        {
            m_program = nullptr;
            m_program_name = "";
        }
        ImGui::SameLine();
        if (ImGui::BeginCombo("Program", m_program_name.c_str()))
        {
            for (auto& program : programs)
            {
                const bool selected = program == m_program_name;
                if (ImGui::Selectable(program.c_str(), selected) && !selected)
                {
                    m_program_name = program;
                    m_program = manager.shader_program(program.c_str());
                }
            }
            ImGui::EndCombo();
        }
        if (ImGui::Button("Clear texture"))
        {
            m_texture = nullptr;
            m_texture_name = "";
        }
        ImGui::SameLine();
        if (ImGui::BeginCombo("Texture", m_texture_name.c_str()))
        {
            for (auto& texture : textures)
            {
                const bool selected = texture == m_texture_name;
                if (ImGui::Selectable(texture.c_str(), selected) && !selected)
                {
                    m_texture_name = texture;
                    m_texture = manager.texture(texture.c_str());
                }
            }
            ImGui::EndCombo();
        }
    }
    void VAOComponent::relink(const Scene& scene)
    {
        auto& manager = scene.gfx_manager();
        m_vao = manager.vertex_array(m_vao_name.c_str());
        m_program = manager.shader_program(m_program_name.c_str());
        m_texture = manager.texture(m_texture_name.c_str());
    }

    void SphereComponent::draw(const maths::Matrix44& transform, const maths::Matrix44& camera, const Scene&, gfx::BatchRenderer&) const
    {
        gfx::draw_sphere(transform, m_radius, camera, m_colour, m_num_segments);
    }

    void SphereComponent::edit(const Scene&)
    {
        ImGui::SliderFloat3("Colour", &m_colour.x, 0.f, 1.f);
        ImGui::SliderFloat("Radius", &m_radius, 0.01f, 10.f);
        ImGui::SliderInt("Segments", &m_num_segments, 2, 64);
    }
    
    void CubeComponent::draw(const maths::Matrix44& transform, const maths::Matrix44& camera, const Scene&, gfx::BatchRenderer&) const
    {
        gfx::draw_cube(transform, m_dimensions, camera, m_colour);
    }
    
    void CubeComponent::edit(const Scene&)
    {
        ImGui::SliderFloat3("Colour", &m_colour.x, 0.f, 1.f);
        ImGui::SliderFloat3("Dimensions", &m_dimensions.x, 0.f, 5.f);
    }

    bool edit(const char* label, std::unique_ptr<VisualComponent>& vc, const Scene& scene)
    {
        bool changed = false;

        ImGui::PushID(label);
        if(ImGui::Button("Use VAO"))
        {
            vc = std::make_unique<VAOComponent>();
            changed = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Use Sphere"))
        {
            vc = std::make_unique<SphereComponent>();
            changed = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Use Cube"))
        {
            vc = std::make_unique<CubeComponent>();
            changed = true;
        }
        
        if(vc)
        {
            //should be checking for changes internally...
            vc->edit(scene);
        }
        ImGui::PopID();

        return changed;
    }
}
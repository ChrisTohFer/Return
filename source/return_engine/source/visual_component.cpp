#include "visual_component.h"

#include "scene.h"

#include "maths/maths.h"
#include "gfx/debug_lines.h"
#include "gfx/graphics_manager.h"

#include "imgui/imgui.h"

namespace re
{
    void SphereComponent::draw(const maths::Matrix44& transform, const maths::Matrix44& camera, gfx::BatchRenderer&) const
    {
        gfx::draw_sphere(transform, m_radius, camera, m_colour, m_num_segments);
    }

    void SphereComponent::edit(const Scene&)
    {
        ImGui::SliderFloat3("Colour", &m_colour.x, 0.f, 1.f);
        ImGui::SliderFloat("Radius", &m_radius, 0.01f, 10.f);
        ImGui::SliderInt("Segments", &m_num_segments, 2, 64);
    }
    
    void CubeComponent::draw(const maths::Matrix44& transform, const maths::Matrix44& camera, gfx::BatchRenderer&) const
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
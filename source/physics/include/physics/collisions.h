#pragma once

#include "colliders.h"

#include "imgui/imgui.h"

#include <vector>

namespace phys
{
    inline void detect_collisions(const std::vector<AABB3>& aabbs, const std::vector<Sphere>& spheres)
    {
        if(ImGui::Begin("collisions"))
        {
            for(int aabb_1_index = 0; aabb_1_index < aabbs.size(); ++aabb_1_index)
            {
                for(int aabb_2_index = aabb_1_index + 1; aabb_2_index < aabbs.size(); ++aabb_2_index)
                {
                    if(intersects(aabbs[aabb_1_index], aabbs[aabb_2_index]))
                    {
                        ImGui::Text("Collision between aabb %d and aabb %d", aabb_1_index, aabb_2_index);
                    }
                }
                for(int sphere_index = 0; sphere_index < spheres.size(); ++sphere_index)
                {
                    if(intersects(aabbs[aabb_1_index], spheres[sphere_index]))
                    {
                        ImGui::Text("Collision between aabb %d and sphere %d", aabb_1_index, sphere_index);
                    }
                }
            }
            for(int sphere_1_index = 0; sphere_1_index < spheres.size(); ++sphere_1_index)
            {
                for(int sphere_2_index = sphere_1_index + 1; sphere_2_index < spheres.size(); ++sphere_2_index)
                {
                    if(intersects(spheres[sphere_1_index], spheres[sphere_2_index]))
                    {
                        ImGui::Text("Collision between sphere %d and sphere %d", sphere_1_index, sphere_2_index);
                    }
                }
            }
        }
        ImGui::End();
    }
}
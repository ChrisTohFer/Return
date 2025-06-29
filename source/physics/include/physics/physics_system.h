#pragma once

#include "rigidbody.h"
#include "colliders.h"

#include "maths/maths.h"

#include <vector>

namespace phys
{
    struct Entity
    {
        maths::Vector3& pos;
        maths::Quaternion& orientation;
        Collider* collider = nullptr;
        RigidBody* rigid = nullptr;
    };

    void handle_cuboid_cuboid(Entity&, Entity&)
    {
        //todo
    }

    void handle_cuboid_sphere(Entity&, Entity&)
    {
        //todo
    }

    void handle_cuboid_aabb(Entity&, Entity&)
    {
        //todo
    }

    void handle_sphere_cuboid(Entity& lhs, Entity& rhs)
    {
        handle_cuboid_sphere(rhs, lhs);
    }

    void handle_sphere_sphere(Entity& lhs, Entity& rhs)
    {
        auto& lhs_col = *static_cast<Sphere*>(lhs.collider);
        lhs_col.pos = lhs.pos;
        auto& rhs_col = *static_cast<Sphere*>(rhs.collider);
        rhs_col.pos = rhs.pos;
        if (!intersects(lhs_col, rhs_col))
        {
            //not touching
            return;
        }

        const auto center_to_center_separation = rhs.pos - lhs.pos;
        const auto center_to_center_separation_magnitude = center_to_center_separation.magnitude();
        const auto center_to_center_separation_unit = center_to_center_separation / center_to_center_separation_magnitude;
        const float overlap = lhs_col.radius + rhs_col.radius - center_to_center_separation_magnitude;
        const auto overlap_vector = overlap * center_to_center_separation_unit;

        const float lhs_rhs_mass_ratio = lhs.rigid->properties.mass / rhs.rigid->properties.mass;
        const float lhs_ratio = 1.f / (1.f + lhs_rhs_mass_ratio);
        const float rhs_ratio = 1.f / (1.f + 1.f / lhs_rhs_mass_ratio);

        //separate the colliders, proportion of separation applied to each entity is based on mass ratio
        lhs.pos -= lhs_ratio * overlap_vector;
        rhs.pos += rhs_ratio * overlap_vector;

        //at the time of collision the colliders should be moving towards one another
        //to avoid continuous collisions the component of relative_velocity that is parallel to the separation vector must be reduced to zero or reversed
        //the change associated with each entity is based on the mass ratio, and the velocity that is conserved is decided by elasticity
        const auto relative_velocity = lhs.rigid->velocity - rhs.rigid->velocity;
        const float parallel_relative_speed = maths::Vector3::dot(relative_velocity, center_to_center_separation_unit);

        //should be a property on the rigidbody
        constexpr float elasticity = 1.f;
        const float relative_speed_delta = 
            parallel_relative_speed /*first term cancels the original velocity*/ +
            parallel_relative_speed * elasticity /*second term is the preserved velocity*/;

        const auto separation_vector = relative_speed_delta * center_to_center_separation_unit;
        lhs.rigid->velocity -= lhs_ratio * separation_vector;
        rhs.rigid->velocity += rhs_ratio * separation_vector;
    }

    void handle_sphere_aabb(Entity&, Entity&)
    {

    }

    void handle_aabb_cuboid(Entity& lhs, Entity& rhs)
    {
        handle_cuboid_sphere(rhs, lhs);
    }

    void handle_aabb_sphere(Entity& lhs, Entity& rhs)
    {
        handle_sphere_aabb(rhs, lhs);
    }

    void handle_aabb_aabb(Entity&, Entity&)
    {
        
    }

    using CollisionHandler = void(*)(Entity&, Entity&);
    CollisionHandler handler_matrix[] = {
        handle_cuboid_cuboid, handle_cuboid_sphere, handle_cuboid_aabb,
        handle_sphere_cuboid, handle_sphere_sphere, handle_sphere_aabb,
        handle_aabb_cuboid, handle_aabb_sphere, handle_aabb_aabb,
    };

    void solve_physics(std::vector<Entity>& entities, float delta_time)
    {
        //advance rigidbodies
        for (auto& entity : entities)
        {
            entity.rigid->update(delta_time, entity.pos, entity.orientation);
        }

        //loop over all entity pairs and detect all collisions
        //this is the simple n squared implementation
        for (int ent_index_1 = 0; ent_index_1 < entities.size(); ++ent_index_1)
        {
            Entity& e1 = entities[ent_index_1];
            if (e1.collider == nullptr) continue;

            for (int ent_index_2 = ent_index_1 + 1; ent_index_2 < entities.size(); ++ent_index_2)
            {
                Entity& e2 = entities[ent_index_2];
                if (e2.collider == nullptr) continue;

                auto handler = handler_matrix[(int)e1.collider->type() * 3 + (int)e2.collider->type()];
                handler(e1, e2);
            }
        }
    }


}
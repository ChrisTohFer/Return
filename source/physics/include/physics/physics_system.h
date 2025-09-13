#pragma once

#include "colliders.h"
#include "rigidbody.h"

#include "maths/maths.h"

#include <vector>

namespace phys
{
    struct Entity
    {
        maths::Vector3&    pos;
        maths::Quaternion& orientation;
        Collider*          collider = nullptr;
        RigidBody*         rigid    = nullptr;
    };

    constexpr float coefficient_of_restitution = 0.5f;

    inline void handle_cuboid_cuboid(Entity&, Entity&)
    {
        //todo
    }

    inline void handle_cuboid_sphere(Entity& lhs, Entity& rhs)
    {
        using namespace maths;

        const Cuboid& cube_col   = *static_cast<Cuboid*>(lhs.collider);
        RigidBody&    cube_rigid = *lhs.rigid;

        const Sphere& sphere_col   = *static_cast<Sphere*>(rhs.collider);
        RigidBody&    sphere_rigid = *rhs.rigid;

        const auto sphere_pos_in_cube_space         = cube_col.orientation.inverse() * (rhs.pos - lhs.pos);
        const auto clamped_sphere_pos_in_cube_space = Vector3{
            std::clamp(sphere_pos_in_cube_space.x, cube_col.min.x, cube_col.max.x),
            std::clamp(sphere_pos_in_cube_space.y, cube_col.min.y, cube_col.max.y),
            std::clamp(sphere_pos_in_cube_space.z, cube_col.min.z, cube_col.max.z)
        };
        if ((clamped_sphere_pos_in_cube_space - sphere_pos_in_cube_space).magnitude_squared() >= sphere_col.radius * sphere_col.radius)
        {
            //not touching
            return;
        }

        //in cube orientation
        const Vector3 collision_point_cube_space   = clamped_sphere_pos_in_cube_space;
        const Vector3 collision_unit_cube_space    = (clamped_sphere_pos_in_cube_space - sphere_pos_in_cube_space).normalized();
        const Vector3 collision_overlap_cube_space = (sphere_pos_in_cube_space - clamped_sphere_pos_in_cube_space) + collision_unit_cube_space * sphere_col.radius;

        //in world space orientation
        const Vector3 collision_point             = cube_col.orientation * collision_point_cube_space + lhs.pos;
        const Vector3 cube_collision_offset       = collision_point - lhs.pos;
        const Vector3 collision_direction         = cube_col.orientation * collision_unit_cube_space;
        const Vector3 collision_overlap           = cube_col.orientation * collision_overlap_cube_space;
        const Vector3 relative_velocity           = sphere_rigid.velocity - cube_rigid.velocity - Vector3::cross(cube_rigid.angular_velocity, cube_collision_offset);
        const float   relative_velocity_collision = Vector3::dot(relative_velocity, collision_direction);
        const float   relative_velocity_change    = -(1.f + coefficient_of_restitution) * relative_velocity_collision;

        const float    inverse_cube_mass                     = 1.f / cube_rigid.properties.mass;
        const float    inverse_sphere_mass                   = 1.f / sphere_rigid.properties.mass;
        const Matrix33 inverse_cube_moment_of_inertia        = inverse_cube_mass * cube_col.inertia_tensor_over_mass().inverse();
        const Vector3  scaled_inverse_cube_moment_of_inertia = inverse_cube_moment_of_inertia * Vector3::cross(Vector3::cross(cube_collision_offset, collision_direction), cube_collision_offset);

        const float   impulse_magnitude = relative_velocity_change / (inverse_cube_mass + inverse_sphere_mass + Vector3::dot(scaled_inverse_cube_moment_of_inertia, collision_direction));
        const Vector3 impulse           = impulse_magnitude * collision_direction;

        //apply impulse
        cube_rigid.velocity -= inverse_cube_mass * impulse;
        cube_rigid.angular_velocity -= inverse_cube_moment_of_inertia * Vector3::cross(cube_collision_offset, impulse);
        sphere_rigid.velocity += inverse_sphere_mass * impulse;

        //separate
        const float total_mass = lhs.rigid->properties.mass + rhs.rigid->properties.mass;
        const float lhs_ratio  = rhs.rigid->properties.mass / total_mass;
        const float rhs_ratio  = lhs.rigid->properties.mass / total_mass;

        //separate the colliders, proportion of separation applied to each entity is based on mass ratio
        lhs.pos += lhs_ratio * collision_overlap;
        rhs.pos -= rhs_ratio * collision_overlap;
    }

    inline void handle_cuboid_aabb(Entity&, Entity&)
    {
        //todo
    }

    inline void handle_sphere_cuboid(Entity& lhs, Entity& rhs)
    {
        handle_cuboid_sphere(rhs, lhs);
    }

    inline void handle_sphere_sphere(Entity& lhs, Entity& rhs)
    {
        auto& lhs_col = *static_cast<Sphere*>(lhs.collider);
        lhs_col.pos   = lhs.pos;
        auto& rhs_col = *static_cast<Sphere*>(rhs.collider);
        rhs_col.pos   = rhs.pos;
        if (!intersects(lhs_col, rhs_col))
        {
            //not touching
            return;
        }

        const auto  center_to_center_separation           = rhs.pos - lhs.pos;
        const auto  center_to_center_separation_magnitude = center_to_center_separation.magnitude();
        const auto  center_to_center_separation_unit      = center_to_center_separation / center_to_center_separation_magnitude;
        const float overlap                               = lhs_col.radius + rhs_col.radius - center_to_center_separation_magnitude;
        const auto  overlap_vector                        = overlap * center_to_center_separation_unit;

        const float lhs_rhs_mass_ratio = lhs.rigid->properties.mass / rhs.rigid->properties.mass;
        const float lhs_ratio          = 1.f / (1.f + lhs_rhs_mass_ratio);
        const float rhs_ratio          = 1.f / (1.f + 1.f / lhs_rhs_mass_ratio);

        //separate the colliders, proportion of separation applied to each entity is based on mass ratio
        lhs.pos -= lhs_ratio * overlap_vector;
        rhs.pos += rhs_ratio * overlap_vector;

        //at the time of collision the colliders should be moving towards one another
        //to avoid continuous collisions the component of relative_velocity that is parallel to the separation vector must be reduced to zero or reversed
        //the change associated with each entity is based on the mass ratio, and the velocity that is conserved is decided by elasticity
        const auto  relative_velocity       = lhs.rigid->velocity - rhs.rigid->velocity;
        const float parallel_relative_speed = maths::Vector3::dot(relative_velocity, center_to_center_separation_unit);

        //should be a property on the rigidbody
        const float relative_speed_delta =
            parallel_relative_speed /*first term cancels the original velocity*/ +
            parallel_relative_speed * coefficient_of_restitution /*second term is the preserved velocity*/;

        const auto separation_vector = relative_speed_delta * center_to_center_separation_unit;
        lhs.rigid->velocity -= lhs_ratio * separation_vector;
        rhs.rigid->velocity += rhs_ratio * separation_vector;
    }

    inline void handle_sphere_aabb(Entity&, Entity&)
    {
    }

    inline void handle_aabb_cuboid(Entity& lhs, Entity& rhs)
    {
        handle_cuboid_sphere(rhs, lhs);
    }

    inline void handle_aabb_sphere(Entity& lhs, Entity& rhs)
    {
        handle_sphere_aabb(rhs, lhs);
    }

    inline void handle_aabb_aabb(Entity&, Entity&)
    {
    }

    using CollisionHandler = void (*)(Entity&, Entity&);
    // clang-format off
    inline CollisionHandler handler_matrix[] = {
        handle_cuboid_cuboid, handle_cuboid_sphere, handle_cuboid_aabb,
        handle_sphere_cuboid, handle_sphere_sphere, handle_sphere_aabb,
        handle_aabb_cuboid,   handle_aabb_sphere,   handle_aabb_aabb,
    };
    // clang-format on

    inline void solve_physics(std::vector<Entity>& entities, float delta_time)
    {
        //advance rigid bodies
        for (auto& entity : entities)
        {
            entity.rigid->update(delta_time, entity.pos, entity.orientation);
        }

        //loop over all entity pairs and detect all collisions
        //this is the simple n squared implementation
        for (size_t ent_index_1 = 0; ent_index_1 < entities.size(); ++ent_index_1)
        {
            Entity& e1 = entities[ent_index_1];
            if (e1.collider == nullptr) continue;

            for (size_t ent_index_2 = ent_index_1 + 1; ent_index_2 < entities.size(); ++ent_index_2)
            {
                Entity& e2 = entities[ent_index_2];
                if (e2.collider == nullptr) continue;

                const auto handler = handler_matrix[static_cast<int>(e1.collider->type()) * 3 + static_cast<int>(e2.collider->type())];
                handler(e1, e2);
            }
        }
    }


} // namespace phys
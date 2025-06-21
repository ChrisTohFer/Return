#pragma once

#include "gfx/gfx_forward.h"
#include "maths/maths.h"

#include "file/file.h"

#include <memory>
#include <string>

namespace re
{
    class Scene;

    enum class VisualComponentType
    {
        VAO,
        Sphere,
        Cube,
    };

    class VisualComponent
    {
    public:
        virtual ~VisualComponent() = default;

        virtual std::unique_ptr<VisualComponent> clone() const = 0;

        virtual void draw(
            const maths::Matrix44& transform,
            const maths::Matrix44& camera,
            gfx::BatchRenderer&) const = 0;
        virtual void edit(const Scene&) = 0;
        virtual void relink(const Scene&) = 0;

        virtual VisualComponentType type() const = 0;
    };

    class SphereComponent : public VisualComponent
    {
    public:
        std::unique_ptr<VisualComponent> clone() const override { return std::make_unique<SphereComponent>(*this); }

        void draw(
            const maths::Matrix44& transform,
            const maths::Matrix44& camera,
            gfx::BatchRenderer&) const override;
        void edit(const Scene&) override;
        void relink(const Scene&) override {}
        VisualComponentType type() const { return VisualComponentType::Sphere; }

    private:
        float m_radius = 1.f;
        maths::Vector3 m_colour = {1.f,0.f,0.f};
        int m_num_segments = 20;
    };

    class CubeComponent : public VisualComponent
    {
    public:
        std::unique_ptr<VisualComponent> clone() const override { return std::make_unique<CubeComponent>(*this); }

        void draw(
            const maths::Matrix44& transform,
            const maths::Matrix44& camera,
            gfx::BatchRenderer&) const override;
        void edit(const Scene&) override;
        void relink(const Scene&) override {}
        VisualComponentType type() const { return VisualComponentType::Cube; }

    private:
        maths::Vector3 m_dimensions = maths::Vector3::one();
        maths::Vector3 m_colour = {1.f,0.f,0.f};
    };

    bool edit(const char* label, std::unique_ptr<VisualComponent>&, const Scene&);
}
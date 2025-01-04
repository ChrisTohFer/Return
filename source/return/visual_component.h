#pragma once

#include "lights.h"

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
    DEFINE_ENUM_SERIALIZE_FUNCTIONS(VisualComponentType);

    class VisualComponent
    {
    public:
        virtual ~VisualComponent() = default;

        virtual std::unique_ptr<VisualComponent> clone() const = 0;

        virtual void draw(
            const maths::Matrix44& transform,
            const maths::Matrix44& camera,
            const Scene&,
            gfx::BatchRenderer&) const = 0;
        virtual void edit(const Scene&) = 0;
        virtual void relink(const Scene&) = 0;

        virtual VisualComponentType type() const = 0;
    };

    file::FileOut& operator<<(file::FileOut& f, const std::unique_ptr<VisualComponent>& vc);
    file::FileIn& operator>>(file::FileIn& f, std::unique_ptr<VisualComponent>& vc);

    class VAOComponent : public VisualComponent
    {
    public:
        DEFINE_SERIALIZATION_FUNCTIONS(m_vao_name, m_program_name, m_texture_name);

        std::unique_ptr<VisualComponent> clone() const override { return std::make_unique<VAOComponent>(*this); }

        void draw(
            const maths::Matrix44& transform,
            const maths::Matrix44& camera,
            const Scene&,
            gfx::BatchRenderer&) const override;
        void edit(const Scene&) override;
        void relink(const Scene&) override;
        VisualComponentType type() const { return VisualComponentType::VAO; }

    private:
        const gfx::VertexArray* m_vao = nullptr;
        const gfx::ShaderProgram* m_program = nullptr;
        const gfx::Texture* m_texture = nullptr;

        //needed by editor and for relinking
        std::string m_vao_name;
        std::string m_program_name;
        std::string m_texture_name;
    };

    class SphereComponent : public VisualComponent
    {
    public:
        DEFINE_SERIALIZATION_FUNCTIONS(m_radius, m_colour, m_num_segments);

        std::unique_ptr<VisualComponent> clone() const override { return std::make_unique<SphereComponent>(*this); }

        void draw(
            const maths::Matrix44& transform,
            const maths::Matrix44& camera,
            const Scene&,
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
        DEFINE_SERIALIZATION_FUNCTIONS(m_dimensions, m_colour);

        std::unique_ptr<VisualComponent> clone() const override { return std::make_unique<CubeComponent>(*this); }

        void draw(
            const maths::Matrix44& transform,
            const maths::Matrix44& camera,
            const Scene&,
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
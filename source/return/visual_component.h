#pragma once

#include "lights.h"

#include "gfx/gfx_forward.h"
#include "maths/maths_forward.h"

#include <string>

namespace re
{
    class Scene;

    class VisualComponent
    {
    public:
        virtual void draw(
            const maths::Matrix44& transform,
            const maths::Matrix44& camera,
            const Scene&) const = 0;
        virtual void edit(const Scene&) = 0;
        virtual void relink(const Scene&) = 0;
    };

    class VAOComponent : public VisualComponent
    {
    public:
        void draw(
            const maths::Matrix44& transform,
            const maths::Matrix44& camera,
            const Scene&) const override;
        void edit(const Scene&) override;
        void relink(const Scene&) override;

    private:
        const gfx::VertexArray* m_vao = nullptr;
        const gfx::ShaderProgram* m_program = nullptr;
        const gfx::Texture* m_texture = nullptr;

        //needed by editor and for relinking
        std::string m_vao_name;
        std::string m_program_name;
        std::string m_texture_name;
    };
}
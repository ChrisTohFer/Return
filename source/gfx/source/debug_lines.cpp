#include "debug_lines.h"
#include "vertex_buffer.h"
#include "vertex_array_object.h"
#include "shader.h"

#include "glad/glad.h"

namespace gfx
{
    const char* g_debug_lines_vertex_shader =
R"(#version 330 core

layout (location = 0) in vec3 pos;

uniform mat4 camera;
uniform bool use_z;

void main()
{
    gl_Position = camera * vec4(pos, 1.0f);
    if(!use_z)
    {
        gl_Position.z = -1.0f;
    }
}
)";
    const char* g_debug_lines_fragment_shader =
R"(#version 330 core
out vec4 FragColor;
uniform vec3 colour;
void main()
{
    FragColor = vec4(colour,1.0f);
}
)";

    static ShaderProgram& debug_lines_shader_program()
    {
        static auto sp = []()
        {
            auto sp_ = ShaderProgram(
                VertexShader(g_debug_lines_vertex_shader),
                FragmentShader(g_debug_lines_fragment_shader)
                );
            return sp_;
        }();

        return sp;
    }

    void draw_line_impl(
        const std::vector<maths::Vector3>& vertices,
        const maths::Matrix44& camera,
        maths::Vector3 colour,
        bool use_z)
    {
        VertexBuffer vbuffer(vertices.data(), (int)vertices.size(), { VertexComponent::Vec3 });
        VertexArray vao(vbuffer, nullptr);

        glBindVertexArray(vao.id());
        debug_lines_shader_program().use();
        gfx::set_uniform(debug_lines_shader_program().uniform_location("camera"), camera);
        gfx::set_uniform(debug_lines_shader_program().uniform_location("use_z"), use_z);
        gfx::set_uniform(debug_lines_shader_program().uniform_location("colour"), colour);
        vao.draw_lines();
    }

    void draw_line(
        const std::vector<maths::Vector3>& points,
        const maths::Matrix44& camera,
        maths::Vector3 colour,
        bool use_z,
        bool loop)
    {
        if(points.size() <= 1)
        {
            return;
        }

        //create vector of positions and reserve required number
        std::vector<maths::Vector3> vertices;
        int num_vertices = (int)points.size() - loop ? 0 : 2;
        vertices.reserve(num_vertices);

        //push a pair of vertices for each line
        vertices.push_back(points.front());
        for(int i = 1; i < points.size() - 1; ++i)
        {
            //push vertex back twice, as end of previous line and beginning of next line
            vertices.push_back(points[i]);
            vertices.push_back(points[i]);
        }
        vertices.push_back(points.back());

        //push additional pair between end and start if looping
        if(loop)
        {
            vertices.push_back(points.back());
            vertices.push_back(points.front());
        }

        //create buffers
        draw_line_impl(vertices, camera, colour, use_z);
    }

    void draw_sphere(
        maths::Vector3 pos,
        float radius,
        const maths::Matrix44 &camera,
        maths::Vector3 colour,
        int num_segments,
        bool use_z)
    {
        if(num_segments < 2)
        {
            return;
        }

        //this would probably be more efficient to do via its own shader but oh well
        std::vector<maths::Vector3> vertices;
        vertices.resize(num_segments * 6);
        const auto ux = maths::Vector3::unit_x();
        const auto uy = maths::Vector3::unit_y();
        const auto uz = maths::Vector3::unit_z();

        float previous_c = 1.f;
        float previous_s = 0.f;

        for(int i = 1; i < num_segments; ++i)
        {
            const float phase = 2.f * maths::PI * (float)i / (float)num_segments;
            const float c = cos(phase);
            const float s = sin(phase);
            //xy plane
            vertices[i * 6]     = pos + radius * (ux * previous_c + uy * previous_s);
            vertices[i * 6 + 1] = pos + radius * (ux * c + uy * s);

            //yz plane
            vertices[i * 6 + 2] = pos + radius * (uy * previous_c + uz * previous_s);
            vertices[i * 6 + 3] = pos + radius * (uy * c + uz * s);

            //zx plane
            vertices[i * 6 + 4] = pos + radius * (uz * previous_c + ux * previous_s);
            vertices[i * 6 + 5] = pos + radius * (uz * c + ux * s);

            previous_c = c;
            previous_s = s;
        }

        //connect start and end in each plane
        vertices[0] = pos + radius * (ux * previous_c + uy * previous_s);
        vertices[1] = pos + radius * (ux);

        vertices[2] = pos + radius * (uy * previous_c + uz * previous_s);
        vertices[3] = pos + radius * (uy);

        vertices[4] = pos + radius * (uz * previous_c + ux * previous_s);
        vertices[5] = pos + radius * (uz);

        draw_line_impl(vertices, camera, colour, use_z);
    }
    
    void draw_aabb(
        maths::Vector3 min,
        maths::Vector3 max,
        const maths::Matrix44 &camera,
        maths::Vector3 colour,
        bool use_z)
    {
        std::vector<maths::Vector3> vertices;
        vertices.reserve(24);

        //square at min x
        vertices.push_back({min.x, min.y, min.z});
        vertices.push_back({min.x, min.y, max.z});
        vertices.push_back({min.x, min.y, max.z});
        vertices.push_back({min.x, max.y, max.z});
        vertices.push_back({min.x, max.y, max.z});
        vertices.push_back({min.x, max.y, min.z});
        vertices.push_back({min.x, max.y, min.z});
        vertices.push_back({min.x, min.y, min.z});

        //square at max x
        vertices.push_back({max.x, min.y, min.z});
        vertices.push_back({max.x, min.y, max.z});
        vertices.push_back({max.x, min.y, max.z});
        vertices.push_back({max.x, max.y, max.z});
        vertices.push_back({max.x, max.y, max.z});
        vertices.push_back({max.x, max.y, min.z});
        vertices.push_back({max.x, max.y, min.z});
        vertices.push_back({max.x, min.y, min.z});

        //connect min and max x
        vertices.push_back({min.x, min.y, min.z});
        vertices.push_back({max.x, min.y, min.z});
        vertices.push_back({min.x, max.y, min.z});
        vertices.push_back({max.x, max.y, min.z});
        vertices.push_back({min.x, max.y, max.z});
        vertices.push_back({max.x, max.y, max.z});
        vertices.push_back({min.x, min.y, max.z});
        vertices.push_back({max.x, min.y, max.z});

        draw_line_impl(vertices, camera, colour, use_z);
    }
}
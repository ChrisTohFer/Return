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
        int num_vertices = loop ? points.size() : points.size() - 2;
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
        VertexBuffer vbuffer(vertices.data(), vertices.size(), {VertexComponent::Vec3});
        VertexArray vao(vbuffer, nullptr);

        glBindVertexArray(vao.id());
        debug_lines_shader_program().use();
        gfx::set_uniform(debug_lines_shader_program().uniform_location("camera"), camera);
        gfx::set_uniform(debug_lines_shader_program().uniform_location("use_z"), use_z);
        gfx::set_uniform(debug_lines_shader_program().uniform_location("colour"), colour);
        vao.draw_lines();
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

        //this would be much more efficient to do via its own shader but oh well
        std::vector<maths::Vector3> points;
        points.resize(num_segments);
        auto ux = maths::Vector3::unit_x();
        auto uy = maths::Vector3::unit_y();
        auto uz = maths::Vector3::unit_z();

        //xy plane
        for(int i = 0; i < num_segments; ++i)
        {
            float phase = 2.f * maths::PI * (float)i / (float)num_segments;
            points[i] = (ux * cos(phase) + uy * sin(phase)) * radius;
        }
        draw_line(points, camera, colour, use_z, true);

        //yz plane
        for(int i = 0; i < num_segments; ++i)
        {
            float phase = 2.f * maths::PI * (float)i / (float)num_segments;
            points[i] = (uy * cos(phase) + uz * sin(phase)) * radius;
        }
        draw_line(points, camera, colour, use_z, true);

        //zx plane
        for(int i = 0; i < num_segments; ++i)
        {
            float phase = 2.f * maths::PI * (float)i / (float)num_segments;
            points[i] = (uz * cos(phase) + ux * sin(phase)) * radius;
        }
        draw_line(points, camera, colour, use_z, true);
    }
    
    void draw_aabb(
        maths::Vector3 min,
        maths::Vector3 max,
        const maths::Matrix44 &camera,
        maths::Vector3 colour,
        bool use_z)
    {
        std::vector<maths::Vector3> points;
        points.resize(4);

        points[0] = {min.x, min.y, min.z};
        points[1] = {min.x, min.y, max.z};
        points[2] = {min.x, max.y, max.z};
        points[3] = {min.x, max.y, min.z};
        draw_line(points, camera, colour, use_z, false);

        points[0] = {max.x, min.y, min.z};
        points[1] = {min.x, min.y, min.z};
        points[2] = {min.x, max.y, min.z};
        points[3] = {max.x, max.y, min.z};
        draw_line(points, camera, colour, use_z, false);

        points[0] = {max.x, min.y, max.z};
        points[1] = {max.x, min.y, min.z};
        points[2] = {max.x, max.y, min.z};
        points[3] = {max.x, max.y, max.z};
        draw_line(points, camera, colour, use_z, false);

        points[0] = {min.x, min.y, max.z};
        points[1] = {max.x, min.y, max.z};
        points[2] = {max.x, max.y, max.z};
        points[3] = {min.x, max.y, max.z};
        draw_line(points, camera, colour, use_z, false);
    }
}
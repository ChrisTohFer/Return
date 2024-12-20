#include "graphics_manager.h"

#include <assert.h>
#include <cstring>
#include <iostream>

namespace gfx
{
    static const char* get_gl_error_string()
    {
        auto error = glGetError();
        switch (error)
        {
        case GL_INVALID_ENUM:       return "GL_INVALID_ENUM";      break;
        case GL_INVALID_VALUE:      return "GL_INVALID_VALUE";     break;
        case GL_INVALID_OPERATION:  return "GL_INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:     return "GL_STACK_OVERFLOW";    break;
        case GL_STACK_UNDERFLOW:    return "GL_STACK_UNDERFLOW";   break;
        case GL_OUT_OF_MEMORY:      return "GL_OUT_OF_MEMORY";     break;
        case GL_NO_ERROR:           return "";     break;
        default:                    assert(!"Unrecognised error type"); return "";
        }
    }
    void report_gl_error()
    {
    #ifdef DEBUG
        auto error_string = get_gl_error_string();
        if (strcmp(error_string, ""))
        {
            std::cout << error_string << "\n";
        }
    #endif
    }

    //GraphicsManager ===========================================================

    void GraphicsManager::clear()
    {
        m_vertex_buffers.clear();
        m_element_buffers.clear();
        m_vertex_arrays.clear();
        m_vertex_shaders.clear();
        m_fragment_shaders.clear();
        m_shader_programs.clear();
    }

    void GraphicsManager::add(const char *name, std::unique_ptr<gfx::VertexBuffer> &&obj)   { m_vertex_buffers.emplace(name, std::move(obj)); }
    void GraphicsManager::add(const char* name, std::unique_ptr<gfx::ElementBuffer>&& obj)  { m_element_buffers.emplace(name, std::move(obj)); }
    void GraphicsManager::add(const char* name, std::unique_ptr<gfx::VertexArray>&& obj)    { m_vertex_arrays.emplace(name, std::move(obj)); }
    void GraphicsManager::add(const char* name, std::unique_ptr<gfx::VertexShader>&& obj)   { m_vertex_shaders.emplace(name, std::move(obj)); }
    void GraphicsManager::add(const char* name, std::unique_ptr<gfx::FragmentShader>&& obj) { m_fragment_shaders.emplace(name, std::move(obj)); }
    void GraphicsManager::add(const char* name, std::unique_ptr<gfx::ShaderProgram>&& obj)  { m_shader_programs.emplace(name, std::move(obj)); }
    void GraphicsManager::add(const char* name, std::unique_ptr<gfx::Texture>&& obj)        { m_textures.emplace(name, std::move(obj)); }

    template<typename MapT>
    static std::vector<std::string> get_map_keys(MapT&& m)
    {
        std::vector<std::string> keys;
        for(auto& entry : m)
        {
            keys.push_back(entry.first);
        }
        return keys;
    }
    template<typename ValueT>
    static const ValueT* get_map_value(const char* name, const std::unordered_map<std::string, std::unique_ptr<ValueT>>& map)
    {
        auto find_iter = map.find(name);
        return find_iter == map.end() ? nullptr : find_iter->second.get();
    }

    std::vector<std::string> GraphicsManager::vertex_buffer_names() const
    {
        return get_map_keys(m_vertex_buffers);
    }
    std::vector<std::string> GraphicsManager::element_buffer_names() const
    {
        return get_map_keys(m_element_buffers);
    }
    std::vector<std::string> GraphicsManager::vertex_array_names() const
    {
        return get_map_keys(m_vertex_arrays);
    }
    std::vector<std::string> GraphicsManager::vertex_shader_names() const
    {
        return get_map_keys(m_vertex_shaders);
    }
    std::vector<std::string> GraphicsManager::fragment_shader_names() const
    {
        return get_map_keys(m_fragment_shaders);
    }
    std::vector<std::string> GraphicsManager::shader_program_names() const
    {
        return get_map_keys(m_shader_programs);
    }
    std::vector<std::string> GraphicsManager::texture_names() const
    {
        return get_map_keys(m_textures);
    }

    const gfx::VertexBuffer* GraphicsManager::vertex_buffer(const char* name) const
    {
        return get_map_value(name, m_vertex_buffers);
    }
    const gfx::ElementBuffer* GraphicsManager::element_buffer(const char* name) const
    {
        return get_map_value(name, m_element_buffers);
    }
    const gfx::VertexArray* GraphicsManager::vertex_array(const char* name) const
    {
        return get_map_value(name, m_vertex_arrays);
    }
    const gfx::VertexShader* GraphicsManager::vertex_shader(const char* name) const
    {
        return get_map_value(name, m_vertex_shaders);
    }
    const gfx::FragmentShader* GraphicsManager::fragment_shader(const char* name) const
    {
        return get_map_value(name, m_fragment_shaders);
    }
    const gfx::ShaderProgram* GraphicsManager::shader_program(const char* name) const
    {
        return get_map_value(name, m_shader_programs);
    }
    const gfx::Texture *GraphicsManager::texture(const char *name) const
    {
        return get_map_value(name, m_textures);
    }
}
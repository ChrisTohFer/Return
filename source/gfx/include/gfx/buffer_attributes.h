#pragma once

#include "maths/maths.h"
#include "maths/vector2.h"

namespace gfx
{
    enum class BufferAttributeType
    {
        #define DEFINE_VERTEX_ATTRIBUTE(a, b, c, d, e) a,
        #include "buffer_attributes.inl"
        #undef DEFINE_VERTEX_ATTRIBUTE

        Num
    };

    struct BufferAttributeInfo
    {
        const char* name; 
        int size;
        bool instanced;
    };

    constexpr BufferAttributeInfo c_buffer_attribute_info[(int)BufferAttributeType::Num] =
    {
        #define DEFINE_VERTEX_ATTRIBUTE(a, b, c, d, e) { #a , sizeof(b), e },
        #include "buffer_attributes.inl"
        #undef DEFINE_VERTEX_ATTRIBUTE
    };

    constexpr int attribute_size(BufferAttributeType type)
    {
        return c_buffer_attribute_info[(int)type].size;
    }

    constexpr const char* attribute_name(BufferAttributeType type)
    {
        return c_buffer_attribute_info[(int)type].name;
    }

    constexpr bool attribute_instanced(BufferAttributeType type)
    {
        return c_buffer_attribute_info[(int)type].instanced;
    }

    int vertex_size(const BufferAttributeType*, int n);

    int attribute_opengl_type(BufferAttributeType);
    int attribute_opengl_count(BufferAttributeType);
}
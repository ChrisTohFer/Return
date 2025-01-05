#pragma once

#include "maths/maths.h"
#include "maths/vector2.h"

#include <cstdint>

namespace gfx
{
    enum class BufferAttributeType
    {
        #define DEFINE_VERTEX_ATTRIBUTE(a, b, c, d, e, f) a,
        #include "buffer_attributes.inl"
        #undef DEFINE_VERTEX_ATTRIBUTE

        Num
    };

    struct BufferAttributeInfo
    {
        const char* name; 
        int size;
        bool instanced;
        int location;
    };

    constexpr BufferAttributeInfo c_buffer_attribute_info[(int)BufferAttributeType::Num] =
    {
        #define DEFINE_VERTEX_ATTRIBUTE(a, b, c, d, e, f) { #a , sizeof(b), e, f },
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

    constexpr int attribute_location(BufferAttributeType type)
    {
        return c_buffer_attribute_info[(int)type].location;
    }

    int vertex_size(const BufferAttributeType*, int n);
    void bind_attribute(BufferAttributeType type, int stride, uint64_t offset);
}
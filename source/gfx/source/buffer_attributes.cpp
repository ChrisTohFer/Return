#include "buffer_attributes.h"

#include "glad/glad.h"

#include <assert.h>

namespace gfx
{
    int vertex_size(const BufferAttributeType* first_component, int n)
    {
        int result = 0;
        for(int i = 0; i < n; ++i) { result += attribute_size(first_component[i]); }
        return result;
    }

    int attribute_opengl_type(BufferAttributeType type)
    {
        static const int sizes[(int)BufferAttributeType::Num] = 
        {
        #define DEFINE_VERTEX_ATTRIBUTE(a, b, c, d, e) c,
        #include "buffer_attributes.inl"
        #undef DEFINE_VERTEX_ATTRIBUTE
        };

        return sizes[(int)type];
    }
    int attribute_opengl_count(BufferAttributeType type)
    {
        static const int counts[(int)BufferAttributeType::Num] = 
        {
        #define DEFINE_VERTEX_ATTRIBUTE(a, b, c, d, e) d,
        #include "buffer_attributes.inl"
        #undef DEFINE_VERTEX_ATTRIBUTE
        };

        return counts[(int)type];
    }
}
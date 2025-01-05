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
        #define DEFINE_VERTEX_ATTRIBUTE(a, b, c, d, e, f) c,
        #include "buffer_attributes.inl"
        #undef DEFINE_VERTEX_ATTRIBUTE
        };

        return sizes[(int)type];
    }
    int attribute_opengl_count(BufferAttributeType type)
    {
        static const int counts[(int)BufferAttributeType::Num] = 
        {
        #define DEFINE_VERTEX_ATTRIBUTE(a, b, c, d, e, f) d,
        #include "buffer_attributes.inl"
        #undef DEFINE_VERTEX_ATTRIBUTE
        };

        return counts[(int)type];
    }

    void bind_attribute(BufferAttributeType type, int stride, uint64_t offset)
    {
        int location = attribute_location(type);
        if(attribute_size(type) > 16)
        {
            //should only be matrices here for now
            assert(attribute_size(type) == 64);
            for(int i = 0; i < 4; ++i)
            {
                glVertexAttribPointer(location + i, 4, GL_FLOAT, GL_FALSE, stride, (void*)(offset + 16ull * (uint64_t)i));
                glEnableVertexAttribArray(location + i);
                glVertexAttribDivisor(location + i, attribute_instanced(type) ? 1 : 0);
            }
        }
        else
        {
            glVertexAttribPointer(location, attribute_opengl_count(type), attribute_opengl_type(type), GL_FALSE, stride, (void*)offset);
            glEnableVertexAttribArray(location);
            glVertexAttribDivisor(location, attribute_instanced(type) ? 1 : 0);
        }
    }
}
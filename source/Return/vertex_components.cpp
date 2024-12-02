#include "vertex_components.h"

#include <assert.h>

namespace gfx
{
    int component_size(VertexComponent type)
    {
        switch(type)
        {
        case VertexComponent::Float: return sizeof(float);
        case VertexComponent::Bool:  return sizeof(bool);
        case VertexComponent::Int:   return sizeof(int);
        case VertexComponent::Vec2:  return sizeof(float) * 2;
        case VertexComponent::Vec3:  return sizeof(float) * 3;
        }
        assert(!"Encountered an invalid vertex component type.");
        return 0;
    }
    
    int vertex_size(const VertexComponent* first_component, int n)
    {
        int result = 0;
        for(int i = 0; i < n; ++i) { result += component_size(first_component[i]); }
        return result;
    }
}
#ifndef DEFINE_VERTEX_ATTRIBUTE
static_assert(false, "buffer_attributes.inl should only be included in buffer_attributes.h for use with DEFINE_VERTEX_ATTRIBUTE!");
#else

//                      Name/Type              Data type           Opengl type  Count  Instanced
DEFINE_VERTEX_ATTRIBUTE(Translation,           maths::Vector3,     GL_FLOAT,    3,     false)
DEFINE_VERTEX_ATTRIBUTE(TextureUVs,            maths::Vector2,     GL_FLOAT,    2,     false)
DEFINE_VERTEX_ATTRIBUTE(InstanceTransform,     maths::Matrix44,    GL_FLOAT,    16,    true)

#endif
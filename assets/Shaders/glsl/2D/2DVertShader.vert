#version 330 core

layout (location = 0) in vec3 aPos;  // Vertex position
layout (location = 1) in vec2 aTexCoord; // Texture coordinates

uniform mat4 viewProjection; // Combined projection and view matrix

out vec2 TexCoord; // Pass to fragment shader

void main()
{
    gl_Position = viewProjection * vec4(aPos, 1.0); // Transform position
    TexCoord = aTexCoord; // Pass texture coordinates
}

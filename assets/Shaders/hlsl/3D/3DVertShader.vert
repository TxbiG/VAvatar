#version 450

layout (location = 0) in vec3 aPos; // Vertex position
layout (location = 1) in vec3 aNormal; // Vertex normal (for lighting)
layout (location = 2) in vec2 aTexCoord; // Texture coordinates

uniform mat4 model; // Model transformation matrix
uniform mat4 viewProjection; // Combined view and projection matrix

out vec3 FragPos; // World position of the fragment
out vec3 Normal;  // Pass normal to fragment shader
out vec2 TexCoord; // Pass texture coordinates

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0)); // Transform position by model matrix
    Normal = mat3(transpose(inverse(model))) * aNormal; // Correct normal for non-uniform scaling
    TexCoord = aTexCoord;

    gl_Position = viewProjection * vec4(FragPos, 1.0); // Final position in clip space
}
#version 330 core

in vec2 TexCoord; // From vertex shader

uniform sampler2D textureSampler; // Texture to be rendered

out vec4 FragColor;

void main()
{
    FragColor = texture(textureSampler, TexCoord); // Sample the texture
}
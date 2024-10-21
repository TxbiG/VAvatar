#version 330 core

out vec4 FragColor;     // Outputs colors in RGBA

in vec3 color;          // Inputs the color from the Vertex Shader
in vec2 texCoord;       // Inputs the texture coordinates from the Vertex Shader

uniform sampler2D tex0; // Gets the Texture Unit from the main function
uniform vec3 tintColor;  // Uniform for the tint color

void main()
{
    vec4 sampled = texture(tex0, texCoord); // Sample the texture
    FragColor = vec4(sampled.rgb * tintColor, sampled.a); // Multiply by tint color
}
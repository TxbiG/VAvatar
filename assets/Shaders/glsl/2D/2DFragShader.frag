#version 330 core

in vec3 FragPos; // World position
in vec3 Normal;  // Normal vector
in vec2 TexCoord; // Texture coordinates

uniform vec3 lightPos; // Position of the light source
uniform vec3 viewPos;  // Camera position
uniform sampler2D textureSampler; // Texture to sample

out vec4 FragColor;

void main()
{
    // Ambient lighting
    vec3 ambient = 0.1 * vec3(1.0, 1.0, 1.0);

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);

    // Specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = 0.5 * spec * vec3(1.0, 1.0, 1.0);

    // Combine results
    vec3 lighting = (ambient + diffuse + specular) * texture(textureSampler, TexCoord).rgb;

    FragColor = vec4(lighting, 1.0); // Final color
}

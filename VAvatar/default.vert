#version 330 core

layout (location = 0) in vec3 aPos;     // Positions/Coordinates
layout (location = 1) in vec3 aColor;   // Colors
layout (location = 2) in vec2 aTex;     // Texture Coordinates


out vec3 color;                 // Outputs the color for the Fragment Shader
out vec2 texCoord;              // Outputs the texture coordinates to the fragment shader
uniform float scale;            // Controls the scale of the vertices


void main()
{
	// Outputs the positions/coordinates of all vertices
	gl_Position = vec4(aPos.x + aPos.x * scale, aPos.y + aPos.y * scale, aPos.z + aPos.z * scale, 1.0);
	// Assigns the colors from the Vertex Data to "color"
	color = aColor;
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	texCoord = aTex;
}
#version 330 core

layout (location = 0) in vec3 aPos;     // Positions/Coordinates
layout (location = 1) in vec3 aColor;   // Colors
layout (location = 2) in vec2 aTex;     // Texture Coordinates


out vec3 color;       // Outputs the color for the Fragment Shader
out vec2 texCoord;              			// Outputs the texture coordinates to the fragment shader

uniform mat4 transform; 		// Transformation matrix (rotation, scaling, etc.)
uniform float scale;     					// Separate scale factor

void main()
{
	gl_Position = transform * vec4(aPos, 1.0); 	// Apply transformation
	color = aColor;								// Assigns the colors from the Vertex Data to "color"
	texCoord = aTex;							// Assigns the texture coordinates from the Vertex Data to "texCoord"
}
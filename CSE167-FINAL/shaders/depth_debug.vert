#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

// Uniform variables can be updated by fetching their location and passing values to that location
out vec2 texOutput;


void main()
{
    gl_Position = vec4(position, 1.0);
	texOutput = texCoord;
}
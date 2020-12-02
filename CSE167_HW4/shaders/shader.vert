#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform bool reflective;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. You can define as many
// extra outputs as you need.
out vec3 normalOutput;
out vec3 posOutput;

void main()
{
	
	normalOutput = mat3(transpose(inverse(model))) * normal;
	posOutput = vec3(model * vec4(position, 1.0));
	gl_Position = projection * view * model * vec4(position, 1.0);
}
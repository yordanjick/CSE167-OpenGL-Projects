#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

layout (location = 0) in vec3 position;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 lightMat;
uniform mat4 model;

void main()
{
    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = lightMat * model * vec4(position, 1.0);
}
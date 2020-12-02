#version 330 core

layout(location = 0) out vec4 fragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
	fragColor = texture(skybox, TexCoords);
}
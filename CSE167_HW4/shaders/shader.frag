#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 normalOutput;
in vec3 posOutput;

uniform vec3 eye;
uniform samplerCube skybox;
uniform vec3 color;
uniform bool normalColor;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{
	if(normalColor)
	{
		// vec3 normColor = normalize(normalOutput) + 1;
		// fragColor = vec4(normColor / 2, 1.0);
		fragColor = vec4(color, 1.0);
	}
	else
	{
		vec3 I = normalize(posOutput - eye);
		vec3 R = reflect(I, normalize(normalOutput));
		fragColor = vec4(texture(skybox, R).rgb, 1.0);
	}
}
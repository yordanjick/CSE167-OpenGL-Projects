#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec2 texOutput;

uniform sampler2D depthMap;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

float near = 0.1;
float far = 100.0;

void main()
{
	float depthValue = texture(depthMap, texOutput).r;
	
	// float z = texture(depthMap, texOutput).r;
	// z = (2.0 * near * far) / (far + near - z * (far - near));
	// float depthValue = z / far;
	
	// depthValue = 1.0 - (1.0 - depthValue) * 25.0;
	// fragColor = vec4(vec3(depthValue), 1.0);
	fragColor = vec4(vec3(depthValue), 1.0); // orthographic
}
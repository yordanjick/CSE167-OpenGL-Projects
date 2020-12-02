#version 330 core
// This is a sample fragment shader.

in vec2 texOutput;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float exposure;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{
	const float gamma = 1.3;
	vec3 hdrColor = texture(scene, texOutput).rgb;
	vec3 bloomColor = texture(bloomBlur, texOutput).rgb;
	hdrColor += bloomColor; // blending
	// tone mapping
	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
	
	// gamma correct
	result = pow(result, vec3(1.0/gamma));
	fragColor = vec4(result, 1.0);
	// fragColor = vec4(texture(scene, texOutput).rgb, 1.0);
}
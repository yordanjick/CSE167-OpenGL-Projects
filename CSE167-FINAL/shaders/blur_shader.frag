#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec2 texOutput;

uniform sampler2D image;

uniform bool horizontal;
// uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
uniform float weight[5] = float[] (0.247027, 0.2145946, 0.1316216, 0.064054, 0.036216);

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{
	vec2 tex_offset = 1.0 / textureSize(image, 0); // get size of texel
	vec3 result = texture(image, texOutput).rgb * weight[0]; // blur factor from current fragment
	if(horizontal)
	{
		for(int i = 1; i < 5; i++)
		{
			result += texture(image, texOutput + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
			result += texture(image, texOutput - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
		}
	}
	else
	{
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, texOutput + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(image, texOutput - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
	fragColor = vec4(result, 1.0);
	
}
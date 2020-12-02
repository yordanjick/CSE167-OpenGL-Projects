#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 normalOutput;
in vec3 posOutput;
in vec2 texOutput;
in vec4 lightFragOutput;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool ignoreLight;
uniform bool toonShading;

// You can output many things. The first vec4 type output determines the color of the fragment
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

float ShadowCalc(vec4 lightFragPos, float bias)
{
	// perspective divide
	vec3 projCoord = (lightFragPos.xyz) / (lightFragPos.w);
	// want to normalaize coords to range [0, 1]
	projCoord = projCoord * 0.5 + 0.5;
	// get closest depth value from light's perspective
	float closestDepth = texture(shadowMap, projCoord.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoord.z;
	// check if current frag pos is in shadow
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	return shadow;
}

void main()
{
	if(ignoreLight)
	{
		fragColor = texture(texture_diffuse1, texOutput) + 
		texture(texture_specular1, texOutput) * .5;
	}
	else
	{
		vec3 color = texture(texture_diffuse1, texOutput).rgb;
		vec3 normal = normalize(normalOutput);
		vec3 lightColor = vec3(1.0);
		
		vec3 ambient = 0.25 * color;
	
		vec3 lightDir = normalize(lightPos - posOutput);
		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = diff * color;
		
		float intensity = dot(lightDir, normal);
	
		vec3 viewDir = normalize(viewPos - posOutput);
		vec3 reflectDir = reflect(-lightDir, normal);
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(normal, reflectDir), 0.0), 64);
		
		vec3 specular = spec * texture(texture_diffuse1, texOutput).rgb;
		
		float bias = max(0.05 * (1 - dot(normal, lightDir)), 0.005);
		// float bias = 0;
		float shadow = ShadowCalc(lightFragOutput, bias);

		fragColor = vec4((((1.0 - shadow) * (diffuse + specular)) * color), 1.0);
		// fragColor = vec4(((ambient + diffuse + specular) * color), 1.0);
		// fragColor = vec4(texture(shadowMap, texOutput).rgb, 1.0);
		
		if(toonShading)
		{
			if (intensity > 0.95)
				fragColor = vec4(fragColor.rgb + ambient * color, 1.0);
			else if (intensity > .75)
				fragColor = vec4(fragColor.rgb * .75 + ambient * color, 1.0);
			else if (intensity > .5)
				fragColor = vec4(fragColor.rgb * .5 + ambient * color, 1.0);
			else if (intensity > .25)
				fragColor = vec4(fragColor.rgb * .25 + ambient * color, 1.0);
			else
				fragColor = vec4(ambient * color, 1.0);
		}
		else
			fragColor = vec4(fragColor.rgb + ambient * color, 1.0);
	}
	// check if output of fragment higher than arbitrary threshold
	// float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	float brightness = dot(fragColor.rgb, vec3(0.2452, 0.7591, 0.3493));
	if(brightness > 1.0)
		brightColor = vec4(fragColor.rgb, 1.0);
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
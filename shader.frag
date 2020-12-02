#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 normalOutput;
in vec3 posOutput;

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform Material material;
uniform vec3 lightColor;
uniform vec3 objColor;

uniform vec3 lightPos;
uniform float lightConst;
uniform float lightLinear;

uniform vec3 viewPos;

uniform bool normalColor;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{
	if(normalColor)
	{
		vec3 normColor = normalize(normalOutput) + 1;
		fragColor = vec4(normColor / 2, 1.0);
	}
	else
	{
		// find attenuation, which is light intensity falloff by distance
		float distance = length(lightPos - posOutput);
		float attenuation = 1.0 / (lightConst + lightLinear * distance);
		
		// ambient light is default light level of entire object
		vec3 ambient = material.ambient * lightColor * attenuation;
	
		// diffuse light depends on angle of vertex normal and angle of light source
		vec3 norm = normalize(normalOutput);
		vec3 lightDir = normalize(lightPos - posOutput);
	
		float diff = max(dot(norm, lightDir), 0);
		vec3 diffuse = lightColor * (diff * material.diffuse) * attenuation;
	
		// specular lighting based on angle between the reflected light and the viewport
		vec3 viewDir = normalize(viewPos - posOutput);
		vec3 reflectDir = reflect(-lightDir, norm);
	
		float spec = pow(max(dot(viewDir, reflectDir), 0), material.shininess);
		vec3 specular = lightColor * (material.specular * spec) * attenuation;
	
		vec3 result = ambient + diffuse + specular;

		// Use the color passed in. An alpha of 1.0f means it is not transparent.
		fragColor = vec4(result, 1.0);
	}
}
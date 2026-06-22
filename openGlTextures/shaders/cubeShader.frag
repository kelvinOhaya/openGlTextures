#version 330 core
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;

void main()
{
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 lightDir = normalize(lightPos - FragPos);
	float ambientStrength = 0.1;
	vec3 ambient = lightColor*ambientStrength;

	vec3 norm = normalize(Normal);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 lightDirection = normalize(lightPos-FragPos);
	float diffuseImpact = max(dot(lightDirection,norm), 0.0);
	vec3 diffuse = diffuseImpact*lightColor;
	vec3 result = (diffuse+ambient+specular)*objectColor;

	FragColor = vec4(result, 1.0);
}

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
    // 1. Ambient Component
    float ambientStrength = 0.3;
    vec3 ambient = lightColor * ambientStrength;
    
    // 2. Vectors Setup
    vec3 norm = normalize(Normal);
    // Use a unified dynamic direction from the fragment to the actual light position
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // 3. Diffuse Component
    float diffuseImpact = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseImpact * lightColor;
    
    // 4. Specular Component (Phong Reflection Model)
    float specularStrength = 0.5;
    // Reflect expects vector pointing from light source TO fragment, so we negate lightDir
    vec3 reflectDir = reflect(-lightDir, norm); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    // 5. Final Assembly
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
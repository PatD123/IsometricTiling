#version 330 core

in vec3 Color;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 camPos;

out vec4 FragColor;

void main()
{
    // Ambient

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular

    float specularStrength = 0.5;
    vec3 viewDir = normalize(camPos - FragPos); // From frag to cam
    vec3 reflectDir = reflect(-lightDir, norm);  // Across norm, pointing away from frag.
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = specularStrength * spec * lightColor;

    FragColor = vec4(Color * (ambient + diffuse + specular), 1.0);  
} 
#version 330 core

in vec3 Color;
in vec3 FragPos;
in vec4 LightSpaceFragPos;
in vec3 Normal;

uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 camPos;

uniform sampler2D shadowMap;

out vec4 FragColor;

float ShadowCalc(vec4 fragPos, vec3 lightDir, vec3 normal) {
    // Perform perspective divide
    vec3 projCoords = fragPos.xyz / fragPos.w;

    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // Check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  

    return shadow;
}

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

    // Shadow Maps
    float shadow = ShadowCalc(LightSpaceFragPos, lightDir, norm);

    FragColor = vec4(Color * (ambient + (1.0 - shadow) * (diffuse + specular)), 1.0);  
} 
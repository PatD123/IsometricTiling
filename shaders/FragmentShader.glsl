#version 330 core

in vec3 Color;

uniform vec3 lightColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(Color * lightColor, 1.0f);
} 
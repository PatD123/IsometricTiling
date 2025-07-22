#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in mat4 aTransform;

out vec3 Color;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 proj_view;

void main()
{
    gl_Position = proj_view * aTransform * model * vec4(aPos, 1.0);
    Color = aColor;
    Normal = aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
}
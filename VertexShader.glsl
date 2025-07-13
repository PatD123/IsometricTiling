#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 3) in mat4 aTransform;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    gl_Position = proj * view * aTransform * model * vec4(aPos, 1.0);
}
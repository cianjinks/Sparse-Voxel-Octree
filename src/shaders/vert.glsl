#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

uniform mat4 u_ProjectionMatrix;

out vec2 vTex;

void main()
{
    gl_Position = u_ProjectionMatrix * vec4(aPos, 1.0);
    vTex = aTex;
}
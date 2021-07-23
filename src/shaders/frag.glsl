#version 430 core

in vec2 vTex;

uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
    FragColor = texture(Texture, vTex);
}
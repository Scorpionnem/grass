#version 330 core

out vec4 FragColor;

in vec3	Normal;
in vec3	FragPos;

uniform vec3 color;

uniform sampler2D   camtexture;

void main()
{
    FragColor = vec4(color, 1.0);
}

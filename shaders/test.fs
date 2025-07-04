#version 330 core

out vec4 FragColor;

in vec2    TexPos;

uniform sampler2D   screenTexture;

void main()
{
	vec3 color = texture(screenTexture, TexPos).rgb;

    FragColor = vec4(color, 1.0);
}

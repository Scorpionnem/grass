#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
	vec2	uv = TexCoords;
	vec3 color = texture(screenTexture, uv).rgb;

	FragColor = vec4(vec3(1), 1.0);
}

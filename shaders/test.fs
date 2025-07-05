#version 330 core

out vec4 FragColor;

in vec2    TexPos;

uniform sampler2D   screenTexture;

vec3 ditheredQuantize(vec2 TexPos, vec3 color, int levels) {
	// 4x4 Bayer matrix (values normalized to [0, 1])
	float bayer[16] = float[](
		0.0 / 16.0,		8.0 / 16.0,		2.0 / 16.0,		10.0 / 16.0,
		12.0 / 16.0,	4.0 / 16.0,		14.0 / 16.0,	6.0 / 16.0,
		3.0 / 16.0,		11.0 / 16.0,	1.0 / 16.0,		9.0 / 16.0,
		15.0 / 16.0,	7.0 / 16,		13.0 / 16.0,	5.0 / 16.0
	);

	// Get 4x4 index from fragment coordinates
	int x = int(mod(TexPos.x, 4.0));
	int y = int(mod(TexPos.y, 4.0));
	float threshold = bayer[y * 4 + x];

	// Apply threshold as offset before quantizing
	vec3 dithered = color + (1.0 / float(levels)) * (threshold - 0.5);
	return (floor(clamp(dithered, 0.0, 1.0) * float(levels)) / float(levels));
}

void main()
{
	vec3 color = texture(screenTexture, TexPos).rgb;

    color = ditheredQuantize(TexPos, color, 8);

    FragColor = vec4(color, 1.0);
}

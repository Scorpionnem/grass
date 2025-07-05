#version 330 core

out vec4 FragColor;
in vec2 TexCoords;
in vec2 FragPos;

bool	isUnderwater = false;

uniform	vec3	viewPos;
uniform	float	time;

uniform sampler2D screenTexture;

float hash(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);

    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f); // Smooth interpolation

    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float fbm(vec2 p)
{
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    for (int i = 0; i < 5; i++)
    {
        value += amplitude * noise(p * frequency);
        frequency *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}

float   amplitude = 5.0;
float   scale = 0.016;

float height(vec2 pos)
{
    return fbm(pos * scale) * amplitude;
}

float   water_level = 15.0;

float VignetteIntensity = 0.5;
float VignetteRadius = 0.75;

void main()
{
	vec2	uv = TexCoords;
	float	waterLevel = water_level;
	float n = fbm(viewPos.xz * scale + (time / 50));
    waterLevel += n * amplitude;

	if (viewPos.y < waterLevel)
		isUnderwater = true;

	vec3 color = texture(screenTexture, uv).rgb;

	if (isUnderwater)
	{
		color += vec3(0.2, 0.4, 1.0) / 2;
		color -= vec3(0.1);
	}

    vec2 centeredUV = uv - vec2(0.5);
    float dist = length(centeredUV);

    float vignette = smoothstep(VignetteRadius, VignetteRadius - 0.3, dist);
    vignette = mix(1.0, vignette, VignetteIntensity);

	color = clamp(color, 0.0, 1.0);
	FragColor = vec4(color * vignette, 1.0);
}

#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 instanceModel;

uniform mat4 view;
uniform mat4 projection;

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

float   amplitude = 50.0;
float   scale = 0.01;

float height(vec2 pos)
{
    return fbm(pos * scale) * amplitude;
}

void main()
{
    vec3    pos = aPos;
    vec4 worldPosXZ = instanceModel * vec4(pos.x, 0.0, pos.z, 1.0);
    vec2 worldXZ = worldPosXZ.xz;
    float n = fbm(worldXZ * scale);
    pos.y += n * amplitude;
	gl_Position = projection * view * instanceModel * vec4(pos, 1.0);
}

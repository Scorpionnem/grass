#version 330 core

out vec4 FragColor;

in vec3	Normal;
in vec3	FragPos;

uniform vec3    viewPos;

float   fogDistance = 300;

uniform sampler2D   grass_texture;
uniform sampler2D   stone_texture;
uniform sampler2D   snow_texture;

void main()
{
	vec3 color = Normal;
    float   dist = length(FragPos - viewPos);

    if (dist > fogDistance + 10)
        discard ;

    float steepness = 1.0f - dot(Normal, vec3(0, 1, 0));

    if (steepness < 0.06)
        color = texture(grass_texture, FragPos.xz).rgb;
    else
        color = texture(stone_texture, FragPos.xz).rgb;

    dist = clamp(dist / fogDistance, 0.0, 1.0);

    color = mix(color, vec3(0.4, 0.4, 1.0), dist);

    FragColor = vec4(color, 1.0);
}

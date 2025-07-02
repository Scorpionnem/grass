#version 330 core

out vec4 FragColor;
in vec3 FragPos;

float   fogDistance = 300;

uniform vec3 viewPos;

void main()
{
	vec3 color = vec3(0.2, 0.1, 1.0);
    float   dist = length(FragPos - viewPos);

    if (dist > fogDistance + 10)
        discard ;

    dist = clamp(dist / fogDistance, 0.0, 1.0);

    color = mix(color, vec3(0.4, 0.4, 1.0), dist);
    FragColor = vec4(color, 0.65);
}

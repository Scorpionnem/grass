#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4    view;
uniform mat4    projection;
uniform mat4    model;

out vec3    Normal;
out vec3    WorldFragPos;
out vec3    FragPos;

out vec4    clipSpacePos;

void main()
{
    vec3 pos = aPos;
	FragPos = aPos;

    pos.x += gl_InstanceID;
    vec4 worldPos = model * vec4(pos, 1.0);
    WorldFragPos = worldPos.xyz;

    gl_Position = projection * view * worldPos;
    clipSpacePos = gl_Position;
}

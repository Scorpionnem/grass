#version 330 core

out vec4 FragColor;

in vec3	Normal;
in vec3	FragPos;
in vec3 WorldFragPos;

uniform sampler2D   depthTex;

float LinearizeDepth(float depth, float near, float far)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (((2.0 * near * far) / (far + near - z * (far - near)) - near) / (far - near));
}

in vec4 clipSpacePos;

void main()
{
    vec3 ndc = clipSpacePos.xyz / clipSpacePos.w;
    ndc.xy = ndc.xy / 2 + 0.5;

    float    depthValue = texture(depthTex, ndc.xy).r;
    
    depthValue = 1 - LinearizeDepth(depthValue, 0.1, 448);

    vec3    color = vec3(depthValue);

    FragColor = vec4(color, 1.0);
}

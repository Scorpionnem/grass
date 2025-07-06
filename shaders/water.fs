#version 330 core

out vec4 FragColor;
in vec3 FragPos;
in vec3 Normal;

uniform float RENDER_DISTANCE;
float   fogDistance = RENDER_DISTANCE - (RENDER_DISTANCE / 3.5);

uniform sampler2D depthTex;

uniform vec3 viewPos;
in vec4 worldPos;
in vec4 clipSpacePos;

void main()
{
    vec3 color = vec3(1);
	vec3 closeColor = vec3(0.5, 0.8, 0.87);
	vec3 farColor = vec3(0.0, 0.3, 1.0);
    float   dist = length(FragPos - viewPos);

    if (dist > fogDistance + 10)
        discard ;

    dist = clamp(dist / fogDistance, 0.0, 1.0);

    vec3    lightDirection = vec3(-0.8f, -0.4f, -0.45f);
    vec3    lightAmbient = vec3(0.2f, 0.2f, 0.2f);
    vec3    lightDiffuse = vec3(0.5f, 0.5f, 0.5f);
    vec3    lightSpecular = vec3(1.0f, 1.0f, 1.0f);
    
    float   shininess = 64.0f;
    float   actualShiness = 1;


    vec3 ndc = clipSpacePos.xyz / clipSpacePos.w;
    ndc.xy = ndc.xy / 2 + 0.5;
    float depth = texture(depthTex, ndc.xy).x * 3;


    color = mix(closeColor, farColor, depth);

    vec3 ambient = lightAmbient * color;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-lightDirection);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightDiffuse * diff * color;  
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = lightSpecular * spec * actualShiness;  
        
    vec3 result = ambient + diffuse + specular;

    //APPLY FOG
    result = mix(result, vec3(0.6, 0.8, 1.0), dist);
    result = clamp(result, 0.0, 1.0);

    FragColor = vec4(result, 0.45 + (specular / 2));
}

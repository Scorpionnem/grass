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

uniform bool getDepth;

float   near = 0.1;
float   far = RENDER_DISTANCE;

    float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    vec3 color = vec3(1);
	vec3 closeColor = vec3(0.3, 0.8, 0.87);
	vec3 farColor = vec3(0.0, 0.2, 1.0);
    float   dist = length(FragPos - viewPos);

    if (dist > fogDistance + 10)
        discard ;

    float farPlaneDist = clamp(dist / RENDER_DISTANCE, 0.0, 1.0);
    dist = clamp(dist / fogDistance, 0.0, 1.0);

    if (getDepth == true)
    {
        FragColor = vec4(vec3(farPlaneDist), 1.0);
        return ;
    }

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3    lightDirection = vec3(-0.8f, -0.4f, -0.45f);
    vec3    lightAmbient = vec3(0.2f, 0.2f, 0.2f);
    vec3    lightDiffuse = vec3(1.0f, 1.0f, 1.0f);
    vec3    lightSpecular = vec3(1.0f, 1.0f, 1.0f);
    
    float   shininess = 256.0f;
    float   actualShiness = 1;


    vec3 ndc = clipSpacePos.xyz / clipSpacePos.w;
    ndc.xy = ndc.xy / 2 + 0.5;
    float distToTerrain = texture(depthTex, ndc.xy).x;
    distToTerrain = (LinearizeDepth(texture(depthTex, ndc.xy).r) - near) / (far - near);

    float waterDist = length(FragPos - viewPos);
    waterDist = clamp(waterDist / RENDER_DISTANCE, 0.0, 1.0);

    float waterDepth = abs(distToTerrain - waterDist);

    color = mix(closeColor, farColor, 1 - exp(-waterDepth * 10));
    color = clamp(color, 0, 1);

    vec3 ambient = lightAmbient * color;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-lightDirection);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightDiffuse * diff * color;  
    
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = lightSpecular * spec * actualShiness;  
        
    vec3 result = ambient + diffuse + specular;

    // APPLY FOG
    result = mix(result, vec3(0.6, 0.8, 1.0), dist);
    result = clamp(result, 0.0, 1.0);

    float   alpha = 0.8 + (specular.r / 2);

    FragColor = vec4(result, alpha);
}

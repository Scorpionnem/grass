#version 330 core

out vec4 FragColor;
in vec3 FragPos;
in vec3 Normal;

float   fogDistance = 900;

uniform vec3 viewPos;

void main()
{
	vec3 color = vec3(0.2, 0.1, 1.0);
    float   dist = length(FragPos - viewPos);

    if (dist > fogDistance + 10)
        discard ;

    dist = clamp(dist / fogDistance, 0.0, 1.0);



    vec3    lightDirection = vec3(-0.8f, -0.4f, -0.45f);
    vec3    lightAmbient = vec3(0.2f, 0.2f, 0.2f);
    vec3    lightDiffuse = vec3(0.5f, 0.5f, 0.5f);
    vec3    lightSpecular = vec3(1.0f, 1.0f, 1.0f);
    
    float   shininess = 64.0f;
    float   actualShiness = 2;

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



    result = mix(result, vec3(0.6, 0.8, 1.0), dist);
    result = clamp(result, 0.0, 1.0);
    FragColor = vec4(result, 0.65 + specular);
}

#version 330 core

out vec4 FragColor;

in vec3	Normal;
in vec3	FragPos;

uniform vec3    viewPos;

uniform float RENDER_DISTANCE;
float   fogDistance = RENDER_DISTANCE - (RENDER_DISTANCE / 3.5);

uniform sampler2D   grass_texture;
uniform sampler2D   stone_texture;
uniform sampler2D   snow_texture;

uniform bool    getDepth;
uniform float   time;

in vec4    worldPos;

void main()
{
    // bool    isUnderwater = false;
    // bool    isCameraUnderwater = false;
    // if (worldPos.y < water_level + (fbm(worldPos.xz * scale + (time / animSpeed)) * amplitude))
    //     isUnderwater = true;

    // if (viewPos.y < water_level + (fbm(viewPos.xz * scale + (time / animSpeed)) * amplitude))
    //     isCameraUnderwater = true;

	vec3 color = Normal;
    float   dist = length(FragPos - viewPos);

    if (dist > fogDistance + 10)
        discard ;

    // if (isCameraUnderwater && !isUnderwater)
    //     dist = clamp(dist / (fogDistance / 2), 0.0, 1.0);
    // else
    float   distFarPlane = clamp(dist / RENDER_DISTANCE, 0.0, 1.0);
    dist = clamp(dist / fogDistance, 0.0, 1.0);

    float steepness = 1.0f - dot(Normal, vec3(0, 1, 0));

    float   shininess = 128.0f;
    float   actualShiness = 0.1;

    if (steepness < 0.06)
    {
        color = texture(grass_texture, FragPos.xz).rgb;
        shininess = 16.0f;
        actualShiness = 0.05;
    }
    else
    {
        color = texture(stone_texture, FragPos.xz).rgb;
        actualShiness = 0.1;
    }



    vec3    lightDirection = vec3(-0.8f, -0.4f, -0.45f);
    vec3    lightAmbient = vec3(0.2f, 0.2f, 0.2f);
    vec3    lightDiffuse = vec3(0.5f, 0.5f, 0.5f);
    vec3    lightSpecular = vec3(1.0f, 1.0f, 1.0f);

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



    FragColor = vec4(vec3(0), 1);
    result = mix(result, vec3(0.6, 0.8, 1.0), dist);
    if (getDepth)
        FragColor.rgb = vec3(distFarPlane);
    else
        FragColor = vec4(result, 1.0);
}

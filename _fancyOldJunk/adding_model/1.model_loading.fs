#version 330 core
out vec4 FragColor;
out vec4 Color;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in mat3 TBN; // TBN matrix passed from vertex shader

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_roughness1;

uniform vec3 viewPos;

struct SpotLight{
    vec3 position;// no longer necessary when using directional lights.
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float cutOff;
    float outerCutOff;
    
    float constant;
    float linear;
    float quadratic;
    
};
uniform SpotLight spotLight;

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D roughness;
    
    float shininess;
};
uniform Material material;

vec3 CalcSpotLight(SpotLight light,vec3 normal,vec3 fragPos,vec3 viewDir)
{
    
    vec3 lightDir=normalize(light.position-fragPos);
    // diffuse shading
    float diff=max(dot(normal,lightDir),0.);
    // specular shading
    vec3 reflectDir=reflect(-lightDir,normal);



    
    float spec=pow(max(dot(viewDir,reflectDir),0.),material.shininess);
    // attenuation
    float distance=length(light.position-fragPos);
    float attenuation=1./(light.constant+light.linear*distance+light.quadratic*(distance*distance));
    // spotlight intensity
    float theta=dot(lightDir,normalize(-light.direction));
    float epsilon=light.cutOff-light.outerCutOff;
    float intensity=clamp((theta-light.outerCutOff)/epsilon,0.,1.);
    // combine results
    vec3 ambient=light.ambient*vec3(texture(texture_diffuse1,TexCoords));
    vec3 diffuse=light.diffuse*diff*vec3(texture(texture_diffuse1,TexCoords));
    vec3 specular=light.specular*spec*vec3(texture(texture_specular1,TexCoords));
    vec3 roughness = light.specular*spec*vec3(texture(texture_roughness1,TexCoords));

    ambient*=attenuation*intensity;
    diffuse*=attenuation*intensity;
    specular*=attenuation*intensity;
    roughness*=roughness*intensity;


    return(ambient+diffuse+specular+roughness);
}

void main()
{
    // Sample normal map and transform to world space
    vec3 tangentNormal = texture(texture_normal1, TexCoords).rgb;
    tangentNormal = normalize(tangentNormal * 2.0 - 1.0); // convert from [0,1] to [-1,1]
    vec3 normal = normalize(TBN * tangentNormal); // transform to world space



    vec3 viewDir=normalize(viewPos-FragPos);
    vec3 result=CalcSpotLight(spotLight,normal,FragPos,viewDir);
    
    //FragColor= texture(texture_diffuse1,TexCoords);
    FragColor=vec4(result,1.);
    
}
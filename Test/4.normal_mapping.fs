#version 330 core
out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

in vec3 Normal;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D texture_normal1;

uniform vec3 lightPos;
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
    
};uniform SpotLight spotLight;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; uniform Material material;

uniform sampler2D rock_color;

vec3 colorObj = vec3(0.67, 0.29, 0.69);


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
    vec3 ambient=light.ambient*material.ambient;//vec3(texture(texture_diffuse1,fs_in.TexCoords));
    vec3 diffuse=light.diffuse*diff*material.diffuse;//vec3(texture(texture_diffuse1,fs_in.TexCoords));
    vec3 specular=light.specular*spec*material.specular;//vec3(texture(texture_specular1,fs_in.TexCoords));

    ambient*=attenuation*intensity;
    diffuse*=attenuation*intensity;
    specular*=attenuation*intensity;


    return(ambient+diffuse+specular);
}

void main()
{           
     // obtain normal from normal map in range [0,1]
    vec3 normal = texture(texture_normal1, fs_in.TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
   
    // get diffuse color
    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    // ambient
    vec3 ambient = 0.1 * color;
    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;

    vec3 result=CalcSpotLight(spotLight,Normal,fs_in.FragPos,viewDir);

    // FragColor = vec4(result, 1.0);
    //     float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    // FragColor = vec4(average,average,average, 1.0);
    
    if( length(result)> 0.95)
        FragColor = vec4(colorObj * length(result),1);
    else if (length(result)> 0.55)
        FragColor = vec4(.55 * colorObj * length(result),1);
    else if (length(result)> 0.25)
        FragColor = vec4(.25 * colorObj *length(result),1);
    else
        FragColor = vec4(.05 * colorObj *length(result),1);
    //FragColor = texture(rock_color,fs_in.TexCoords);
}
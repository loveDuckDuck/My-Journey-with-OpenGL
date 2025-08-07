#version 330 core
out vec4 FragColor;

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



uniform sampler2D metal_normal;
uniform sampler2D metal_diff;
uniform sampler2D metal_spec;



vec3 CalcSpotLight(SpotLight light,vec3 normal,vec3 fragPos,vec3 viewDir)
{
    
    vec3 lightDir=normalize(light.position-fragPos);
    // diffuse shading
    float diff=max(dot(normal,lightDir),0.);
    // specular shading
    vec3 reflectDir=reflect(-lightDir,normal);
    float spec=pow(max(dot(viewDir,reflectDir),0.),10);
    // attenuation
    float distance=length(light.position-fragPos);
    float attenuation=1./(light.constant+light.linear*distance+light.quadratic*(distance*distance));
    // spotlight intensity
    float theta=dot(lightDir,normalize(-light.direction));
    float epsilon=light.cutOff-light.outerCutOff;
    float intensity=clamp((theta-light.outerCutOff)/epsilon,0.,1.);
    // combine results
    vec3 ambient=light.ambient*vec3(texture(metal_diff,fs_in.TexCoords));//vec3(texture(texture_diffuse1,fs_in.TexCoords));
    vec3 diffuse=light.diffuse*diff*vec3(texture(metal_diff,fs_in.TexCoords));//vec3(texture(texture_diffuse1,fs_in.TexCoords));
    vec3 specular=light.specular*spec*vec3(texture(metal_spec,fs_in.TexCoords));//vec3(texture(texture_specular1,fs_in.TexCoords));

    ambient*=attenuation*intensity;
    diffuse*=attenuation*intensity;
    specular*=attenuation*intensity;


    return(ambient+diffuse+specular);
}

void main()
{    

    // obtain normal from normal map in range [0,1]
    vec3 normal = texture(metal_normal, fs_in.TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    vec3 result = CalcSpotLight(spotLight,normal,fs_in.FragPos,viewDir);
    FragColor = vec4(result,1);
}

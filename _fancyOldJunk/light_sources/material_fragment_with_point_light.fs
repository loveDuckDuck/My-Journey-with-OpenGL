

#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    float     shininess;
}; 
uniform Material material;



struct Light {
    vec3 position; // no longer necessary when using directional lights.
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
uniform Light light; 

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
uniform vec3 viewPos;

void main()
{
    
    
    // diffuse
    vec3 norm=normalize(Normal);
    vec3 lightDir= normalize(-light.direction);;
    float diff=max(dot(norm,lightDir),0.);
    
    // specular
    vec3 viewDir=normalize(viewPos-FragPos);
    vec3 reflectDir=reflect(-lightDir,norm);
    float spec=pow(max(dot(viewDir,reflectDir),0.),material.shininess);
    

    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));  
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
    		    light.quadratic * (distance * distance)); 


    ambient  *= attenuation; 
    diffuse  *= attenuation;
    specular *= attenuation; 


    vec3 result=ambient+diffuse+specular;
    FragColor=vec4(result,1.);
}
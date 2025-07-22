#version 330 core
layout(location=0)in vec3 aPos;
layout(location=1)in vec3 aNormal;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 LightingColor;// resulting color from lighting calculations

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    
    gl_Position=projection*view*model*vec4(aPos,1.);
    
    vec3 Normal=mat3(transpose(inverse(model)))*aNormal;
    vec3 Position=vec3(model*vec4(aPos,1.));
    
    // ambient color
    float ambientStrength=.3;
    vec3 ambient=ambientStrength*lightColor;
    
    //diffuse colore
    vec3 norm=normalize(Normal);
    vec3 lightDir=normalize(lightPos-Position);
    float diff=max(dot(norm,lightDir),0.);
    vec3 diffuse=diff*lightColor;
    
    //specular colors
    float specularStrength=1.f;
    
    vec3 viewDir=normalize(viewPos-Position);
    vec3 reflectDir=reflect(-lightDir,norm);
    float spec=pow(max(dot(viewDir,reflectDir),0.),32);//shines value
    vec3 specular=specularStrength*spec*lightColor;
    
    LightingColor=ambient+diffuse+specular;
}
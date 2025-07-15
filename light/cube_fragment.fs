

#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;  
uniform vec3 viewPos;

void main()
{
    // here calculate the ambient, so the light that tecnichanil refectled
    // along the ambient
    float ambientStrength=.3;
    float specularStrength = 0.5;
    vec3 ambient=ambientStrength*lightColor;
    
    // here define the diffuse light, given bu the dot product between
    // the vector of the incidence light and the normal of the surface
    // if the angle is 90 degree we got 0 color, more the vector of the light is
    // perpendicular to the surface, more light we got
    // diffuse equal to the color of the light and the diffuse calculated
    vec3 norm=normalize(Normal);
    vec3 lightDir=normalize(lightPos-FragPos);
    // If the angle between both vectors is greater 
    // than 90 degrees then the result of the dot product
    // will actually become negative and we end up with 
    // a negative diffuse component. For that reason we use the max
    // function that returns the highest of both its parameters to make 
    // sure the diffuse component (and thus the colors) 
    // never become negative. Lighting for negative colors is not really defined so it's best to stay away from that,
    // unless you're one of those eccentric artists. 


    float diff= max (dot(norm,lightDir),0.);
    vec3 diffuse=diff*lightColor;


    // so the calcualtion of the specular light is quite so
    // we need to think that we can use the view direction that is this case 
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);//shines value
    vec3 specular = specularStrength * spec * lightColor;  


    vec3 result=(ambient+diffuse + specular)*objectColor;

    FragColor=vec4(result,1.);
    
}


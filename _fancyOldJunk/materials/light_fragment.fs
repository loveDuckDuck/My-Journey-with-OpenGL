

#version 330 core
out vec4 FragColor;
uniform vec3 aColor;


void main()
{
    FragColor = vec4(aColor,1.0); // set all 4 vector values to 1.0
}


#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{   // normal color
    //FragColor = texture(texture1, TexCoords);

    FragColor = vec4(vec3(texture(texture1, TexCoords)), 1.0);
}
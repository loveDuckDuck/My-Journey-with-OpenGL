#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D texture1;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_roughness1;


void main()
{             
    vec4 texColor = texture(texture1, TexCoords);

    FragColor = vec4(texColor);
}
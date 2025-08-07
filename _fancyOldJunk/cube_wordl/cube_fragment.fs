#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

//in vec4 vertexColor; // input variable from vs (same name and type)
uniform sampler2D texture1;
uniform sampler2D texture2;
void main()
{
    FragColor=mix(texture(texture1,TexCoord),
    texture(texture2,TexCoord),0.2);
}

#version 330 core
out vec4 FragColor;
in vec3  ourColorVertex;
uniform vec4 ourColor; // we set this variable in the OpenGL code.

//in vec4 vertexColor; // input variable from vs (same name and type)


void main()
{
    FragColor = vec4(ourColorVertex,1.0f);
}

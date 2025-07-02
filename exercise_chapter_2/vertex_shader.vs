#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor; // color has attribute position 1
out vec4 vertexColor; // specify a color output to the fragment shader

out vec3 ourColorVertex; // output a color to the fragment shader
void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // output variable to dark-red
    ourColorVertex = aColor; // set ourColor to input color from the vertex data
}

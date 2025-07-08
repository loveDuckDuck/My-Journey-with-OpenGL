#version 330 core
layout(location=0)in vec3 aPos;
layout(location=1)in vec3 aColor;// color has attribute position 1
layout(location=2)in vec2 aTexCoord;

uniform vec4 offsetTriangle;// we set this variable in the OpenGL code.

out vec4 vertexColor;// specify a color output to the fragment shader
out vec4 vertexPosition;
out vec3 ourColorVertex;// output a color to the fragment shader
out vec3 ourPositionVertex;// output a color to the fragment shader

out vec2 TexCoord;

uniform mat4 transform;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position=projection * view * model * (vec4(aPos, 1.0) * transform);
    ourColorVertex=aColor;// set ourColor to input color from the vertex data
    TexCoord=aTexCoord;
}


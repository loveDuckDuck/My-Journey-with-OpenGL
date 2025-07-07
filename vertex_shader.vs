#version 330 core
layout(location=0)in vec3 aPos;
layout(location=1)in vec3 aColor;// color has attribute position 1
out vec3 ourColorVertex;// output a color to the fragment shader


void main()
{
    gl_Position = vec4(aPos.x,aPos.y,aPos.z,1.0f);//inverted this to make an upsidedown triangle
    ourColorVertex= aColor;// set ourColor to input color from the vertex data
}


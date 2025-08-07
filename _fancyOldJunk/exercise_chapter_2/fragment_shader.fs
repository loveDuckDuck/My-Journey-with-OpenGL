#version 330 core
out vec4 FragColor;
out vec4 FragPosition;

in vec3 ourColorVertex;
in vec3 ourPositionVertex;

uniform vec4 ourColor;// we set this variable in the OpenGL code.
uniform vec4 ourPosit;// we set this variable in the OpenGL code.

//in vec4 vertexColor; // input variable from vs (same name and type)

void main()
{
    FragColor=vec4(ourColorVertex,1.f);
    /*why is the bottom-left side of our triangle black? because
    Think about this for a second: the output of our fragment's color is equal to the (interpolated) coordinate of
    the triangle. What is the coordinate of the bottom-left point of our triangle? This is (-0.5f, -0.5f, 0.0f). Since the
    xy values are negative they are clamped to a value of 0.0f. This happens all the way to the center sides of the
    triangle since from that point on the values will be interpolated positively again. Values of 0.0f are of course black
    and that explains the black side of the triangle.
    */
}

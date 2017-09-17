#version 120
attribute vec4 vertex;
attribute vec4 colour;

varying vec4 v_color;

uniform mat4 proj;
uniform vec4 trans;

void main()
{

    v_color = colour;  

     gl_Position = proj * (vertex + trans);
}


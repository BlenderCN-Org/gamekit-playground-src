#version 120

precision mediump float;
varying vec2 v_texCoord;
varying vec4 v_color;
uniform sampler2D s_texture;
uniform float u_alpha;

void main()
{
   gl_FragColor = v_color;
}
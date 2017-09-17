#version 120

varying vec2 v_texCoord;
varying vec4 v_color;
uniform sampler2D s_texture;
uniform float u_alpha;

void main()
{
   gl_FragColor = texture2D( s_texture, v_texCoord ) * v_color;
}
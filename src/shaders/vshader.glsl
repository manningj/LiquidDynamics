#version 150

in vec4 vPosition;
uniform float Time; // in milliseconds
uniform mat4 ModelView, Projection;
out vec4 f_colour;
uniform vec4 colour;
void main()
{
  vec4  v = vPosition;
  f_colour = colour;
  gl_Position =  v;
}




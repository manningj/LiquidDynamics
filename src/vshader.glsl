#version 150

in vec4 vPosition;
uniform float Time; // in milliseconds
uniform mat4 ModelView, Projection;
out vec4 f_colour;

void main()
{
  gl_Position = vPosition;
  f_colour = vec4(0.2,1.0,0.5,1);
}




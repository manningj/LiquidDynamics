#version 150

in vec4 f_colour;
out vec4 out_colour;

uniform vec2 NewForce;

void main() 
{
  // Just added some color
  out_colour = vec4(NewForce.x*5, NewForce.y*5, 0.5, 1);
  //out_colour = f_colour;
}

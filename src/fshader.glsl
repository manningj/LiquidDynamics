#version 150

in vec4 f_colour;
out vec4 out_colour;

uniform sampler2D Sampler;
uniform vec3 FillColor;
uniform vec2 Scale;

void main() 
{

  //float L = texture(Sampler, gl_FragCoord.xy * Scale).r;
  //out_colour = vec4(FillColor, L);
  out_colour = f_colour;
}

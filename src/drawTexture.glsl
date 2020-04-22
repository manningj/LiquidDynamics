#version 150

in vec4 f_colour;
out vec4 out_colour;

uniform sampler2D Sampler;
uniform vec3 FillColor;
uniform vec2 Scale;

void main() 
{
 //vec3 L = FillColor;
  vec3 L = texture(Sampler, gl_FragCoord.xy * Scale).rgb;
  out_colour = vec4(L, 1);
  //out_colour = f_colour;
}

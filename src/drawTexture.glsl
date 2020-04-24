#version 150

in vec4 f_colour;
out vec4 out_colour;

uniform sampler2D Sampler;
uniform vec3 FillColor;
uniform vec2 Scale; // 1/fieldWith 1/fieldHeight

void main() 
{
 //vec3 L = FillColor;
  vec3 curr = texture(Sampler, gl_FragCoord.xy * Scale).rgb;
  out_colour = vec4(curr, 1);
  //out_colour = f_colour;

    // if (gl_FragCoord.x < 1 || gl_FragCoord.y <1) {
  //   out_colour = vec4(1.0,0.0,0.0,1.0);
  // } else if (gl_FragCoord.x > 639 || gl_FragCoord.y > 636) { 
  //   out_colour = vec4(1.0,0.0,0.0,1.0);
  // }
}

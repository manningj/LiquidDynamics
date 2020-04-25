#version 150

out vec4 out_colour;

uniform sampler2D Sampler;
uniform vec2 Scale; // 1/fieldWith, 1/fieldHeight

void main() 
{
  // Get texture at current fragment and display it
  vec3 curr = texture(Sampler, gl_FragCoord.xy * Scale).rgb;
  out_colour = vec4(curr, 1);
}

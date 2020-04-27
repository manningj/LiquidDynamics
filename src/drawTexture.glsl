#version 150

out vec4 out_colour;

uniform sampler2D Sampler;
uniform vec2 Scale; // 1/fieldWith, 1/fieldHeight
uniform bool Velocity; 
uniform float VelocityDisplayScale; 

void main() 
{
  // Get texture at current fragment and display it
  vec3 curr = texture(Sampler, gl_FragCoord.xy * Scale).rgb;
  out_colour = vec4(curr, 1);

  if (Velocity) {
    out_colour = vec4(0.5 + curr/VelocityDisplayScale, 1);
  } else {
    out_colour = vec4(curr, 1);
  }
}

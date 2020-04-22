#version 150

in vec4 f_colour;
out vec4 out_colour;

uniform vec2 NewForce; // F.xy
uniform float dt;
uniform float ImpulseRadius; // r
uniform float ImpulsePosition; .// xp and yp
uniform Scale; // 1/fieldWith 1/fieldHeight -> Used to get texture for fragment

// Equation for c (added velocity from forces):
//        
//   c = F * dt ^ (((x-xp)^2 + (y-yp)^2)/r)


void main() 
{
  // Grab the current velocity
  vec3 curr = texture(Sampler, gl_FragCoord*windowScale).rg;

  // Calculate the new velocity caused from forces
  float exp = pow(gl_FragCoord.x - ImpulsePosition.x,2) - pow(gl_FragCoord.y - ImpulsePosition.y,2)/InpulseRadius;
  vec3 added = vec3(newForce,0) * pow(timestep, exp);
  
  // Write new velocity to framebuffer
  out_colour = vec4(curr+added, 1);
  //out_colour = f_colour;
}

#version 150
out vec4 out_colour;

uniform sampler2D Sampler; // Curr velocity
uniform vec2 NewForce; // F.xy
uniform float TimeStep; // dt
uniform float ImpulseRadius; // r
uniform vec2 ImpulsePosition; // xp and yp
uniform vec2 Scale; // 1/fieldWith, 1/fieldHeight -> Used to get texture for fragment

// Equation for c (added velocity from forces):
//        
//   c = F * dt ^ (((x-xp)^2 + (y-yp)^2)/r)

void main() 
{
  // check if we're a boundary, if so, just get to zero velocity
  if ((gl_FragCoord.x > 638 || gl_FragCoord.x < 1) || (gl_FragCoord.y > 638 || gl_FragCoord.y < 1)) { 
    out_colour = vec4(0.5,0.5,0.5, 1);
  } else {
    // Grab the current velocity
    vec2 curr = texture(Sampler, gl_FragCoord.xy*Scale).rg;

    // Calculate the new velocity caused from forces
    float exp = (pow(gl_FragCoord.x - ImpulsePosition.x,2) + pow(gl_FragCoord.y - ImpulsePosition.y,2))/ImpulseRadius;
    vec2 added = NewForce * pow(TimeStep, exp);

    // Write new velocity to framebuffer
    out_colour = vec4(curr + added, 0.5, 1);
  }
  
}

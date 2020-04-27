#version 150
out vec4 out_colour;

uniform sampler2D Sampler; // Curr velocity
uniform vec2 NewForce; // F.xy
uniform float dt; // dt
uniform float ImpulseRadius; // r
uniform vec2 ImpulsePosition; // xp and yp
uniform vec2 Scale; // 1/fieldWith, 1/fieldHeight -> Used to get texture for fragment
// To find if we are interior or are a boundary
uniform vec2 InteriorRangeMin;
uniform vec2 InteriorRangeMax;

// Equation for c (added velocity from forces):
//        
//   c = F * dt ^ (((x-xp)^2 + (y-yp)^2)/r)

void main() 
{
  // check if we're a boundary, if so, just get to zero velocity
  if ((gl_FragCoord.x > InteriorRangeMax.x || gl_FragCoord.x < InteriorRangeMin.x) || (gl_FragCoord.y > InteriorRangeMax.y || gl_FragCoord.y < InteriorRangeMin.y)) { 
    out_colour = vec4(0.0,0.0,0.0,1.0);
  } else {
    // Grab the current velocity
    vec2 curr = texture(Sampler, gl_FragCoord.xy*Scale).rg;

    // Calculate the new velocity caused from forces using equation from GPU Gems
    float exp = (pow(gl_FragCoord.x - ImpulsePosition.x,2) + pow(gl_FragCoord.y - ImpulsePosition.y,2))/ImpulseRadius;
    vec2 added = NewForce * pow(dt, exp);

    // Write new velocity to framebuffer
    out_colour = vec4(curr + added, 0.0, 1);
  }
  
}

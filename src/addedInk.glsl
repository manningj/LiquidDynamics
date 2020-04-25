#version 150

in vec4 f_colour;
out vec4 out_colour;

uniform sampler2D Sampler; // Curr velocity
uniform vec3 NewInk; // F.xy
//uniform float TimeStep; // dt
uniform float InkRadius; // r
uniform float InkStrength; 
uniform vec2 InkPosition;

uniform vec2 Scale; // 1/fieldWith 1/fieldHeight -> Used to get texture for fragment

// Equation for c (added velocity from forces):
//        
//   c = F * dt ^ (((x-xp)^2 + (y-yp)^2)/r)

void main() 
{
  // Grab the current velocity
   if ((gl_FragCoord.x > 638 || gl_FragCoord.x < 1) || (gl_FragCoord.y > 638 || gl_FragCoord.y < 1)) { 
    out_colour = vec4(0.0,0.0,0.0, 1);
  } else {
      
    vec3 curr = texture(Sampler, gl_FragCoord.xy*Scale).rgb;
    vec3 added = vec3(0,0,0);
    
    
    // Calculate the new velocity caused from forces
    // float exp = (pow(gl_FragCoord.x - ImpulsePosition.x,2) + pow(gl_FragCoord.y - ImpulsePosition.y,2))/ImpulseRadius;
    float inkDistance = distance(gl_FragCoord.xy,InkPosition);
    if (inkDistance < InkRadius) {
      added = InkStrength * (InkRadius - inkDistance)/InkRadius * NewInk;
    }

    // Write new velocity to framebuffer
    out_colour = vec4(curr + added, 1);
    //out_colour = vec4(0.0, 0.0, 0.0, 1);
  }
}
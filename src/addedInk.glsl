#version 150

out vec4 out_colour;

uniform sampler2D Sampler; // Curr ink
uniform vec3 NewInk; // output ink
uniform float InkRadius; // r
uniform float InkStrength; // Stength of the color
uniform vec2 InkPosition; // Where the ink dropped
uniform vec2 Scale; // 1/fieldWith, 1/fieldHeight -> Used to get texture for fragment
// To find if we are interior or are a boundary
uniform vec2 InteriorRangeMin;
uniform vec2 InteriorRangeMax;

void main() 
{
  // Check if we're a boundary, if so, just get to zero color
  if ((gl_FragCoord.x > InteriorRangeMax.x || gl_FragCoord.x < InteriorRangeMin.x) || (gl_FragCoord.y > InteriorRangeMax.y || gl_FragCoord.y < InteriorRangeMin.y)) { 
    out_colour = vec4(0.0,0.0,0.0,1.0);
  } else {
    // Grab the current velocity
    vec3 curr = texture(Sampler, gl_FragCoord.xy*Scale).rgb;
    vec3 added = vec3(0,0,0);
    
    // Calculate the new ink for fragment caused from added ink
    float inkDistance = distance(gl_FragCoord.xy,InkPosition);
    if (inkDistance < InkRadius) {
      added = InkStrength * (InkRadius - inkDistance)/InkRadius * NewInk;
    }

    // Write new ink to framebuffer
    out_colour = vec4(curr + added, 1);
  }
}
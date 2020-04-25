#version 150

out vec4 out_colour;

uniform sampler2D Sampler; // Curr ink
uniform vec3 NewInk; // output ink
uniform float InkRadius; // r
uniform float InkStrength; // Stength of the color
uniform vec2 InkPosition; // Where the ink dropped
uniform vec2 Scale; // 1/fieldWith, 1/fieldHeight -> Used to get texture for fragment

void main() 
{
  // check if we're a boundary, if so, just get to zero color
  if ((gl_FragCoord.x > 638 || gl_FragCoord.x < 1) || (gl_FragCoord.y > 638 || gl_FragCoord.y < 1)) { 
    out_colour = vec4(0.0,0.0,0.0, 1);
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
#version 150

out vec4 newX; //the advected position

uniform vec2 Scale; // {1/field width, 1/field height}
uniform float timeStep; //dt
uniform float dissipation; //diffusion value
uniform bool isInk;
uniform sampler2D veloTex; //velocity texture
uniform sampler2D posTex; // position texture.
// To find if we are interior or are a boundary
uniform vec2 InteriorRangeMin;
uniform vec2 InteriorRangeMax;

// formula is q(x,t + dt) = q(x - u(x,t)dt,t)

/*
To sample the color of a texture we use GLSL's built-in texture function
 that takes as its first argument a texture sampler 
 and as its second argument the corresponding texture coordinates. 
The texture function then samples the corresponding color value 
using the texture parameters we set earlier. 
    -- from a website explaing how texture() works
*/
void main() 
{
  // check if we're a boundary, if so, just get to zero velocity/ink
  if ((gl_FragCoord.x > InteriorRangeMax.x || gl_FragCoord.x < InteriorRangeMin.x) || (gl_FragCoord.y > InteriorRangeMax.y || gl_FragCoord.y < InteriorRangeMin.y)) { 
    if (isInk) {
      newX = vec4(0.0,0.0,0.0, 1);
    } else {
      newX = vec4(0.5,0.5,0.5, 1);
    }
  } else {
    vec2 fragCoord = gl_FragCoord.xy; // gets the window coord of the fragment

    //this gets the samples the value of velo tex at the window coordinates of the fragment.
    // u = the velocity of this fragment(or cell)
    vec2 u = ((texture(veloTex, Scale * fragCoord).xy) - 0.5) * 640.0f;; 
    //go to previous position.
    
    vec2 pos = (fragCoord - timeStep * u )* Scale;
    //this gets the new advectedd pos.

    vec3 newU = texture(posTex, pos).xyz;
    newX = vec4(dissipation *newU.xyz, 1.0);
  }
}


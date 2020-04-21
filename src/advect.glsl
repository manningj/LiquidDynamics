#version 150

out vec4 newX; //the advected position

uniform vex2 rdx; // {1/width, 1/height}
uniform float timeStep; //dt
uniform sampler2D veloTex; //velocity texture
uniform sampler2D posTex;// position texture.
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
  vec2 fragCoord = gl_FragCoord.xy; // gets the window coord of the fragment
  
  //this gets the samples the value of velo tex at the window coordinates of the fragment.
  // u = the velocity of this fragment(or cell)
  vec2 u = (texture(veloTex, rdx * fragCoord).xy); 
  //go to previous position.
  //prev pos is calculated using the 
  vec2 prevPos =  rdx * (fragCoord - timeStep * u);

//this gets the new advectd pos.
  newX = texture(posTex, prevPos); 
}


#version 150

in vec4 f_colour;
out vec4 out_colour;

//uniform sampler2D Sampler;
uniform vec2 Offset;
//uniform vec2 Scale; 
// uniform bool isVelocity;

void main() {
  // if (isVelocity) { // Velocity
  //   // If velocity, need to make sure that neighboring velocity + boundary velocity = 0
  //   // So, set boundary velocity to -neighboring velocity value.
  //   // Need to take into consideration we use 0.5 as no velocity
  //   vec2 neighboring = texelFetchOffset(Sampler, fragCoord, 0, offset).rg - 0.5;
  //   out_colour = (-neighboring) + 0.5;
  // } else { // Pressure
  //   // If pressure, need to make sure that neighboring pressure - boundary pressure = 0
  //   // So, set boundary pressure to neighboring pressure value.
  //   out_colour = vec4(texture(Sampler, fragCoord.xy * Scale).rgb, 1);
  // }

  //out_colour = vec4(1.0,0.0,0.0,1.0);


  out_colour = vec4(Offset,1.0,1.0);


  //vec2 tex = texture(Sampler, gl_FragCoord.xy*(1.0/(639.0))).rg;

  //if (gl_FragCoord.x == 0 || gl_FragCoord.y == 0) {
  //  out_colour = vec4(1.0,1.0,1.0,1.0);
  //}


  //vec2 tex = vec2(1.0,1.0); //texture(Sampler, gl_FragCoord.xy*Scale).rg;
  //vec2 tex = texture(Sampler, gl_FragCoord.xy*Scale).rg;

  //if (tex.x == 0 || tex.y == 0) {
    
  // } else if (tex.x == 0.5 || tex.y == 0.5) {
  //   out_colour = vec4(0.0,1.0,0.0,1.0);
  // } else {
  //   out_colour = vec4(0.0,0.0,1.0,1.0);
  // }

 // out_colour = vec4(texture(Sampler, gl_FragCoord.xy*Scale).rg,0.0,1.0);
 // out_colour = vec4(texture(Sampler, gl_FragCoord.xy*Scale).rg,0.0,1.0);
}
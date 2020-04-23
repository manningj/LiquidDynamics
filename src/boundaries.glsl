#version 150

in vec4 f_colour;
out vec4 out_colour;

uniform sampler2D Sampler;
uniform ivec2 Offset;
uniform vec2 Scale; 
uniform bool isVelocity;

void main() {
  if (isVelocity) { // Velocity
    // If velocity, need to make sure that neighboring velocity + boundary velocity = 0
    // So, set boundary velocity to -neighboring velocity value.
    // Need to take into consideration we use 0.5 as no velocity
    vec2 neighboring = texelFetchOffset(Sampler, fragCoord, 0, offset).rg - 0.5;
    out_colour = (-neighboring) + 0.5;
  } else { // Pressure
    // If pressure, need to make sure that neighboring pressure - boundary pressure = 0
    // So, set boundary pressure to neighboring pressure value.
    out_colour = vec4(texture(Sampler, fragCoord.xy * Scale).rgb, 1);
  }
}
#version 150

out vec2 fragOut;

uniform sampler2D pressure;
uniform sampler2D velocity;

uniform float gradScale; // 0.5/ gridscale

void main(){

    ivec2 fragCoord = ivec2 (gl_FragCoord.xy);

    //we need to get the neighbouring pressure values.

    vec4 pT = texelFetchOffset(pressure, fragCoord, 0, ivec2(0, 1)).r;
    vec4 pB = texelFetchOffset(pressure, fragCoord, 0, ivec2(0, -1)).r;
    vec4 pR = texelFetchOffset(pressure, fragCoord, 0, ivec2(1, 0)).r;
    vec4 pL = texelFetchOffset(pressure, fragCoord, 0, ivec2(-1, 0)).r;

    //index vector for masking velocity
    vec2 veloMask = vec2(1.0);

    //free slip boundary condition
    vec2 prevVelo = texelFetch(velocity, fragCoord, 0).xy;
    vec2 grad = vec2(pL - pR, pT - pB) * gradScale;
    vec2 newVelo = prevVelo - grad;


    fragOut = (veloMask * newVelo);

}
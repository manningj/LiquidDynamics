#version 150

out float fragDivergence;

uniform sampler2D velocity;
uniform float halfrdx; // half of the reciprocal of dx

void main(){

    vec2 fragCoords = gl_FragCoord.xy;


    vec4 vT = texelFetchOffset(velocity, fragCoord, 0, ivec2(0, 1)).r;
    vec4 vB = texelFetchOffset(velocity, fragCoord, 0, ivec2(0, -1)).r;
    vec4 vR = texelFetchOffset(velocity, fragCoord, 0, ivec2(1, 0)).r;
    vec4 vL = texelFetchOffset(velocity, fragCoord, 0, ivec2(-1, 0)).r;

    fragDivergence = halfrdx * (vR.x - vL.x + vT.y - vB.y);
}
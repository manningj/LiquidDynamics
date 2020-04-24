#version 150

out float fragDivergence;

uniform sampler2D velocity;
uniform float halfrdx; // half of the reciprocal of dx

void main(){

    ivec2 fragCoord = ivec2(gl_FragCoord.xy);


    vec2 vT = texelFetchOffset(velocity, fragCoord, 0, ivec2(0, 1)).xy;
    vec2 vB = texelFetchOffset(velocity, fragCoord, 0, ivec2(0, -1)).xy;
    vec2 vR = texelFetchOffset(velocity, fragCoord, 0, ivec2(1, 0)).xy;
    vec2 vL = texelFetchOffset(velocity, fragCoord, 0, ivec2(-1, 0)).xy;


    fragDivergence = halfrdx * ((vR.x - vL.x) + (vT.y - vB.y));
}
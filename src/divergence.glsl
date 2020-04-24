#version 150

out float fragDivergence;

uniform sampler2D velocity;
uniform sampler2D boundaryTex;

uniform float halfrdx; // half of the reciprocal of dx
void main(){

    ivec2 fragCoord = ivec2(gl_FragCoord.xy);


    vec2 vT = texelFetchOffset(velocity, fragCoord, 0, ivec2(0, 1)).xy;
    vec2 vB = texelFetchOffset(velocity, fragCoord, 0, ivec2(0, -1)).xy;
    vec2 vR = texelFetchOffset(velocity, fragCoord, 0, ivec2(1, 0)).xy;
    vec2 vL = texelFetchOffset(velocity, fragCoord, 0, ivec2(-1, 0)).xy;

    vec4 boundaryTop = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(0, 1));
    vec4 boundaryBot = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(0, -1));
    vec4 boundaryRight = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(1, 0));
    vec4 boundaryLeft = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(-1, 0));

    if(boundaryTop.z == 1.0){vT = vec2(0.5f);}
    if(boundaryBot.z == 1.0){vB = vec2(0.5f);}
    if(boundaryRight.z == 1.0){vR = vec2(0.5f);}
    if(boundaryLeft.z == 1.0){vL = vec2(0.5f);}


    fragDivergence = halfrdx * ((vR.x - vL.x) + (vT.y - vB.y));
}
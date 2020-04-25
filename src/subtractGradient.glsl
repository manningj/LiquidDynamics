#version 150

out vec4 fragOut;

uniform sampler2D velocity;
uniform sampler2D pressure;
uniform sampler2D boundaryTex;

uniform float gradScale; // 0.5 / gridscale

void main(){

    ivec2 fragCoord = ivec2 (gl_FragCoord.xy);

    vec3 bC  = texelFetch(boundaryTex, fragCoord, 0).xyz;

    if(bC.z < 0){
        fragOut = vec4(0.5f, 0.5f, 0.5f, 1.0f);
        return;
    }

    //we need to get the neighbouring pressure values.

    float pT = texelFetchOffset(pressure, fragCoord, 0, ivec2(0, 1)).r;
    float pB = texelFetchOffset(pressure, fragCoord, 0, ivec2(0, -1)).r;
    float pR = texelFetchOffset(pressure, fragCoord, 0, ivec2(1, 0)).r;
    float pL = texelFetchOffset(pressure, fragCoord, 0, ivec2(-1, 0)).r;
    
    float pC = texelFetch(pressure, fragCoord, 0).r;

    vec4 boundaryTop = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(0, 1));
    vec4 boundaryBot = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(0, -1));
    vec4 boundaryRight = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(1, 0));
    vec4 boundaryLeft = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(-1, 0));


    if(boundaryTop.z == 1.0){ pT = pC;}
    if(boundaryBot.z == 1.0){ pB = pC;}
    if(boundaryRight.z == 1.0){ pR = pC;}
    if(boundaryLeft.z == 1.0){ pL = pC;}


    //free slip boundary condition
    vec2 prevVelo = texelFetch(velocity, fragCoord, 0).xy;
    vec2 grad = vec2(pL - pR, pT - pB) * gradScale;
    
    vec2 newVelo = prevVelo - grad;

    fragOut = vec4(newVelo.x,newVelo.y, 0.5, 1.0);

}
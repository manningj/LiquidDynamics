#version 150

out vec4 fragOut;

uniform sampler2D velocity;
uniform sampler2D pressure;
uniform sampler2D boundaryTex;

uniform float gradScale; // 0.5 / gridscale

void main(){

    ivec2 fragCoord = ivec2 (gl_FragCoord.xy);

    //we need to get the neighbouring pressure values.

    float pT = texelFetchOffset(pressure, fragCoord, 0, ivec2(0, 1)).r;
    float pB = texelFetchOffset(pressure, fragCoord, 0, ivec2(0, -1)).r;
    float pR = texelFetchOffset(pressure, fragCoord, 0, ivec2(1, 0)).r;
    float pL = texelFetchOffset(pressure, fragCoord, 0, ivec2(-1, 0)).r;

    //vec3 boundaryCoord(boundaryTex, fragCoord);
    vec3 boundaryCoord = texelFetch(boundaryTex, fragCoord, 0).rgb;

    if(boundaryCoord.z == 1.0){
        
        ivec2 offsets = ivec2(((boundaryCoord.xy) *2) -1);
        fragCoord = fragCoord + offsets;

        vec2 pC = texelFetch(pressure, fragCoord, 0).xy;

        fragOut = vec4(pC, 0.5, 1.0); // If this is pressure, do we need y? If velocity, this should do it though
    }


    //free slip boundary condition
    vec2 prevVelo = texelFetch(velocity, fragCoord, 0).xy;
    vec2 grad = vec2(pL - pR, pT - pB) * gradScale;
    
    vec2 newVelo = prevVelo - grad;

    fragOut = vec4(newVelo.x,newVelo.y, 0.5, 1.0);

}
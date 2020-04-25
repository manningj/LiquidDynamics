#version 150

out vec4 uNew; // output fragment for velocity

uniform sampler2D velocity;
uniform sampler2D pressure;
uniform sampler2D boundaryTex;

uniform float gradScale; // 0.5 / gridscale

bool isBoundary(float x, float y);

void main(){

    ivec2 fragCoord = ivec2 (gl_FragCoord.xy);

    if (!isBoundary(fragCoord.x, fragCoord.y)) {

        //we need to get the neighbouring pressure values.

        float pT = texelFetchOffset(pressure, fragCoord, 0, ivec2(0, 1)).r;
        float pB = texelFetchOffset(pressure, fragCoord, 0, ivec2(0, -1)).r;
        float pR = texelFetchOffset(pressure, fragCoord, 0, ivec2(1, 0)).r;
        float pL = texelFetchOffset(pressure, fragCoord, 0, ivec2(-1, 0)).r;
        
        float pC = texelFetch(pressure, fragCoord, 0).r;

        // vec4 boundaryTop = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(0, 1));
        // vec4 boundaryBot = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(0, -1));
        // vec4 boundaryRight = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(1, 0));
        // vec4 boundaryLeft = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(-1, 0));


        // if(boundaryTop.z == 1.0){ pT = pC;}
        // if(boundaryBot.z == 1.0){ pB = pC;}
        // if(boundaryRight.z == 1.0){ pR = pC;}
        // if(boundaryLeft.z == 1.0){ pL = pC;}


        if(isBoundary(fragCoord.x, fragCoord.y+1.0)) { pT = pC; }
        if(isBoundary(fragCoord.x, fragCoord.y-1.0)) { pB = pC; }
        if(isBoundary(fragCoord.x+1.0, fragCoord.y)) { pR = pC; }
        if(isBoundary(fragCoord.x-1.0, fragCoord.y)) { pL = pC; }

        vec2 prevVelo = texelFetch(velocity, fragCoord, 0).xy;

        // Previous velocity minus the gradient
        vec2 newVelo = prevVelo - vec2(pL - pR, pT - pB) * gradScale;

        uNew = vec4(newVelo.x,newVelo.y, 0.5, 1.0);
    } else {
        uNew = vec4(0.5,0.5,0.5,1.0);
    }

}

bool isBoundary(float x, float y) {
    if ((gl_FragCoord.x > 638 || gl_FragCoord.x < 1) || (gl_FragCoord.y > 638 || gl_FragCoord.y < 1)) { 
        return true;
    }
    return false;
}
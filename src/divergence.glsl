#version 150

out vec4 fragDivergence;

uniform sampler2D velocity;
uniform sampler2D boundaryTex;

uniform float halfrdx; // half of the reciprocal of dx

bool isBoundary(float x, float y);

void main(){

    ivec2 fragCoord = ivec2(gl_FragCoord.xy);

    if (!isBoundary(fragCoord.x, fragCoord.y)) {

        vec2 vT = texelFetchOffset(velocity, fragCoord, 0, ivec2(0, 1)).xy * 10.0f;
        vec2 vB = texelFetchOffset(velocity, fragCoord, 0, ivec2(0, -1)).xy * 10.0f;
        vec2 vR = texelFetchOffset(velocity, fragCoord, 0, ivec2(1, 0)).xy * 10.0f;
        vec2 vL = texelFetchOffset(velocity, fragCoord, 0, ivec2(-1, 0)).xy * 10.0f;

        vec4 center = texelFetch(velocity, fragCoord, 0);
        vec4 centerV = (center * 2.0f) -1.0f; //convert to (-1, 1)

        centerV = (centerV * -1.0); // invert if velocity 
        centerV = (centerV + 1.0f)/2.0f; // convert back to texture range (0,1)
        
        center = centerV * 10.0f;

        if(isBoundary(fragCoord.x, fragCoord.y+1.0)) { vT = vec2(center.xy); }
        if(isBoundary(fragCoord.x, fragCoord.y-1.0)) { vB = vec2(center.xy); }
        if(isBoundary(fragCoord.x+1.0, fragCoord.y)) { vR = vec2(center.xy); }
        if(isBoundary(fragCoord.x-1.0, fragCoord.y)) { vL = vec2(center.xy); }

    // vec4 boundaryTop = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(0, 1));
    // vec4 boundaryBot = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(0, -1));
    // vec4 boundaryRight = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(1, 0));
    // vec4 boundaryLeft = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(-1, 0));

    // if(boundaryTop.z == 1.0){vT = vec2(0.5f);}
    // if(boundaryBot.z == 1.0){vB = vec2(0.5f);}
    // if(boundaryRight.z == 1.0){vR = vec2(0.5f);}
    // if(boundaryLeft.z == 1.0){vL = vec2(0.5f);}
    
        float red = halfrdx * ((vR.x - vL.x) + (vT.y - vB.y));
        //float red = halfrdx;

        fragDivergence = vec4(red,0.0, 0.0, 1.0);
        //fragDivergence = vec4(0.8,0.0, 0.0, 1.0);
    } else {
        fragDivergence = vec4(0.0,0.0,0.0,1.0);
    }
}


bool isBoundary(float x, float y) {
    if ((gl_FragCoord.x > 638 || gl_FragCoord.x < 1) || (gl_FragCoord.y > 638 || gl_FragCoord.y < 1)) { 
        return true;
    }
    return false;
}
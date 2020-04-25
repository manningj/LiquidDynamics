#version 150

out vec4 fragDivergence;

uniform sampler2D velocity;
uniform float halfrdx; // half of the reciprocal of dx
// To find if we are interior or are a boundary
uniform vec2 InteriorRangeMin;
uniform vec2 InteriorRangeMax;

bool isBoundary(float x, float y);

void main(){

    ivec2 fragCoord = ivec2(gl_FragCoord.xy);

    // Check if current fragment is a boundary
    if (!isBoundary(fragCoord.x, fragCoord.y)) {
        // Find neighboring velocities
        vec2 vT = texelFetchOffset(velocity, fragCoord, 0, ivec2(0, 1)).xy * 10.0f;
        vec2 vB = texelFetchOffset(velocity, fragCoord, 0, ivec2(0, -1)).xy * 10.0f;
        vec2 vR = texelFetchOffset(velocity, fragCoord, 0, ivec2(1, 0)).xy * 10.0f;
        vec2 vL = texelFetchOffset(velocity, fragCoord, 0, ivec2(-1, 0)).xy * 10.0f;

        // Calculate inverse velocity in case one of the neighbors is a boundary
        vec4 center = texelFetch(velocity, fragCoord, 0);
        vec4 centerV = (center * 2.0f) -1.0f; // convert to (-1, 1)

        centerV = (centerV * -1.0); // invert if velocity 
        centerV = (centerV + 1.0f)/2.0f; // convert back to texture range (0,1)
        
        center = centerV * 10.0f;

        // Check if boundary, if so set to inverse center
        if(isBoundary(fragCoord.x, fragCoord.y+1.0)) { 
            vT = vec2(center.xy); 
        }
        if(isBoundary(fragCoord.x, fragCoord.y-1.0)) {
            vB = vec2(center.xy); 
        }
        if(isBoundary(fragCoord.x+1.0, fragCoord.y)) {
            vR = vec2(center.xy); 
        }
        if(isBoundary(fragCoord.x-1.0, fragCoord.y)) {
            vL = vec2(center.xy); 
        }
    
        float red = halfrdx * ((vR.x - vL.x) + (vT.y - vB.y));
        fragDivergence = vec4(red,0.0, 0.0, 1.0);
    } else {
        fragDivergence = vec4(0.0,0.0,0.0,1.0);
    }
}


bool isBoundary(float x, float y) {
    if ((gl_FragCoord.x > InteriorRangeMax.x || gl_FragCoord.x < InteriorRangeMin.x) || (gl_FragCoord.y > InteriorRangeMax.y || gl_FragCoord.y < InteriorRangeMin.y)) { 
        return true;
    }
    return false;
}
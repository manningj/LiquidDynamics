#version 150

out vec4 uNew; // Output fragment for velocity

uniform sampler2D velocity;
uniform sampler2D pressure;

uniform float gradScale; // 0.5 / gridscale according to GPU Gems
// To find if we are interior or are a boundary
uniform vec2 InteriorRangeMin;
uniform vec2 InteriorRangeMax;

bool isBoundary(float x, float y);

void main(){

    ivec2 fragCoord = ivec2 (gl_FragCoord.xy);

    // Check if current fragment is a boundary
    if (!isBoundary(fragCoord.x, fragCoord.y)) {

        // We need to get the neighbouring pressure values.
        float pT = texelFetchOffset(pressure, fragCoord, 0, ivec2(0, 1)).r;
        float pB = texelFetchOffset(pressure, fragCoord, 0, ivec2(0, -1)).r;
        float pR = texelFetchOffset(pressure, fragCoord, 0, ivec2(1, 0)).r;
        float pL = texelFetchOffset(pressure, fragCoord, 0, ivec2(-1, 0)).r;
        
        // Get current fragment's pressure
        float pC = texelFetch(pressure, fragCoord, 0).r;

        // If a neighbor is a boundary, set to the current fragment's pressure
        if(isBoundary(fragCoord.x, fragCoord.y+1.0)) {
            pT = pC; 
        }
        if(isBoundary(fragCoord.x, fragCoord.y-1.0)) { 
            pB = pC; 
        }
        if(isBoundary(fragCoord.x+1.0, fragCoord.y)) { 
            pR = pC; 
        }
        if(isBoundary(fragCoord.x-1.0, fragCoord.y)) {
             pL = pC; 
        }

        // Get the velocity for the fragment, which we'll alter by the gradient
        vec2 prevVelo = texelFetch(velocity, fragCoord, 0).xy;
        // Previous velocity minus the gradient
        vec2 newVelo = prevVelo - vec2(pL - pR, pT - pB) * gradScale;
        // Set the new velocity
        uNew = vec4(newVelo.x,newVelo.y, 0.5, 1.0);
    } else { // Boundary, set zero velocity
        uNew = vec4(0.5,0.5,0.5,1.0);
    }
}

bool isBoundary(float x, float y) {
    if ((gl_FragCoord.x > InteriorRangeMax.x || gl_FragCoord.x < InteriorRangeMin.x) || (gl_FragCoord.y > InteriorRangeMax.y || gl_FragCoord.y < InteriorRangeMin.y)) { 
        return true;
    }
    return false;
}
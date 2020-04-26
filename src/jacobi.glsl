#version 150

out vec4 xNew; //result

uniform float alpha; 
uniform float rBeta;
uniform bool Velocity; // Are we dealing with velocity? If false, it's pressure
// To find if we are interior or are a boundary
uniform vec2 InteriorRangeMin;
uniform vec2 InteriorRangeMax;

uniform sampler2D x; // x vector, Ax = b
uniform sampler2D b; // b vector, Ax = b

// for pressure, alpha = -(dx^2)
//              rbeta = 1/4
//              x = pressure
//              b = divergence
// for diffusion, alpha = dx^2 /v*dt 
//              rbeta = 1/(4 + dx^2 /v*dt)

//              x = velocity
//              b = velocity

bool isBoundary(float x, float y);

void main(){
    
    ivec2 fragCoord = ivec2(gl_FragCoord.xy);

    // Check if current fragment is a boundary
    if (!isBoundary(fragCoord.x, fragCoord.y)) {

        // Get left, right, bottom, top samples.
        vec4 top = texelFetchOffset(x, fragCoord, 0, ivec2(0, 1));
        vec4 bot = texelFetchOffset(x, fragCoord, 0, ivec2(0, -1));
        vec4 right = texelFetchOffset(x, fragCoord, 0, ivec2(1, 0));
        vec4 left = texelFetchOffset(x, fragCoord, 0, ivec2(-1, 0));
    
        vec4 center = texelFetch(x, fragCoord, 0);
        
        // If any of the surrounding cells are a boundary, we want to use the center for velocity/pressure.
        // Negative center velocity for velocity, positive center pressure for pressure.
        if(Velocity){
            vec4 centerV = vec4(-center.rgb, 1.0);  // Set to -u
        }

        // Check if boundary, if so set to center (or inverse center if velocity)
        if(isBoundary(fragCoord.x, fragCoord.y + 1.0)) {
            top = center; 
        }
        if(isBoundary(fragCoord.x, fragCoord.y - 1.0)) { 
            bot = center; }
        if(isBoundary(fragCoord.x + 1.0, fragCoord.y)) {
            right = center; 
        }
        if(isBoundary(fragCoord.x - 1.0, fragCoord.y)) {
            left = center; 
        }

        // Get center of b sample
        vec4 bC = texelFetch(b, fragCoord, 0);
        // if (Velocity) {
            xNew = ((top + bot + right + left + (alpha * bC)) * rBeta );
        // } else {
        //     // Just use x coordinates with pressure
        //     // This is separate just to make sure no other fields are altered
        //     float outX = (top.x + bot.x + right.x  + left.x + (alpha * bC.x) * rBeta);
            
        //     xNew = vec4(outX, 0.0, 0.0, 1.0);
        //     // if (outX>1 || xNew <-1) {
        //     //     xNew = vec4(1.0, 1.0, 1.0, 1.0);
        //     // }
        // }
    } else { // Boundary, set to zero velocity/pressure
        if(Velocity){
            xNew = vec4(0.0,0.0,0.,1.0);
        } else {
            xNew = vec4(0.0,0.0,0.0,1.0);
        }
    }
 

} // Jacobi main

// Checks if boundary based on passed in interior range
bool isBoundary(float x, float y) {
    if ((gl_FragCoord.x > InteriorRangeMax.x || gl_FragCoord.x < InteriorRangeMin.x) || (gl_FragCoord.y > InteriorRangeMax.y || gl_FragCoord.y < InteriorRangeMin.y)) { 
        return true;
    }
    return false;
}
#version 150

out vec4 xNew; //result

uniform float alpha; 
uniform float rBeta;
uniform bool Velocity;

uniform sampler2D x;// x vector, Ax = b
uniform sampler2D b;// b vector, Ax = b
uniform sampler2D boundaryTex;

//for pressure, alpha = -(dx^2)
//              rbeta = 1/4
//              x = pressure
//              b = divergence
//for diffusion, alpha = dx^2 /v*dt 
//              rbeta = 1/(4 + dx^2 /v*dt)

//              x = velocity
//              b = velocity


bool isBoundary(float x, float y);

void main(){
    
    ivec2 fragCoord = ivec2(gl_FragCoord.xy);

    if (!isBoundary(fragCoord.x, fragCoord.y)) {

        //get left, right, bottom, top samples.

        vec4 top = texelFetchOffset(x, fragCoord, 0, ivec2(0, 1));
        vec4 bot = texelFetchOffset(x, fragCoord, 0, ivec2(0, -1));
        vec4 right = texelFetchOffset(x, fragCoord, 0, ivec2(1, 0));
        vec4 left = texelFetchOffset(x, fragCoord, 0, ivec2(-1, 0));
    
        vec4 center = texelFetch(x, fragCoord, 0);
        
        //if any of the surrounding cells are an obstacle, we want to use the center for velocity/pressure.
        //negative center velocity for velocity, positive center pressure for pressure.
    
        if(Velocity){
            vec4 centerV = (center * 2.0f) -1.0f; //convert to (-1, 1)

            centerV = (centerV * -1.0); // invert if velocity 
            centerV = (centerV + 1.0f)/2.0f; // convert back to texture range (0,1)
            
            center = centerV;
        }

        if(isBoundary(fragCoord.x, fragCoord.y+1.0)) { top = center; }
        if(isBoundary(fragCoord.x, fragCoord.y-1.0)) { bot = center; }
        if(isBoundary(fragCoord.x+1.0, fragCoord.y)) { right = center; }
        if(isBoundary(fragCoord.x-1.0, fragCoord.y)) { left = center; }
        

        //BAD STUFF HERE

        // //get surrounding boundary cells
        // vec4 boundaryTop = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(0, 1));
        // vec4 boundaryBot = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(0, -1));
        // vec4 boundaryRight = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(1, 0));
        // vec4 boundaryLeft = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(-1, 0));

        // //if any of the surrounding cells are an obstacle, we want to use the center for velocity/pressure.
        // //negative center velocity for velocity, positive center pressure for pressure.
        
        // if(boundaryTop.z == 1.0){top = center;}
        // if(boundaryBot.z == 1.0){bot = center;}
        // if(boundaryRight.z == 1.0){right = center;}
        // if(boundaryLeft.z == 1.0){left = center;}

        // BAD STUFF OVER

        //get center of b sample
        vec4 bC = texelFetch(b, fragCoord, 0);

        // vec2 temp = (top.xy + bot.xy + right.xy + left.xy + (alpha * bC.xy)) * rBeta ;

        // xNew = vec4(temp.xy, 0.5, 1.0);
        if (Velocity) {
            xNew = ((top + bot + right + left + (alpha * bC)) * rBeta );

        } else {
            // With pressure, need to make proper to velocity?
            float outX = (top.x + bot.x + right.x  + left.x + (alpha * bC.x) * rBeta);
            
            // if ( bC.x >0.0) {
            //      xNew = vec4(1.0, 0.0, 0.0, 1.0);
            //  } else {
            //      xNew = vec4(0.0, 1.0, 0.0, 1.0);
            //  }
            xNew = vec4(outX, 0.0, 0.0, 1.0);
        }
        
        //xNew = alpha*bC;;
    } else {
        if(Velocity){
            xNew = vec4(0.5,0.5,0.5,1.0);
        } else {
            xNew = vec4(0.0,0.0,0.0,1.0);
        }
    }
 

}//end jacobi main

bool isBoundary(float x, float y) {
    if ((gl_FragCoord.x > 638 || gl_FragCoord.x < 1) || (gl_FragCoord.y > 638 || gl_FragCoord.y < 1)) { 
        return true;
    }
    return false;
}
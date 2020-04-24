#version 150

out vec4 xNew; //result

uniform float alpha; 
uniform float rBeta;
uniform int Scale;

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

void main(){
    
    ivec2 fragCoord = ivec2(gl_FragCoord.xy);

    //get left, right, bottom, top samples.

    vec4 top = texelFetchOffset(x, fragCoord, 0, ivec2(0, 1));
    vec4 bot = texelFetchOffset(x, fragCoord, 0, ivec2(0, -1));
    vec4 right = texelFetchOffset(x, fragCoord, 0, ivec2(1, 0));
    vec4 left = texelFetchOffset(x, fragCoord, 0, ivec2(-1, 0));
   
    vec4 center = texelFetch(x, fragCoord, 0);
    
    //if any of the surrounding cells are an obstacle, we want to use the center for velocity/pressure.
    //negative center velocity for velocity, positive center pressure for pressure.
   
    
    vec4 centerV = (center * 2.0f) -1.0f; //convert to (-1, 1)
    
    centerV = (centerV * Scale); //invert if velocity 
    centerV = (centerV + 1.0f)/2.0f; // convert back to texture range (0,1)
    
    center = centerV;
    

    //get surrounding boundary cells
    vec4 boundaryTop = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(0, 1));
    vec4 boundaryBot = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(0, -1));
    vec4 boundaryRight = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(1, 0));
    vec4 boundaryLeft = texelFetchOffset(boundaryTex, fragCoord, 0, ivec2(-1, 0));

    //if any of the surrounding cells are an obstacle, we want to use the center for velocity/pressure.
    //negative center velocity for velocity, positive center pressure for pressure.
    if(boundaryTop.z == 1.0){top = center;}
    if(boundaryBot.z == 1.0){bot = center;}
    if(boundaryRight.z == 1.0){right = center;}
    if(boundaryLeft.z == 1.0){left = center;}

    //get center of b sample
    vec4 bC = texelFetch(b, fragCoord, 0);

   // vec2 temp = (top.xy + bot.xy + right.xy + left.xy + (alpha * bC.xy)) * rBeta ;

   // xNew = vec4(temp.xy, 0.5, 1.0);
  xNew = ((top + bot + right + left + (alpha * bC)) * rBeta );
 

}//end jacobi main
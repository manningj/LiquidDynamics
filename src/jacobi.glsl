#version 150

out vec4 xNew; //result

uniform float alpha; 
uniform float rBeta;

uniform sampler2D x;// x vector, Ax = b
uniform sampler2D b;// b vector, Ax = b


//for pressure, alpha = -(dx^2)
//              rbeta = 1/4
//              x = pressure
//              b = divergence
//for diffusion, alpha = dx^2 /v*dt 
//              rbeta = 1/(4 + dx^2 /v*dt)
//              x = velocity
//              d = velocity

void main(){
    ivec2 fragCoord = ivec2(gl_FragCoord.xy);
    //get left, right, bottom, top samples.
    vec4 top = texelFetchOffset(x, fragCoord, 0, ivec2(0, 1));
    vec4 bot = texelFetchOffset(x, fragCoord, 0, ivec2(0, -1));
    vec4 right = texelFetchOffset(x, fragCoord, 0, ivec2(1, 0));
    vec4 left = texelFetchOffset(x, fragCoord, 0, ivec2(-1, 0));

    //get center of b sample
    vec4 bC = texelFetch(b, fragCoord, 0);

    xNew = (top + bot + right + left + alpha * bC) * rBeta;

}//end jacobi main
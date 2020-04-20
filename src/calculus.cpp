#include "headers/calculus.hpp"

glm::vec2 gradient(CellField cellField, int i, int j, int dx, int dy){
    //returns a vector of size 2 (3 if 3d). takes in a scalar field, 
    //and a reference to a cell in that scalar field, in form of i j.
    
    //calculations for gradient are as follows:

    /* p[i+1][j] - p[i-1][j]        p[i][j+1] - p[i][j-1]
    -------------------------- ,-------------------------- , 
               2*  dx                        2*  dy    
    */
    float p_over_dx = (cellField.at(i+1).at(j).getPressure() 
                    - cellField.at(i-1).at(j).getPressure()) / (2*dx);

    float p_over_dy = (cellField.at(i).at(j+1).getPressure() 
                    - cellField.at(i).at(j-1).getPressure()) / (2*dy);
    return glm::vec2(p_over_dx, p_over_dy);
}
float divergence(CellField cellField, float v, int i, int j, int dx, int dy){
    //the rate at which density exits a giver region of space.
    //applied to the velocity of the flow,
    //and measure the net change in velocity 
    //across a surface surrounding a small piece of fluid

  /* u[i+1][j] - u[i-1][j]        v[i][j+1] - v[i][j-1]
-------------------------- + -------------------------- where u is hte velocity and v is 
          2* dx                        2*  dy           viscocity (assumed constant 4 now)
    */
    float velocity_over_dx = (cellField.at(i+1).at(j).getVelocity() 
                            - cellField.at(i-1).at(j).getVelocity()) / (2*dx);

    float viscocity_over_dy = (v - v) / (2*dy);
    float divergence = velocity_over_dx + viscocity_over_dy;

    divergence = 0;
    return divergence;
}

void laplacian(CellField cellField, int i, int j, int dx){

}
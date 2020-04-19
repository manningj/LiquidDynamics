#include "headers/calculus.hpp"

//glm::vec2 gradient(const CellField cellField, int i, int j, int dx, int dy){
    //returns a vector of size 2 (3 if 3d). takes in a scalar field, 
    //and a reference to a cell in that scalar field, in form of i j.
    //we need to decide if we are doing a cell object or not.
    //the big issue is that we have two kinds of vector fields, scalar and vector  
    //Im leaning towards creating a field class of cell objects, 
    //where cells have both velocity (u) and pressure (p). 
    //then, we can add more things to the cells like colour pretty easily.

    //calculations for gradient are as follows:

    /* p[i+1][j] - p[i-1][j]        p[i][j+1] - p[i][j-1]
    -------------------------- ,-------------------------- , 
                dx                          dy    
    */
	
//}
void divergence(){

}

void laplacian(){

}
#include "headers/common.h"

Cell::Cell(float newPressure, glm::vec2 newVelocity) : 
        pressure{newPressure}, velocity{newVelocity}
{
}

Cell::~Cell()
{
}

float Cell::getPressure(){
    return pressure;
}
glm::vec2 Cell::getVelocity(){
    return velocity;
}

void Cell::setPressure(float newPressure){
    pressure = newPressure;
}
void Cell::setVelocity(glm::vec2 newVelocity){
    velocity = newVelocity;
}





#ifndef CELL
#define CELL
#include "common.h"

class Cell
{
private:
    /* data */
    float pressure;
    glm::vec2 velocity; //velocity, u in the pdf
  
public:
    Cell(float newPressure, glm::vec2 newVelocity);
    ~Cell();
    
    float getPressure();
    glm::vec2 getVelocity();

    void setPressure(float newPressure);
    void setVelocity(glm::vec2 newVelocity);
};

#endif

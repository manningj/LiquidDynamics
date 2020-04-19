//author: John Manning

#ifndef NAVIER_STOKES
#define NAVIER_STOKES
//navier stokes methods definitions.

//these will not be void 4 long
extern void advection();
extern void diffusion();
extern void addForces();

extern void computePressure();
extern void subtractPressureGradient();








#endif

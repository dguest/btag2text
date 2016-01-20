#ifndef MATH_HH
#define MATH_HH

// first - second, on the range -pi to pi
double phi_mpi_pi(double, double);

// recover the original d0 sign from the lifetime signed version
double d0_from_signed_d0_dphi(double signed_d0, double dphi);

#endif

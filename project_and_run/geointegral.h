const size_t Order = 6 ;
 
inline void epsilons(double cos_alpha0, double *e)
{
	const double ep = sqrt(eccentricity2nd2);
	double k = ep * cos_alpha0;
	double sqrtk1 = hypot(1, k) + 1;
	e[0] = k * k / (sqrtk1 * sqrtk1); 
	e[1] = e[0] * e[0]; 
	e[2] = e[1] * e[0]; 
	e[3] = e[2] * e[0]; 
	e[4] = e[3] * e[0]; 
	e[5] = e[4] * e[0]; 
}
inline double calcB( 
	double C1, 
	double C2, 
	double C3, 
	double C4, 
	double C5, 
	double C6, 
	double cos2x, double sin2x, bool truncate = false)
{
	cos2x *= 2;
 
	double b6 = C6; 
	double b5 = C5 + b6 * cos2x; 
	double b4 = C4 + b5 * cos2x - b6; 
	double b3 = C3 + b4 * cos2x - b5; 
	double b2 = C2 + b3 * cos2x - b4; 
	return !truncate
		? (C1 + b2 * cos2x - b3) * sin2x : b2 * sin2x;
}
 
/* ----------------------------- I1 -----------------------------*/ 
inline double A1(double e2, double e1)
{
	return (e2*(e2*(e2+4)+64)+256)/(256*(1-e1));
} 
inline double C11(double eps2, double e1)
{
	return e1 * ((6-eps2)*eps2-16)/32;
} 
inline double C12(double eps2, double e2)
{
	return e2 * ((64-9*eps2)*eps2-128)/2048;
} 
inline double C13(double eps2, double e3)
{
	return e3 * (9*eps2-16)/768;
} 
inline double C14(double eps2, double e4)
{
	return e4 * (3*eps2-5)/512;
} 
inline double C15(double eps2, double e5)
{
	return e5 * -7/1280;
} 
inline double C16(double eps2, double e6)
{
	return e6 * -7/2048;
} 
inline double calcI1(const double *e, double sigma, double cos2x, double sin2x)
{
	double e2 = e[1];
	return A1(e2, e[0]) * (sigma + calcB( 
			C11(e2, e[0]), 
			C12(e2, e[1]), 
			C13(e2, e[2]), 
			C14(e2, e[3]), 
			C15(e2, e[4]), 
			C16(e2, e[5]), 
			cos2x, sin2x));
}
inline double calcI1diff(const double *e,
	double sigma2, double cos2x2, double sin2x2,
	double sigma1, double cos2x1, double sin2x1)
{
	double e2 = e[1]; 
	double C1 = C11(e2, e[0]); 
	double C2 = C12(e2, e[1]); 
	double C3 = C13(e2, e[2]); 
	double C4 = C14(e2, e[3]); 
	double C5 = C15(e2, e[4]); 
	double C6 = C16(e2, e[5]); 
	return A1(e2, e[0]) * (  (sigma2 + calcB( 
				C1, 
				C2, 
				C3, 
				C4, 
				C5, 
				C6, 
				cos2x2, sin2x2))
			- (sigma1 + calcB( 
				C1, 
				C2, 
				C3, 
				C4, 
				C5, 
				C6, 
				cos2x1, sin2x1)));
}
 
/* ------------------------- I1 reversion -----------------------*/ 
inline double C11prime(double eps2, double e1)
{
	return e1 * (eps2*(205*eps2-432)+768)/1536;
} 
inline double C12prime(double eps2, double e2)
{
	return e2 * (eps2*(4005*eps2-4736)+3840)/12288;
} 
inline double C13prime(double eps2, double e3)
{
	return e3 * (116-225*eps2)/384;
} 
inline double C14prime(double eps2, double e4)
{
	return e4 * (2695-7173*eps2)/7680;
} 
inline double C15prime(double eps2, double e5)
{
	return e5 * 3467/7680;
} 
inline double C16prime(double eps2, double e6)
{
	return e6 * 38081/61440;
} 
double calcRevertI1(const double *e, double s)
{
	double e2 = e[1];
	double tau = s / (radius_b * A1(e2, e[0]));
	return tau + calcB( 
		C11prime(e2, e[0]), 
		C12prime(e2, e[1]), 
		C13prime(e2, e[2]), 
		C14prime(e2, e[3]), 
		C15prime(e2, e[4]), 
		C16prime(e2, e[5]), 
		cos(2*tau), sin(2*tau));
}
 
/* ----------------------------- I2 -----------------------------*/ 
inline double A2(double e2, double e1)
{
	return ((e2*(e2*(25*e2+36)+64)+256)/256)*(1-e1);
} 
inline double C21(double eps2, double e1)
{
	return e1 * (eps2*(eps2+2)+16)/32;
} 
inline double C22(double eps2, double e2)
{
	return e2 * (eps2*(35*eps2+64)+384)/2048;
} 
inline double C23(double eps2, double e3)
{
	return e3 * (15*eps2+80)/768;
} 
inline double C24(double eps2, double e4)
{
	return e4 * (7*eps2+35)/512;
} 
inline double C25(double eps2, double e5)
{
	return e5 * 63/1280;
} 
inline double C26(double eps2, double e6)
{
	return e6 * 77/2048;
} 
inline double calcI2(const double *e, double sigma, double cos2x, double sin2x)
{
	double e2 = e[1];
	return A2(e2, e[0]) * (sigma + calcB( 
			C21(e2, e[0]), 
			C22(e2, e[1]), 
			C23(e2, e[2]), 
			C24(e2, e[3]), 
			C25(e2, e[4]), 
			C26(e2, e[5]), 
			cos2x, sin2x));
}
inline double calcI2diff(const double *e,
	double sigma2, double cos2x2, double sin2x2,
	double sigma1, double cos2x1, double sin2x1)
{
	double e2 = e[1]; 
	double C1= C21(e2, e[0]); 
	double C2= C22(e2, e[1]); 
	double C3= C23(e2, e[2]); 
	double C4= C24(e2, e[3]); 
	double C5= C25(e2, e[4]); 
	double C6= C26(e2, e[5]); 
	return A2(e2, e[0]) * (  (sigma2 + calcB( 
				C1, 
				C2, 
				C3, 
				C4, 
				C5, 
				C6, 
				cos2x2, sin2x2))
			- (sigma1 + calcB( 
				C1, 
				C2, 
				C3, 
				C4, 
				C5, 
				C6, 
				cos2x1, sin2x1)));
}
 
/* ----------------------------- I3 -----------------------------*/
const double n1 = flattening3rd; 
const double n2 = n1 * n1; 
const double n3 = n2 * n1; 
const double n4 = n3 * n1; 
const double n5 = n4 * n1; 
const double n6 = n5 * n1; 
const double flat3s[] = { 
	n1, 
	n2, 
	n3, 
	n4, 
	n5, 
	n6, 
}; 
inline double A3(double e1)
{
	return (e1*(e1*(e1*((-8*n1-24)*n1+e1*(-4*n1-3*e1-6)-8)+n1*(48*n1-16)-32)+64*n1-64)+128)/128;
} 
inline double C31(double eps1, double e1)
{
	return e1 * (eps1*(-16*n2+eps1*(eps1*(2*n1+3*eps1+5)+(6-2*n1)*n1+6)+16)-32*n1+32)/128;
} 
inline double C32(double eps1, double e2)
{
	return e2 * (eps1*(eps1*(2*n1+5*eps1+6)+(-12*n1-8)*n1+12)+n1*(8*n1-24)+16)/256;
} 
inline double C33(double eps1, double e3)
{
	return e3 * (n1*(40*n1-72)+eps1*(-40*n1+21*eps1+36)+40)/1536;
} 
inline double C34(double eps1, double e4)
{
	return e4 * (-14*n1+7*eps1+7)/512;
} 
inline double C35(double eps1, double e5)
{
	return e5 * 21/2560;
} 
inline double calcI3(const double *e, double sigma, double cos2x, double sin2x)
{
	double e1 = e[0];
	return A3(e1) * (sigma + calcB(0, 
		C31(e1, e[0]), 
		C32(e1, e[1]), 
		C33(e1, e[2]), 
		C34(e1, e[3]), 
		C35(e1, e[4]), 
		cos2x, sin2x, true));
}
inline double calcI3diff(const double *e,
	double sigma2, double cos2x2, double sin2x2,
	double sigma1, double cos2x1, double sin2x1)
{
	double e1 = e[0];
	double C1 = 0; 
	double C2 = C31(e1, e[0]); 
	double C3 = C32(e1, e[1]); 
	double C4 = C33(e1, e[2]); 
	double C5 = C34(e1, e[3]); 
	double C6 = C35(e1, e[4]); 
	return A3(e1) * (  (sigma2 + calcB( 
				C1, 
				C2, 
				C3, 
				C4, 
				C5, 
				C6, 
				cos2x2, sin2x2, true))
			- (sigma1 + calcB( 
				C1, 
				C2, 
				C3, 
				C4, 
				C5, 
				C6, 
				cos2x1, sin2x1, true)));
}
 

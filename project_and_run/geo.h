#ifndef GEO_H__
#define GEO_H_

#include <math.h>
#include <stdio.h>


#define SQR(x)	((x)*(x))
#define DEG(h,m,s)	(h+m/60.0 + s/3600.0)

//ph 緯度
//la 経度
inline void coordTransform( double ph0, double la0, double x, double y, double& ph, double& la)
{
	const double a = 6378137.0;
	const double f = 1.0/298.257222101;
	const double b = a*(1 - f);
	const double c = SQR(a)/b;
	const double e = sqrt((SQR(a) - SQR(b))/SQR(a));
	const double e_ = sqrt((SQR(a) - SQR(b))/SQR(b));
	const double m0 = 0.9999;

	const double A1= 1.00167851427; const double A7=-0.00000001419;
	const double A2= 0.00251882660; const double A8= 0.00000000002;
	const double A3= 0.00000370095; const double A9= 0.00000000007;
	const double A4= 0.00000845577; const double A10=-0.00000000008;
	const double A5= 0.00000000745; const double A11=-0.00000000002;
	const double A6= 0.00000002485;

	const double A= 1.005052501813087; const double D= 0.000000020820379;
	const double B= 0.005063108622224; const double E= 0.000000000039324;
	const double C= 0.000010627590263; const double F= 0.000000000000071;

	const double S0 = a*(1-SQR(e))*(A*ph0 - B*sin(2*ph0)/2 + C*sin(4*ph0)/4 - D*sin(6*ph0)/6 + E*sin(8*ph0)/8 - F*sin(10*ph0)/10);

	const double M = S0 + x/m0;
	const double th = M/a;

	const double ph1 = (A1*th + A2*sin(2*th) + A3*sin(4*th) + A4*th*cos(2*th) + A5*sin(6*th) 
						+ A6*cos(4*th) + A7*SQR(th)*sin(2*th) + A8*sin(8*th) + A9*th*cos(6*th) 
						+ A10*SQR(th)*sin(4*th) + A11*pow(th,3)*cos(2*th));

	const double it = e_*cos(ph1);
	const double it2 = SQR(it);
	const double M1 = c/sqrt( pow(1 + it2, 3) );
	const double N1 = c/sqrt( 1 + it2 );

	ph = ph1 - ( tan(ph1)/(2*M1*N1) )*SQR(y/m0)
			+ ( tan(ph1)/(24*M1*pow(N1,3)))*(5 + 3*SQR(tan(ph1)) + it2 - 9*it2*SQR(tan(ph1)) - 4*pow(it,4))*pow(y/m0,4)
			- (tan(ph1)/(720*M1*pow(N1,5)))*(61 + 90*SQR(tan(ph1)) + 45*pow(tan(ph1),4))*pow(y/m0, 6);

	la = la0 +(1/(N1*cos(ph1)))*(y/m0) - ((1+2*SQR(tan(ph1)) + it2)/(6*pow(N1,3)*cos(ph1)))*pow(y/m0,3)
		+( (5 + 28*SQR(tan(ph1)) + 24*pow(tan(ph1),4))/(120*pow(N1,5)*cos(ph1)))*pow(y/m0,5);
	
	int bd = (int)(ph);
	float bm = (ph-bd)*60.0;
	int bmin = (int)(bm);
	float bs = (bm - bmin)*60.0;

	int ld = (int)(la);
	float lm = (la-ld)*60.0;
	int lmin = (int)(lm);
	float ls = (lm - lmin)*60.0;

	printf("経度 %d %d %f\n", ld, lmin, ls);
	printf("緯度 %d %d %f\n", bd, bmin, bs);
}


#if 0
//ph 経度
//la 緯度
inline void coordTransform( double ph0, double la0, double x, double y, double& ph, double& la)
{
	const float m0 = 0.9999;

	float l0 = la0;
	float b0 = ph0;

	const float Pi = 3.14159265358979;
	const float lo = 180 / Pi;
	
	b0 = b0 / lo;
	l0 = l0 / lo;

	const float ra = 6378137;
	const float rb = 6356752.314;

	const float e2 = (SQR(ra) - SQR(rb)) / SQR(ra);
	const float e = sqrt(e2);
	const float e12 = (SQR(ra) - SQR(rb)) / SQR(rb);
	const float e1 = sqrt(e12);

	const float fa = ra * (1 - e2);
	const float k1 = 1.005052501813087;
	const float k2 = 5.063108622224e-03;
	const float k3 = .000010627590263;
	const float k4 = 2.0820379e-08;
	const float k5 = 3.9324e-11;
	const float k6 = 7.1e-14;
	
	float d = k1 * b0 - k2 / 2 * sin(2 * b0);
	d = d + k3 / 4 * sin(4 * b0) - k4 / 6 * sin(6 * b0);
	d = d + k5 / 8 * sin(8 * b0) - k6 / 10 * sin(10 * b0);
	d = d * fa + x / m0;
	float p[20], s[20], m[20], w[20];
	for ( int i = 1; i <= 9; i++ )
	{
		if ( i == 1 )p[i] = b0;
		if ( i > 1 ) p[i] = p[i - 1] - (s[i - 1] - d) / m[i - 1];
		s[i] = k1 * p[i] - k2 / 2 * sin(2 * p[i]);
		s[i] = s[i] + k3 / 4 * sin(4 * p[i]) - k4 / 6 * sin(6 * p[i]);
		s[i] = s[i] + k5 / 8 * sin(8 * p[i]) - k6 / 10 * sin(10 * p[i]);
		s[i] = s[i] * fa;
		w[i] = 1 - e2 * SQR(sin(p[i]));
		w[i] = sqrt(w[i]);
		m[i] = fa / pow(w[i],3);
	}
	const float p10 = p[9];
	const float w10 = w[9];
	const float m10 = m[9];

	const float n = ra / w10;
	const float y1 = y / m0;
	const float tp = tan(p10);
	const float het = e1 * cos(p10);
	const float b1 = -SQR(y1) * tp / (2 * m10 * n);
	const float bf2 = pow(y1,4) * tp / (24 * m10 * pow(n, 3));
	const float b2 = bf2 * (5 + 3 * SQR(tp) + SQR(het) - 9 * SQR(tp * het));
	const float bf3 = -pow(y1, 6) * tp / (720 * m10 * pow(n,5));
	const float b3 = bf3 * (61 + 90 * SQR(tp) + 45 * pow(tp,4));
	
	float b = p10 + (b1 + b2 + b3);
	b = b * lo;

	const float l1 = y1 / (n * cos(p10));
	const float lf2 = -pow(y1, 3) / (6 * pow(n, 3) * cos(p10));
	const float l2 = lf2 * (1 + 2 * SQR(tp) + SQR(het));
	const float lf3 = pow(y1, 5) / (120 * pow(n, 5) * cos(p10));
	const float l3 = lf3 * (5 + 28 * SQR(tp) + 24 * pow(tp, 4));
	float l = l0 + (l1 + l2 + l3);
	l = l * lo;

	int bd = (int)(b);
	float bm = (b-bd)*60.0;
	int bmin = (int)(bm);
	float bs = (bm - bmin)*60.0;

	int ld = (int)(l);
	float lm = (l-ld)*60.0;
	int lmin = (int)(lm);
	float ls = (lm - lmin)*60.0;

	la = l;
	ph = b;

	printf("緯度 %d %d %f\n", ld, lmin, ls);
	printf("経度 %d %d %f\n", bd, bmin, bs);
}
#endif

#endif

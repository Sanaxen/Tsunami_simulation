/*
 * Copyright (c) 2013 Naoshi Murata
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#define _USE_MATH_DEFINES 
#include <cfloat>
#include <cmath>
#include <algorithm>
#include "geodesic.h"

using namespace std;

namespace {
const double dblerror   = DBL_MIN;
const double dblepsilon = DBL_EPSILON;
inline bool dbl_equal(double val, double base)
{
	if (val == base) {
		return true;
	} else {
		double absdiff = fabs(val - base);
		if (absdiff > dblerror) {
			return false;
		} else {
			double absbase = fabs(base);
			return absbase <= dblerror
				? true : absdiff <= dblepsilon * absbase;
		}
	}
}

#include "geointegral.h"

inline double sq(double x)
{
	return x * x;
}
inline double cos2x(double cosx, double sinx)
{
	return sq(cosx) - sq(sinx);
}
inline double sin2x(double cosx, double sinx)
{
	return 2 * cosx * sinx;
}

inline double reduced_latitude(double phi, double& cos_beta, double& sin_beta)
{
	cos_beta = cos(phi);
	sin_beta = (1 - flattening) * sin(phi);
	double denom = hypot(cos_beta, sin_beta);
	cos_beta /= denom;
	sin_beta /= denom;
	return sin_beta / cos_beta;
}
inline void alpha0(
	double cos_alpha, double sin_alpha, double cos_beta, double sin_beta,
	double& cos_alpha0, double& sin_alpha0)
{
	cos_alpha0 = hypot(cos_alpha, sin_alpha * sin_beta);
	sin_alpha0 = sin_alpha * cos_beta;
}
inline double sigma(double cos_alpha, double tan_beta,
	double& cos_sigma, double& sin_sigma)
{
	double denom = hypot(tan_beta, cos_alpha);
	sin_sigma = tan_beta / denom;
	cos_sigma = cos_alpha / denom;
	return atan2(sin_sigma, cos_sigma);
}
inline double sigma_to_s(
	const double *e, double sigma, double cos2x, double sin2x)
{
	return radius_b * calcI1(e, sigma, cos2x, sin2x);
}

inline void normalize_coordinate(double& lambda, double& phi)
{
	if (lambda < -M_PI) {
		if (lambda < -M_PI*2) { lambda = -fmod(-lambda, M_PI*2); }
		lambda += M_PI*2;
	} else if (M_PI < lambda) {
		if (lambda >  M_PI*2) { lambda =  fmod( lambda, M_PI*2); }
		lambda -= M_PI*2;
	}

	if (phi < -M_PI_2) {
		if (phi < -M_PI*2) { phi = -fmod(-phi, M_PI*2); }
		phi = (phi < -M_PI_2*3) ? phi + M_PI*2 : -(phi + M_PI);
	} else if (M_PI_2 < phi) {
		if (phi >  M_PI*2) { phi =  fmod( phi, M_PI*2); }
		phi = (phi >  M_PI_2*3) ? phi - M_PI*2 : -(phi - M_PI);
	}
}

}

double solve_direct_problem(
	double lambda1, double phi1, double s12, double alpha1,
	double& lambda2, double& phi2)
{
	struct normalize {
		double& lambda_; double& phi_;
		normalize(double& l, double& p): lambda_(l), phi_(p) {}
		~normalize() { normalize_coordinate(lambda_, phi_); }
	} t(lambda2, phi2);

	normalize_coordinate(lambda1, phi1);

	if (dbl_equal(fabs(phi1), M_PI_2)) {
		// (lambda1, phi1) is one of the poles.
		lambda2 = lambda1 + alpha1;
		double s1 = radius_b * A1(n2, n1) * phi1;
		double sigma2 = calcRevertI1(flat3s, s1 + s12);
		phi2 = atan2(sin(sigma2), (1-flattening)*cos(sigma2));

		double alpha2;
		if (fabs(sigma2) > 2*M_PI) { sigma2 = fmod(sigma2, 2*M_PI); }
		if (-M_PI_2 <= sigma2 && sigma2 <= M_PI_2) {
			alpha2 = phi1 > 0 ? M_PI : 0;
		} else {
			alpha2 = phi1 > 0 ? 0 : M_PI;
			lambda2 += M_PI;
		}
		return alpha2;

	} else if (dbl_equal(phi1, 0) && dbl_equal(fabs(alpha1), M_PI_2)) {
		// s12 is a part of the equator.
		double dl = s12 / radius_a;
		lambda2 = (alpha1 > 0) ? (lambda1 + dl) : (lambda1 - dl);
		phi2 = 0;
		return alpha1;
	}

	double cos_beta1, sin_beta1;
	double tan_beta1 = reduced_latitude(phi1, cos_beta1, sin_beta1);

	double cos_alpha0, sin_alpha0;
	double cos_alpha1 = dbl_equal(alpha1, M_PI_2) ? 0 : cos(alpha1);
	double sin_alpha1 = dbl_equal(alpha1, -M_PI)  ? 0 : sin(alpha1);
	alpha0(cos_alpha1, sin_alpha1, cos_beta1, sin_beta1,
		cos_alpha0, sin_alpha0);

	double cos_sigma1, sin_sigma1;
	double sigma1 = sigma(cos_alpha1, tan_beta1, cos_sigma1, sin_sigma1);

	double e[Order];
	epsilons(cos_alpha0, e);
	double cos2sigma1 = cos2x(cos_sigma1, sin_sigma1);
	double sin2sigma1 = sin2x(cos_sigma1, sin_sigma1);
	double s1 = sigma_to_s(e, sigma1, cos2sigma1, sin2sigma1);
	double omega1 = atan2(sin_alpha0 * sin_sigma1, cos_sigma1);

	double s2 = s1 + s12;
	double sigma2 = calcRevertI1(e, s2);
	double cos_sigma2 = cos(sigma2);
	double sin_sigma2 = sin(sigma2);

	double cos_beta2 = hypot(sin_alpha0, cos_alpha0 * cos_sigma2);
	double sin_beta2 = cos_alpha0 * sin_sigma2;
	double alpha2 = atan2(sin_alpha0, cos_alpha0 * cos_sigma2);
	double omega2 = atan2(sin_alpha0 * sin_sigma2, cos_sigma2);

	phi2 = atan2(sin_beta2, (1 - flattening) * cos_beta2);

	double cos2sigma2 = cos2x(cos_sigma2, sin_sigma2);
	double sin2sigma2 = sin2x(cos_sigma2, sin_sigma2);
	lambda2 = lambda1
		+ (omega2 - omega1) - flattening * sin_alpha0
			* calcI3diff(e,
				sigma2, cos2sigma2, sin2sigma2,
				sigma1, cos2sigma1, sin2sigma1);
	return alpha2;
}

namespace {

struct inverse {
	static const unsigned int tr1  = 0;
	static const unsigned int tr21 = 1<<0;
	static const unsigned int tr22 = 1<<1;
	static const unsigned int tr23 = 1<<3;
	static const unsigned int tr3  = 1<<4;
	static const unsigned int tr4  = 1<<5;

	double lambda1_, phi1_, lambda2_, phi2_;
	unsigned int tr_;
	double cos_beta1_,  sin_beta1_, tan_beta1_;
	double cos_beta2_,  sin_beta2_, tan_beta2_;

	/* work variables for Newton iteration */
	double alpha1_, cos_alpha1_, sin_alpha1_;
	double alpha2_, cos_alpha2_, sin_alpha2_;
	double e_[Order];
	double I1_2m1_;
	double cos_alpha0_, sin_alpha0_;
	double sigma1_, cos_sigma1_, sin_sigma1_, cos2sigma1_, sin2sigma1_;
	double sigma2_, cos_sigma2_, sin_sigma2_, cos2sigma2_, sin2sigma2_;

	inverse(double lambda1, double phi1, double lambda2, double phi2):
		lambda1_(lambda1), phi1_(phi1),
		lambda2_(lambda2), phi2_(phi2),
		tr_(config_endpoints()),
		tan_beta1_(reduced_latitude(phi1_, cos_beta1_, sin_beta1_)),
		tan_beta2_(reduced_latitude(phi2_, cos_beta2_, sin_beta2_))
		{}

	double Newton_derivative();
	bool Newton_init(double& ret);
	double solve_direct_variant();
	void calc_azimuth(double& a1, double& a2);
	double case_meridian();
	double case_equator();

private:
	unsigned int config_endpoints();
	void calc_sphere_alpha1(double cos_omega12, double sin_omega12);
	void calc_sphere_alpha2(double cos_omega12, double sin_omega12);
	double calc_sphere_sigma12(double cos_omega12, double sin_omega12);
	double calc_reduced_length();
	double calc_w12();
	double calc_antipodal_alpha1(double x, double y);

};

unsigned int inverse::config_endpoints()
{
	normalize_coordinate(lambda1_, phi1_);
	normalize_coordinate(lambda2_, phi2_);
	double lambda12 = lambda2_ - lambda1_;
	unsigned int ret = tr1;

	if (lambda12 < -M_PI) {
		lambda12 = 2 * M_PI + lambda12;
		ret |= tr21;
	} else if (-M_PI <= lambda12 && lambda12 < 0) {
		lambda12 = -lambda12;
		ret |= tr22;
	} else if (0 <= lambda12 && lambda12 <= M_PI) {
		// do nothing
	} else if (M_PI < lambda12) {
		lambda12 = M_PI * 2 - lambda12;
		ret |= tr23;
	}
	lambda1_ = 0;
	lambda2_ = lambda12;

	if (fabs(phi1_) < fabs(phi2_)) {
		swap(phi1_, phi2_);
		ret |= tr3;
	}
	if (phi1_ > 0) {
		phi1_ = -phi1_; phi2_ = -phi2_;
		ret |= tr4;
	}

	return ret;
}
inline void inverse::calc_sphere_alpha1(double cos_omega12, double sin_omega12)
{
	double sin_omega122 = sq(sin_omega12);
	double sb1cb2 = sin_beta1_ * cos_beta2_;
	double sb2cb1 = sin_beta2_ * cos_beta1_;
	double sb2m1 = sb2cb1 - sb1cb2;
	double sb2p1 = sb2cb1 + sb1cb2;

	double R1 = cos_omega12 >= 0
		? sb2m1 + sb1cb2 * sin_omega122 / (1 + cos_omega12)
		: sb2p1 - sb1cb2 * sin_omega122 / (1 - cos_omega12);
	alpha1_ = atan2(cos_beta2_ * sin_omega12, R1);
	cos_alpha1_ = cos(alpha1_);
	sin_alpha1_ = sin(alpha1_);
}
inline void inverse::calc_sphere_alpha2(double cos_omega12, double sin_omega12)
{
	double sin_omega122 = sq(sin_omega12);
	double sb1cb2 = sin_beta1_ * cos_beta2_;
	double sb2cb1 = sin_beta2_ * cos_beta1_;
	double sb2m1 = sb2cb1 - sb1cb2;
	double sb2p1 = sb2cb1 + sb1cb2;

	double R2 = cos_omega12 <= 0
		?  sb2p1 + sb2cb1 * sin_omega122 / (1 - cos_omega12)
		: -sb2m1 + sb2cb1 * sin_omega122 / (1 + cos_omega12);
	alpha2_ = atan2(cos_beta1_ * sin_omega12, R2);
	cos_alpha2_ = cos(alpha2_);
	sin_alpha2_ = sin(alpha2_);
}
inline double inverse::calc_sphere_sigma12(
		double cos_omega12, double sin_omega12)
{
	double sin_omega122 = sq(sin_omega12);
	double sb1cb2 = sin_beta1_ * cos_beta2_;
	double sb2cb1 = sin_beta2_ * cos_beta1_;
	double sb2m1 = sb2cb1 - sb1cb2;
	double sb2p1 = sb2cb1 + sb1cb2;

	double R1 = cos_omega12 >= 0
		? sb2m1 + sb1cb2 * sin_omega122 / (1 + cos_omega12)
		: sb2p1 - sb1cb2 * sin_omega122 / (1 - cos_omega12);

	return atan2(
		hypot(cos_beta2_ * sin_omega12, R1),
		cos_beta1_ * cos_beta2_ * cos_omega12
				+ sin_beta1_ * sin_beta2_);
}
inline double inverse::calc_reduced_length()
{
	const double ep = sqrt(eccentricity2nd2);

	double I2_2m1 = calcI2diff(e_,
		sigma2_, cos2sigma2_, sin2sigma2_,
		sigma1_, cos2sigma1_, sin2sigma1_);
	double J_2m1 = I1_2m1_ - I2_2m1;
	double w1 = hypot(1, ep * cos_alpha0_ * sin_sigma1_);
	double w2 = hypot(1, ep * cos_alpha0_ * sin_sigma2_);
	return radius_b * (
		  w2 * cos_sigma1_ * sin_sigma2_
		- w1 * cos_sigma2_ * sin_sigma1_
		- cos_sigma1_ * cos_sigma2_ * J_2m1);
}
inline double inverse::Newton_derivative()
{
	if (!dbl_equal(cos_beta2_, cos_beta1_)
	 || !dbl_equal(cos_alpha1_, 0)) {
		return calc_reduced_length()
			/ (radius_a * cos_alpha2_ * cos_beta2_);
	} else {
		const double e = sqrt(eccentricity2);

		return - 2 * hypot(radius_b/radius_a/sin_beta1_, e);
	}
}
inline double inverse::calc_w12()
{
	return sqrt(1 - eccentricity2 * sq((cos_beta1_+cos_beta2_)/2));
}
inline double inverse::calc_antipodal_alpha1(double x, double y)
{
	double x2 = sq(x), y2 = sq(y);
	double r = (x2 + y2 - 1) / 6;
	double kappa = 0;
	if (!dbl_equal(x, 0) || r > 0) {
		double r2 = sq(r), r3 = r2 * r;
		double S = x2 * y2 / 4;
		double d = S * (S + 2 * r3);
		double u;
		double T = S + r3;
		if (d >= 0) {
			T += (T >= 0 ? sqrt(d) : -sqrt(d));
			T = pow(T, 1.0/3.0);
			u = r + T + r2 / T;
		} else {
			double psi = atan2(-sqrt(-d), T);
			u = r * (1 + 2 * cos(psi / 3));
		}
		double v = hypot(u, y);
		double vpu = u >= 0 ? v + u : y2 / (v - u);
		double w = (vpu - y2) / (2 * v);
		kappa = vpu / (sqrt(vpu + sq(w)) + w);
	}

	alpha1_ = atan2(-y/kappa, -x/(1+kappa)) + M_PI_2;
	cos_alpha1_ = cos(alpha1_);
	sin_alpha1_ = sin(alpha1_);
	return alpha1_;
}
inline bool inverse::Newton_init(double& ret)
{
	double sb2m1 = sin_beta2_ * cos_beta1_ - cos_beta2_ * sin_beta1_;

	if (lambda2_ < M_PI/6 && sb2m1 < 0.5) {
		double w12 = calc_w12();
		double omega12 = lambda2_ / w12;
		double co12 = cos(omega12);
		double so12 = sin(omega12);
		double sigma12 = calc_sphere_sigma12(co12, so12);
		if (sigma12 < 100 * sqrt(dblepsilon)) {
			calc_sphere_alpha1(co12, so12);
			calc_sphere_alpha2(co12, so12);
			ret = radius_a * w12 * sigma12;
			return true;
		} else {
			calc_sphere_alpha1(co12, so12);
			ret = alpha1_;
			return false;
		}
	}

	double omega12 = lambda2_;
	double co12 = cos(omega12);
	double so12 = sin(omega12);
	double sigma12 = calc_sphere_sigma12(co12, so12);
	calc_sphere_alpha1(co12, so12);

	alpha0(cos_alpha1_, sin_alpha1_,
		cos_beta1_, sin_beta1_, cos_alpha0_, sin_alpha0_);
	epsilons(cos_alpha0_, e_);

	double Delta = flattening * M_PI * A3(e_[0]) * cos_beta1_;
	if (sigma12 <= M_PI - 3 * Delta * cos_beta1_) {
		ret = alpha1_;
	} else {
		double x = (lambda2_ - M_PI) / Delta;
		double beta1 = atan2(sin_beta1_, cos_beta1_);
		double beta2 = atan2(sin_beta2_, cos_beta2_);
		double y = (beta2 + beta1) / (cos_beta1_ * Delta);

		if (y > -100 * dblepsilon 
		 && x > -1 - 1000 * sqrt(dblepsilon)) {
			sin_alpha1_ = min(1.0, -x);
			cos_alpha1_ = -sqrt(1 - sin_alpha1_*sin_alpha1_);
			alpha1_ = atan2(sin_alpha1_, cos_alpha1_);
			ret = alpha1_;
		} else {
			ret = calc_antipodal_alpha1(x, y);
		}
	}
	return false;
}
inline double inverse::solve_direct_variant()
{
	cos_alpha1_ = cos(alpha1_);
	sin_alpha1_ = sin(alpha1_);
	if (!dbl_equal(phi1_, 0)) {
		alpha0(cos_alpha1_, sin_alpha1_, cos_beta1_, sin_beta1_,
			cos_alpha0_, sin_alpha0_);
		double b22_b12 = (tan_beta1_ > -1)
			? (cos_beta2_ - cos_beta1_) * (cos_beta2_ + cos_beta1_)
			: (sin_beta1_ - sin_beta2_) * (sin_beta1_ + sin_beta2_);
		cos_alpha2_ = sqrt(sq(cos_alpha1_) * sq(cos_beta1_)
						+ b22_b12) / cos_beta2_;
		sin_alpha2_ = sin_alpha0_ / cos_beta2_;
		alpha2_ = atan2(sin_alpha2_, cos_alpha2_);
	} else {
		cos_alpha1_ =  fabs(cos_alpha1_);
		cos_alpha0_ =  cos_alpha1_;
		sin_alpha0_ =  sin_alpha1_;
		cos_alpha2_ = -cos_alpha1_;
		sin_alpha2_ =  sin_alpha1_;
		alpha2_ = M_PI - alpha1_;
	}

	sigma1_ = sigma(cos_alpha1_, tan_beta1_, cos_sigma1_, sin_sigma1_);
	double omega1 = atan2(sin_alpha0_ * tan_beta1_, cos_alpha1_);

	sigma2_     = atan2(tan_beta2_, cos_alpha2_);
	double tan_sigma2 = tan_beta2_ / cos_alpha2_;
	cos_sigma2_ = 1 / hypot(1, tan_sigma2);
	sin_sigma2_ = tan_sigma2 * cos_sigma2_;
	double omega2 = atan2(sin_alpha0_ * tan_beta2_, cos_alpha2_);

	epsilons(cos_alpha0_, e_);
	cos2sigma1_ = cos2x(cos_sigma1_, sin_sigma1_);
	sin2sigma1_ = sin2x(cos_sigma1_, sin_sigma1_);
	cos2sigma2_ = cos2x(cos_sigma2_, sin_sigma2_);
	sin2sigma2_ = sin2x(cos_sigma2_, sin_sigma2_);

	return (omega2 - omega1)
	     - flattening * sin_alpha0_
		* calcI3diff(e_,
			sigma2_, cos2sigma2_, sin2sigma2_,
			sigma1_, cos2sigma1_, sin2sigma1_);
}
inline void inverse::calc_azimuth(double& a1, double& a2)
{
	a1 = alpha1_;
	a2 = alpha2_;
	if (tr_ & tr4) {
		a1 = M_PI - a1;
		a2 = M_PI - a2;
	}
	if (tr_ & tr3) {
		double tmp_a1 = M_PI - a2;
		double tmp_a2 = M_PI - a1;
		a1 = tmp_a1;
		a2 = tmp_a2;
	}
	if (tr_ & tr21) {
		// do nothing
	} else if (tr_ & tr22) {
		a1 = -a1;
		a2 = -a2;
	} else if (tr_ & tr23) {
		// do nothing
	}
}
inline double inverse::case_meridian()
{
	if (dbl_equal(lambda2_, 0)) {
		alpha1_ = 0;     cos_alpha1_ = 1; sin_alpha1_ = 0;
		cos_sigma1_ = cos_beta1_;
		sin_sigma1_ = sin_beta1_;
		sigma1_     = atan2(sin_sigma1_, cos_sigma1_);
	} else {
		alpha1_ = M_PI; cos_alpha1_ = -1; sin_alpha1_ = 0;
		cos_sigma1_ = -cos_beta1_;
		sin_sigma1_ =  sin_beta1_;
		sigma1_     = dbl_equal(phi1_, 0)
			? -M_PI : atan2(sin_sigma1_, cos_sigma1_);
	}

	alpha2_ = 0; cos_alpha2_ = 1; sin_alpha2_ = 0;
	cos_sigma2_ = cos_beta2_;
	sin_sigma2_ = sin_beta2_;

	cos2sigma1_ = cos2x(cos_sigma1_, sin_sigma1_);
	sin2sigma1_ = sin2x(cos_sigma1_, sin_sigma1_);

	sigma2_     = atan2(sin_sigma2_, cos_sigma2_);
	cos2sigma2_ = cos2x(cos_sigma2_, sin_sigma2_);
	sin2sigma2_ = sin2x(cos_sigma2_, sin_sigma2_);

	epsilons(1, e_);
	I1_2m1_ = calcI1diff(e_,
			sigma2_, cos2sigma2_, sin2sigma2_,
			sigma1_, cos2sigma1_, sin2sigma1_);

	return radius_b * I1_2m1_;
}
inline double inverse::case_equator()
{
	alpha1_ = alpha2_ = M_PI_2;
	return radius_a * (lambda2_ - lambda1_);
}
}

double solve_inverse_problem(
	double lambda1, double phi1, double lambda2, double phi2,
	double& alpha1, double& alpha2)
{
	inverse v(lambda1, phi1, lambda2, phi2);

	double lambda12 = v.lambda2_;
	if (lambda12) {
		lambda12 = fmod(lambda12, M_PI);
	}
	if (dbl_equal(lambda12, 0) || dbl_equal(v.phi1_, -M_PI_2)) {
		double s12 = v.case_meridian();
		v.calc_azimuth(alpha1, alpha2);
		return s12;
	}
	if (dbl_equal(v.phi1_, v.phi2_)
	 && dbl_equal(v.phi1_, 0)
	 && v.lambda2_ <= (1 - flattening) * M_PI) {
		double s12 = v.case_equator();
		v.calc_azimuth(alpha1, alpha2);
		return s12;
	}


	double s12;
	if (v.Newton_init(s12)) {
		v.calc_azimuth(alpha1, alpha2);
		return s12;
	}

	bool end = false;
	for (int limit = 0; limit < 50; limit++) {
		double dy = v.solve_direct_variant() - v.lambda2_;
		v.I1_2m1_ = calcI1diff(v.e_,
			v.sigma2_, v.cos2sigma2_, v.sin2sigma2_,
			v.sigma1_, v.cos2sigma1_, v.sin2sigma1_);

		if (end) { break; }

		double dx = v.Newton_derivative();
		double dlt = dy /dx;
		double org = v.alpha1_;
		v.alpha1_ -= dlt;

		if (fabs(dlt) <= sqrt(dblepsilon) / (1<<10) * fabs(org)) {
			end = true;
		}
	}

	v.calc_azimuth(alpha1, alpha2);
	return radius_b * v.I1_2m1_;
}


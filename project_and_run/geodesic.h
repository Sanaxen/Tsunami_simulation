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

#ifndef _simx_geodesic_h_
#define _simx_geodesic_h_

// GRS80
const double radius_a		= 6378137.0;		// [m]
const double flattening		= 1.0/298.257222101;

const double radius_b		= radius_a * (1 - flattening);
const double flattening3rd	= flattening / (2 - flattening);
const double eccentricity2	= flattening * (2 - flattening);
const double eccentricity2nd2
	= flattening * (2 - flattening) / ((1 - flattening) * (1 - flattening));


double solve_direct_problem(
	double lambda1, double phi1, double s12, double alpha1,
	double& lambda2, double& phi2);

double solve_inverse_problem(
	double lambda1, double phi1, double lambda2, double phi2,
	double& alpha1, double& alpha2);

#endif	// _simx_geodesic_h_

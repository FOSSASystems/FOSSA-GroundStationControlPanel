/***************************************************************************
 * Name: solarProcedures.cpp
 *
 * Description: Solar System calculation procedures.
 *              Implements some procedures needed for satellite functionality
 *
 *
 ***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2004 by J. L. Canales                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "solarProcedures.hpp"
#include <math.h>

#include "stdsat.h"

#include <stdio.h>
#include <stdlib.h>

/*

Procedure Calculate_Solar_Position(time : double;
                       var solar_vector : vector);
  var
    mjd,year,T,M,L,e,C,O,Lsa,nu,R,eps : double;
    ob                                : vector;
  begin
  mjd := time - 2415020.0;
  year := 1900 + mjd/365.25;
  T := (mjd + Delta_ET(year)/secday)/36525.0;

  M := Radians(Modulus(358.47583 + Modulus(35999.04975*T,360.0)
     - (0.000150 + 0.0000033*T)*Sqr(T),360.0));
  L := Radians(Modulus(279.69668 + Modulus(36000.76892*T,360.0)
     + 0.0003025*Sqr(T),360.0));
  e := 0.01675104 - (0.0000418 + 0.000000126*T)*T;
  C := Radians((1.919460 - (0.004789 + 0.000014*T)*T)*Sin(M)
     + (0.020094 - 0.000100*T)*Sin(2*M) + 0.000293*Sin(3*M));
  O := Radians(Modulus(259.18 - 1934.142*T,360.0));
  Lsa := Modulus(L + C - Radians(0.00569 - 0.00479*Sin(O)),twopi);
  nu := Modulus(M + C,twopi);
  R := 1.0000002*(1 - Sqr(e))/(1 + e*Cos(nu));
  eps := Radians(23.452294 - (0.0130125 + (0.00000164 - 0.000000503*T)*T)*T
       + 0.00256*Cos(O));
  R := AU*R;
  solar_vector[1] := R*Cos(Lsa);
  solar_vector[2] := R*Sin(Lsa)*Cos(eps);
  solar_vector[3] := R*Sin(Lsa)*Sin(eps);
  solar_vector[4] := R;
  end; {Procedure Calculate_Solar_Position}
*/


// Operation computeSolarPosition
//! @brief This operation compute the sun ECI coordinates for the
//! ai_epoch time
//! @details
//! References:
//!  Fundamentals of Astrodynamics and Applications
//!   David A. Vallado
//!   Chapter 5.1 Solar Phenomena
//! @param[in] ai_epoch  GTime. Epoch for the ECI reference system calculation
//! @param[out] ao_TEMEposition gVector Sun position vector

gVector computeSolarPosition( gTime ai_epoch)
{

	//Variables definition
    //mjd,year,T,M,L,e,C,O,Lsa,nu,R,eps : double;
    //ob                                : vector;

	gVector sunTEMEposition(3);
	double sunLongitudeDeg; //meassured in Deg.
	double JCenturies = ai_epoch.toJCenturies();
	double sunMeanAnomalyRad; //meassured in Radians.
	double eclipticLongitudeRad; //meassured in Radians.
	double sunRKm; //meassured in Km
	double eclipticObliquityRad;


	sunLongitudeDeg   = fmod((280.460 + (36000.771 * JCenturies)),360.0);

	sunMeanAnomalyRad = fmod((357.5277233 + (35999.05034 * JCenturies)),360.0)
						* KDEG2RAD;

	eclipticLongitudeRad =  (sunLongitudeDeg
			             + 1.914666471 * sin(sunMeanAnomalyRad)
			             + 0.019994643*sin(2*sunMeanAnomalyRad))
			             * KDEG2RAD;

	sunRKm = (1.000140612
		     - 0.016708617 * cos(sunMeanAnomalyRad)
		     - 0.000139589 * cos(2 * sunMeanAnomalyRad))
		     * KAU;

	eclipticObliquityRad = (23.439291 - 0.0130042*JCenturies) * KDEG2RAD;

	sunTEMEposition[0] = sunRKm * cos(eclipticLongitudeRad);
	sunTEMEposition[1] = sunRKm * cos(eclipticObliquityRad) * sin(eclipticLongitudeRad);
	sunTEMEposition[2] = sunRKm * sin(eclipticObliquityRad) * sin(eclipticLongitudeRad);


	return sunTEMEposition;

}

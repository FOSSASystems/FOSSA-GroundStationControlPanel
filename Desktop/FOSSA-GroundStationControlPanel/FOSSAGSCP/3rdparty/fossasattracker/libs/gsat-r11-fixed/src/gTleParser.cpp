/**********************************************************************
** Name: GTleParser.cpp
**
** $Date: 2007-03-19 18:39:30 +0100 (Mon, 19 Mar 2007) $
** $Revision: 10 $
** $HeadURL: http://gsat.svn.sourceforge.net/svnroot/gsat/tags/gsat-0.1.0/src/GKepnasa.cpp $
**
** Description: GKepNasa class is a TLE files parser that
**			     extract the satellites keplerian data to generate
** 				 a satellite list.
**
**********************************************************************/

/***************************************************************************
 *   Copyright (C) 2004 by JL Trabajo                                      *
 *   ph03696@homeserver                                                    *
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
 *   You should haGINT32ve received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <fstream>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
// GKepFile
#include "gTleParser.hpp"
// GExcpt
#include "stdsat.h"

using namespace std;

/*Line 1
1 NNNNNU NNNNNAAA NNNNN.NNNNNNNN +.NNNNNNNN +NNNNN-N +NNNNN-N N NNNNN

01-01 Line Number of Element Data
03-07 Satellite Number
10-11 International Designator (Last two digits of launch year)
12-14 International Designator (Launch number of the year)
15-17 International Designator (Piece of launch)
19-20 Epoch Year (Last two digits of year)
21-32 Epoch (Julian Day and fractional portion of the day)
34-43 First Time Derivative of the Mean Motion divided by 2., or Ballistic Coefficient (Depending of ephemeris type)
45-52 Second Time Derivative of Mean Motion divided by 6. (Blank if N/A)
54-61 BSTAR drag term if GP4 general perturbation theory was used. Otherwise, radiation pressure coefficient.
63-63 Ephemeris type
65-68 Element number
69-69 Check Sum (Modulo 10)

*/

/* Line 2
2 NNNNN NNN.NNNN NNN.NNNN NNNNNNN NNN.NNNN NNN.NNNN NN.NNNNNNNNNNNNNN

01-01 Line Number of Element Data
03-07 Satellite Number
09-16 Inclination [Degrees]
18-25 Right Ascension of the Ascending Node [Degrees]
27-33 Eccentricity (decimal point assumed)
35-42 Argument of Perigee [Degrees]
44-51 Mean Anomaly [Degrees]
53-63 Mean Motion [Revs per day]
64-68 Revolution number at epoch [Revs]
69-69 Check Sum (Modulo 10)
*/


/* -----------------------------------------------------------------------------
*
*                           function ParseTleData
*
*  this function converts the two line element set character string data to
*    variables and initializes the sgp4 variables. several intermediate varaibles
*    and quantities are determined. note that the result is a structure so multiple
*    satellites can be processed simultaneously without having to reinitialize. the
*    verification mode is an important option that permits quick checks of any
*    changes to the underlying technical theory. this option works using a
*    modified tle file in which the start, stop, and delta time values are
*    included at the end of the second line of data. this only works with the
*    verification mode. the catalog mode simply propagates from -1440 to 1440 min
*    from epoch and is useful when performing entire catalog runs.

  --------------------------------------------------------------------------- */

#include <math.h>
#include "gTleParser.hpp"
#include "stdsat.h"


KeplerianData_t ParseTleData( const char *pstrName, 
			      char *pstrTleLine1, char *pstrTleLine2,
			      char ai_typerun){


  KeplerianData_t KepData;

  //Variables to colect TLE data from File	

  long int  satnum;
  int       epochyr, j,//for loops index
  	    nexp, //Balistic coeficient exponent
  	    ibexp, // bstar exponent
  	    year;
  	    
  int 	    cardnumb, numb;
  long	    revnum = 0, elnum = 0;
  char 	    classification, intldesg[11];
  double    epochdays, nddot, ndot,
            bstar  , inclo  , nodeo    , ecco , argpo , mo, no;

       
       
       
	// set the implied decimal points since doing a formated read
	// fixes for bad input data values (missing, ...)
	for ( j = 10; j <= 15; j++)
	   if (pstrTleLine1[j] == ' ')
	       pstrTleLine1[j] = '_';
	
	if (pstrTleLine1[44] != ' ')
	   pstrTleLine1[43] = pstrTleLine1[44];
	pstrTleLine1[44] = '.';
	if (pstrTleLine1[7] == ' ')
	   pstrTleLine1[7] = 'U';
	if (pstrTleLine1[9] == ' ')
	   pstrTleLine1[9] = '.';
	for (j = 45; j <= 49; j++)
	   if (pstrTleLine1[j] == ' ')
	       pstrTleLine1[j] = '0';
	if (pstrTleLine1[51] == ' ')
	   pstrTleLine1[51] = '0';
	if (pstrTleLine1[53] != ' ')
	   pstrTleLine1[52] = pstrTleLine1[53];
	pstrTleLine1[53] = '.';
	pstrTleLine2[25] = '.';
	for (j = 26; j <= 32; j++)
	   if (pstrTleLine2[j] == ' ')
	       pstrTleLine2[j] = '0';
	if (pstrTleLine1[62] == ' ')
	   pstrTleLine1[62] = '0';
	if (pstrTleLine1[68] == ' ')
	   pstrTleLine1[68] = '0';
	
	sscanf(pstrTleLine1,"%2d %5ld %1c %10s %2d %12lf %11lf %7lf %2d %7lf %2d %2d %6ld\n ",
	               &cardnumb,&satnum,&classification, intldesg, &epochyr,
	               &epochdays,&ndot, &nddot, &nexp, &bstar,
	               &ibexp, &numb, &elnum);

	if ( pstrTleLine2[52] == ' ')
         sscanf( pstrTleLine2,"%2d %5ld %9lf %9lf %8lf %9lf %9lf %10lf %6ld\n",
                 &cardnumb,&satnum, &inclo,
                 &nodeo,&ecco, &argpo, &mo, &no,
                 &revnum);
    else
         sscanf( pstrTleLine2,"%2d %5ld %9lf %9lf %8lf %9lf %9lf %11lf %6ld\n",
                 &cardnumb,&satnum, &inclo,
                 &nodeo,&ecco, &argpo, &mo, &no,
                 &revnum);

    //Falta incluir el Nombre
	KepData.SatName	       = pstrName;
	KepData.Classification = classification;
	KepData.SatNum	       = satnum ;
	KepData.IntDesign      = intldesg;
	KepData.Eccentricity   = ecco;
	KepData.OrbitNum       = (long)revnum/10;
	KepData.EphemType      = numb;
	KepData.ElsetNum       = (long)elnum/10;
	
	
       // ---------------- temp fix for years from 1957-2056 -------------------
       // --------- correct fix will occur when year is 4-digit in tle ---------
       if (epochyr < 57)
           year= epochyr + 2000;
         else
           year= epochyr + 1900;
	
       KepData.MessurementDate.setTime( year, epochdays); // Keplerian data Messurement Date



    // ---- find no, ndot, nddot ----
    KepData.MeanMotion  = no / XPDOTP; //* rad/min
    KepData.nddot= nddot * pow(10.0, nexp);
    KepData.bstar= bstar * pow(10.0, ibexp);

    // ---- convert to sgp4 units ----
    KepData.ndot = ndot  / (XPDOTP*1440.0);  //* ? * minperday
    KepData.nddot= KepData.nddot / (XPDOTP*1440.0*1440);

    // ---- find standard orbital elements ----
    KepData.Inclination = inclo  * KDEG2RAD;
    KepData.RAAN 	    = nodeo  * KDEG2RAD;
    KepData.ArgPerigee  = argpo  * KDEG2RAD;
    KepData.MeanAnomaly = mo     * KDEG2RAD;

	return(KepData);
}

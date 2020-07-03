/*
MIT License

Copyright (c) 2020 Richard G. Bamford

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <iostream>
#include <iomanip>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <gSatTEME.hpp>
#include <gTime.hpp>
#include <gVector.hpp>
#include <gObserver.hpp>

#include <raylib.h>
#include <raymath.h>

class SatellitePassInformation
{
public:
	SatellitePassInformation()
	{
		//
		// Satellite configuration.
		//
		char pstrTleLine1[130]="1 44829U 19084F   20184.87561446  .00042981  00000-0  24679-3 0  9990";
		char pstrTleLine2[130]="2 44829  96.9731  54.0045 0026760 173.0170 187.1476 15.78131063 32871";

		gSatTEME *satellite;
		satellite = new gSatTEME("FOSSASAT-1", pstrTleLine1, pstrTleLine2);
		
		//
		// Observer configuration.
		//
		gObserver observer;
		double birchwoodLat, birchwoodLong, birchwoodAttitude;
		birchwoodLat = 53.45143;
		birchwoodLong = -2.612575;
		birchwoodAttitude = 1345.0;
		observer.setPosition(birchwoodLat, birchwoodLong, birchwoodAttitude);
		
		//
		// Current time as jdays epoch.
		//
		gTime epoch; // 2458824.00000
		epoch.setTime(2019,184.87561446);
		
		int year, mon, day, hr, min;
		double sec;
		invjday(epoch.getGmtTm(), year, mon, day, hr, min, sec);
		
		int durationMinutes = 60; // from epoch for this long is the window of samples.
		int sampleRate = 1; // how many minutes between each sample.
		gTimeSpan tsince; // jdays offset from epoch to the above sampled time.

		for(int minFromEpoch = 0; minFromEpoch <= durationMinutes; minFromEpoch += sampleRate)
		{
			tsince = (double)minFromEpoch/KMIN_PER_DAY;
			
			gTime sampleEpoch = epoch + tsince; // calculate the offset time.

			//
			// Calculate satellite position, velocity and LatLong position.
			//
			satellite->setEpoch(sampleEpoch); // set the time of the simulation to epoch + sample offset.
			
			gVector position = satellite->getPos(); // get position in TEME
			gVector velocity = satellite->getVel(); // get velocity in TEME
			gVector latlong = satellite->getSubPoint(); // get the sub-point (defined here http://www.celestrak.com/columns/v02n03/)
			
			
			printf("%lf, %lf\r\n", latlong[0], latlong[1], latlong[2]);
			
			//
			// Calculate the look parameters from the observer to satellite.
			//
			gVector observerAziElevPos = observer.calculateLook(*satellite, sampleEpoch);
			double azimuth = observerAziElevPos[AZIMUTH]; // #define AZIMUTH in gsat.
			double elevation = observerAziElevPos[ELEVATION]; // #define ELEVATION in gsat.
			double slantRange = observerAziElevPos[RANGE]; // #define RANGE in gsat
			double rangeRate = observerAziElevPos[RANGERATE]; // #define RANGERATE in gsat.
			
		}
		
		delete satellite;
	}
};

void GetSatellitePositions()
{
	
}

int main(int argc, char* argv[])
{
	
 	std::cout << "Satellite SGP4 calculation Example\n";

	
	
}

/*
int main(int argc, char* argv[])
{

	gTimeSpan tsince;
	
	char pstrTleLine1[130]="1 44829U 19084F   20184.87561446  .00042981  00000-0  24679-3 0  9990";
	char pstrTleLine2[130]="2 44829  96.9731  54.0045 0026760 173.0170 187.1476 15.78131063 32871";

 	std::cout << "Satellite SGP4 calculation Example\n";

	gSatTEME *satellite;
	satellite = new gSatTEME("FOSSASAT-1", pstrTleLine1, pstrTleLine2);
	
	gTime epoch;
	epoch.setTime(2019, 184.87561446);
	
	int durationMinutes = 60; // 1 hours.
	int sampleRate = 1; // 10 mins

	for(int minFromEpoch = 0; minFromEpoch <= durationMinutes; minFromEpoch += sampleRate)
	{
		tsince = (double)minFromEpoch/KMIN_PER_DAY;
		
		gTime calEpoch = epoch + tsince;

		satellite->setEpoch(calEpoch);
		
		gVector position = satellite->getPos();
		gVector velocity = satellite->getVel();
		gVector latlong = satellite->getSubPoint();
		
		// check SConversion and compare to GSAt.
		
		int year, mon, day, hr, min;
		double sec;
		invjday(calEpoch.getGmtTm(),
				year, mon, day, hr, min, sec);
		double totalSec = (hr * 60 * 60) + (min * 60) + sec;
	
		
		// Eci to Ecr.
		double eciToEcr[3][3];
		SCoord::makeEciToEcrMatrix(totalSec, eciToEcr);
		
		double eciPos[3] = { position[0], position[1], position[2] }; // TEME position
		double ecrPos[3];
		SCoord::posEciToEcr(eciToEcr, eciPos, ecrPos);
		
		// ECR to LLA (geodetic)
		double llaPos[3];
		SCoord::posEcrToGeodetic(ecrPos, llaPos);
		
		// geodetic to geocentric LLA.
		double geocentricLLA = SCoord::getGeocentricLatFromLLA(llaPos);
		llaPos[0] = geocentricLLA;
		
		if (llaPos[0] > (pi/2.0))
		{
			llaPos[0] -= (2*pi);
		}
		
		llaPos[0] = llaPos[0] * (180.0/pi);
		llaPos[1] = llaPos[1] * (180.0/pi);
		
		if (llaPos[1] < -180.0)
		{
			llaPos[1] += 360.0;
		}
		else if (llaPos[1] > 180.0f)
		{
			llaPos[1] -= 360.0f;
		}
		printf("SConvert: %lf, %lf, %lf\r\n", llaPos[0], llaPos[1], 0.0);
		printf("GSat: %lf, %lf, %lf\r\n", latlong[0], latlong[1], latlong[2]);
		printf("\r\n");

	}
	delete satellite;
}
*/

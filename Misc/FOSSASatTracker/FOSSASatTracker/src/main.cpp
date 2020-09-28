/*
MIT License

Copyright (c) 2020 FOSSA Systems

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

#include <gTime.hpp>
#include <gObserver.hpp>

#include "TLE.hpp"
#include "SatelliteSimulation.hpp"


int main(int argc, char* argv[])
{
	//
	// Create the TLE.
	//
	char TLELine1[70]="1 44829U 19084F   20184.87561446  .00042981  00000-0  24679-3 0  9990";
	char TLELine2[70]="2 44829  96.9731  54.0045 0026760 173.0170 187.1476 15.78131063 32871";
	
	FOSSASatTracker::TLE tle(TLELine1, TLELine2);
	
	
	//
	// Create the observer.
	//
	gObserver observer;
	double birchwoodLat, birchwoodLong, birchwoodAttitude;
	birchwoodLat = 53.45143;
	birchwoodLong = -2.612575;
	birchwoodAttitude = 1345.0;
	observer.setPosition(birchwoodLat, birchwoodLong, birchwoodAttitude);
	
	
	//
	// Initialize the satellite simulation.
	//
	FOSSASatTracker::SatelliteSimulation satelliteSimulation("FOSSASAT-1", tle);
	
	
	//
	// A simulation run with both observer and epoch.
	//
	gTime epoch = gTime::getCurrentTime();
	gTimeSpan timeSinceEpoch;
	for (int mins = 0; mins < 270; mins++)
	{
		timeSinceEpoch = (double)(mins)/KMIN_PER_DAY;
		gTime newEpoch = epoch + timeSinceEpoch;
		
		FOSSASatTracker::SimulationResult result = satelliteSimulation.Simulate(observer, newEpoch);

		FOSSASatTracker::ObserverInformation& observerInfo = result.GetObserverInformation();
		FOSSASatTracker::SatelliteInformation& satelliteInfo = result.GetSatelliteInformation();
		
		const gVector& velocity = satelliteInfo.GetVelocity();
		//std::cout << velocity << std::endl;

		const double& elevation = observerInfo.GetElevation();
		const gVector& subPoint = satelliteInfo.GetSubPoint(); // the subpoint is the geocentric latitude and longitude.
		
		const double& rangeRate = observerInfo.GetRangeRate(); // in KM
		
		time_t timeNowEpoch = newEpoch.toTime();
		
		std::cout << timeNowEpoch << "," << rangeRate << std::endl;
		
		//printf("%jd,%lf,%lf,%lf\r\n", timeNowEpoch, subPoint[0], subPoint[1], elevation);
	}
	
	return 0;
}

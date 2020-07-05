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

#include "SatelliteSimulation.hpp"

namespace FOSSASatTracker
{


SatelliteSimulation::SatelliteSimulation(std::string satelliteName, TLE tle) : m_tle(tle), m_satelliteName(satelliteName)
{
	const char* cstrName = satelliteName.c_str();
	
	m_satellite = new gSatTEME(cstrName, m_tle.GetFirstLine(), m_tle.GetSecondLine());
}

SatelliteSimulation::~SatelliteSimulation()
{
	delete m_satellite;
}

FOSSASatTracker::SimulationResult SatelliteSimulation::Simulate(gTime epoch)
{
	if (m_satellite == nullptr)
	{
		throw std::runtime_error("Satellite member is null.");
	}
	
	m_satellite->setEpoch(epoch);
	
	// Get satellite information.
	gVector position = m_satellite->getPos(); // get position in TEME
	gVector velocity = m_satellite->getVel(); // get velocity in TEME
	gVector latlong = m_satellite->getSubPoint(); // get the sub-point (defined here http://www.celestrak.com/columns/v02n03/)
	
	SatelliteInformation satelliteInformation(position, velocity, latlong);
	
	
	// Combine into composite.
	SimulationResult result;
	result.AddSatelliteInformation(satelliteInformation);
	return result;
}

FOSSASatTracker::SimulationResult SatelliteSimulation::Simulate(gObserver observer, gTime epoch)
{
	if (m_satellite == nullptr)
	{
		throw std::runtime_error("Satellite member is null.");
	}
	
	m_satellite->setEpoch(epoch);

	
	// Get satellite information.
	gVector position = m_satellite->getPos(); // get position in TEME
	gVector velocity = m_satellite->getVel(); // get velocity in TEME
	gVector latlong = m_satellite->getSubPoint(); // get the sub-point (defined here http://www.celestrak.com/columns/v02n03/)
	
	SatelliteInformation satelliteInformation(position, velocity, latlong);
	
	
	
	// Get observer information.
	gVector observerAziElevPos = observer.calculateLook(*m_satellite, epoch);
	
	double azimuth = observerAziElevPos[AZIMUTH]; // #define AZIMUTH in gsat.
	double elevation = observerAziElevPos[ELEVATION]; // #define ELEVATION in gsat.
	double slantRange = observerAziElevPos[RANGE]; // #define RANGE in gsat
	double rangeRate = observerAziElevPos[RANGERATE]; // #define RANGERATE in gsat.
	
	ObserverInformation observerInformation(azimuth, elevation, slantRange, rangeRate);
	
	
	// Combine into composite.
	SimulationResult result;
	result.AddSatelliteInformation(satelliteInformation);
	result.AddObserverInformation(observerInformation);
	return result;
}

}

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

#ifndef SatelliteSimulation_hpp
#define SatelliteSimulation_hpp

#include <gTime.hpp>
#include <gVector.hpp>
#include <gSatTEME.hpp>
#include <gObserver.hpp>

#include "TLE.hpp"
#include "SimulationResult.hpp"

namespace FOSSASatTracker
{

//!  @brief This class encapsulates 1 satellite, this is because multiple
//!			satellite objects could be made to plot more than 1 satellite.
class SatelliteSimulation
{
public:
	SatelliteSimulation(std::string satelliteName, TLE tle);
	
	~SatelliteSimulation();
	
	SatelliteSimulation(const SatelliteSimulation& other);
	
	//! @brief gTIme is part of the gsat library, it uses julian days. see: gTime.setTime(year, jday)
	//!		e.g. gTime time(2019,184.87561446); // year and julian day of year
	//!		e.g. gTime time(2458824.00000); // julian days
	SimulationResult Simulate(gTime epoch);
	SimulationResult Simulate(gObserver observer, gTime epoch);
private:
	gSatTEME* m_satellite;
	TLE m_tle;
	std::string m_satelliteName;
};

}

#endif /* SatelliteSimulation_hpp */

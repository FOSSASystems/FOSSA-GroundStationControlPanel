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

#include "SimulationResult.hpp"

namespace FOSSASatTracker
{

SimulationResult::SimulationResult()
{
}

SimulationResult::~SimulationResult()
{
}

SimulationResult::SimulationResult(const FOSSASatTracker::SimulationResult &other)
{
	// deep copy.
	
	m_satelliteInformationAssigned = other.m_satelliteInformationAssigned;
	m_observerInformationAssigned = other.m_observerInformationAssigned;

	// if the other satellite has new satellite information.
	if (m_satelliteInformationAssigned)
	{
		m_satelliteInformation = other.m_satelliteInformation;
	}
	if (m_observerInformationAssigned)
	{
		m_observerInformation = other.m_observerInformation;
	}
}

void SimulationResult::AddSatelliteInformation(SatelliteInformation satelliteInformation)
{
	m_satelliteInformation = satelliteInformation;
	m_satelliteInformationAssigned = true;
}

bool SimulationResult::HasSatelliteInformation()
{
	return m_satelliteInformationAssigned;
}

FOSSASatTracker::SatelliteInformation &SimulationResult::GetSatelliteInformation()
{
	return m_satelliteInformation;
}

void SimulationResult::AddObserverInformation(ObserverInformation observerInformation)
{
	m_observerInformation = observerInformation;
	m_observerInformationAssigned = true;
}

bool SimulationResult::HasObserverInformation()
{
	return m_observerInformationAssigned;
}

FOSSASatTracker::ObserverInformation &SimulationResult::GetObserverInformation()
{
	return m_observerInformation;
}


}

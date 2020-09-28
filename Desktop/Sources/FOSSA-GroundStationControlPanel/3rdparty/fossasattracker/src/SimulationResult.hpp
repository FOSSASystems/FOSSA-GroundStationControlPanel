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

#ifndef SimulationResult_hpp
#define SimulationResult_hpp

#include "SatelliteInformation.hpp"
#include "ObserverInformation.hpp"

namespace FOSSASatTracker
{

//! @brief This class encapsulates the results of the GSat calculations,
//! 			it is a composite design pattern.
class SimulationResult
{
public:
	SimulationResult();
	~SimulationResult();
	SimulationResult(const SimulationResult& other);
	
	void AddSatelliteInformation(SatelliteInformation satelliteInformation);
	bool HasSatelliteInformation();
	SatelliteInformation& GetSatelliteInformation();
	
	void AddObserverInformation(ObserverInformation observerInformation);
	bool HasObserverInformation();
	ObserverInformation& GetObserverInformation();

private:
	SatelliteInformation m_satelliteInformation;
	bool m_satelliteInformationAssigned = false;
	
	ObserverInformation m_observerInformation;
	bool m_observerInformationAssigned = false;
};

}
#endif /* SimulationResult_hpp */

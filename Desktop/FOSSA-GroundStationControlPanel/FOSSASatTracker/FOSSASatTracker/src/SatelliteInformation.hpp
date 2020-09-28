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

#ifndef SatelliteInformation_hpp
#define SatelliteInformation_hpp

#include <gVector.hpp>

namespace FOSSASatTracker
{

class SatelliteInformation
{
public:
	SatelliteInformation();
	
	SatelliteInformation(gVector position,
						 gVector velocity,
						 gVector subPoint);
	
	~SatelliteInformation();
	
	SatelliteInformation(const SatelliteInformation& other);
	
	const gVector& GetPosition();
	const gVector& GetVelocity();
	const gVector& GetSubPoint();
private:
	gVector m_position; //! Position in TEME (ECI)
	gVector m_velocity; //! Velocity in TEME (ECI)
	gVector m_subpoint; //! Sub point position in geocentric lat long.
};

}

#endif /* SatelliteInformation_hpp */

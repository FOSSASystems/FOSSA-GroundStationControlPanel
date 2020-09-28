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

#include "SatelliteInformation.hpp"

namespace FOSSASatTracker
{

SatelliteInformation::SatelliteInformation()
{
	
}

SatelliteInformation::SatelliteInformation(gVector position,
										   gVector velocity,
										   gVector subPoint) : m_position(position), m_velocity(velocity), m_subpoint(subPoint)
{
	
}

SatelliteInformation::~SatelliteInformation()
{
}

SatelliteInformation::SatelliteInformation(const FOSSASatTracker::SatelliteInformation &other)
{
	m_position = other.m_position;
	m_velocity = other.m_velocity;
	m_subpoint = other.m_subpoint;
}

const gVector& SatelliteInformation::GetPosition()
{
	return m_position;
}

const gVector& SatelliteInformation::GetVelocity()
{
	return m_velocity;
}

const gVector& SatelliteInformation::GetSubPoint()
{
	return m_subpoint;
}




}

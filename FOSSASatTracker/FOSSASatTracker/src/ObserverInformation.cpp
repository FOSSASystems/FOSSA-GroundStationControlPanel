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

#include "ObserverInformation.hpp"

namespace FOSSASatTracker
{

ObserverInformation::ObserverInformation()
{
	
}

ObserverInformation::ObserverInformation(double azimuth,
										 double elevation,
										 double range,
										 double rangeRate) : m_azimuth(azimuth), m_elevation(elevation), m_range(range), m_rangeRate(rangeRate)
{
}

ObserverInformation::~ObserverInformation()
{

}

ObserverInformation::ObserverInformation(const FOSSASatTracker::ObserverInformation &other)
{
	m_azimuth = other.m_azimuth;
	m_elevation = other.m_elevation;
	m_range = other.m_range;
	m_rangeRate = other.m_rangeRate;
}

const double& ObserverInformation::GetAzimuth()
{
	return m_azimuth;
}

const double& ObserverInformation::GetElevation()
{
	return m_elevation;
}

const double& ObserverInformation::GetRange()
{
	return m_range;
}

const double& ObserverInformation::GetRangeRate()
{
	return m_rangeRate;
}



}

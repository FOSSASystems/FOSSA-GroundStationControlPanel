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

#include "TLE.hpp"

namespace FOSSASatTracker
{

TLE::TLE(char lineA[70], char lineB[70])
{
	//
	// Copy lineA to m_lineA
	//
	{
		char* destination = &(m_lineA[0]); // point to the first element in the line array member.
		char* source = &(lineA[0]); // point to the first element in the lineA argument.
		size_t length = 70;
		
		memcpy(destination, source, 70);
	}
	
	//
	// Copy lineB to m_lineB
	//
	{
		char* destination = &(m_lineB[0]);
		char* source = &(lineB[0]);
		size_t length = 70;
		
		memcpy(destination, source, 70);
	}
}

char* TLE::GetFirstLine()
{
	return m_lineA;
}

char* TLE::GetSecondLine()
{
	return m_lineB;
}

}

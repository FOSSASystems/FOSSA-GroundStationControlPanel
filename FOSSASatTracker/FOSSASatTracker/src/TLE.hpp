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

#ifndef TLE_hpp
#define TLE_hpp

#include <cstdlib>
#include <cstdio>
#include <cstring>

namespace FOSSASatTracker
{

class TLE
{
public:
	//! @brief This class encapsulates the TLE information.s
	//!  it copies the character arrays into its internal arrays.
	//! @param[in] 	lineA First line of the TLE as characters
	//! @param[in] 	lineB Second line of the TLE as characters.
	TLE(char lineA[70], char lineB[70]);
	
	char* GetFirstLine();
	char* GetSecondLine();
private:
	char m_lineA[70] = { '\0' };
	char m_lineB[70] = { '\0' };
};

}
#endif /* TLE_hpp */

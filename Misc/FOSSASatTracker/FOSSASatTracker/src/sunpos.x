/*
 MIT License

 Copyright (c) 2020 Richard G. Bamford

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

#include <solarProcedures.hpp>
#include <scoord.h>

int main()
{
	
	std::cout << "Program start.\n" << std::endl;
	
	gTime timeNow = gTime::getCurrentTime();
	double gmtJulianTimeNow = timeNow.getGmtTm();
	double gmstTheta = timeNow.toThetaGMST(); // rads.
	
	// (s) * (rads/s) = rads
	time_t rawtime;
	struct tm* ptm;
	time(&rawtime);
	ptm = gmtime(&rawtime);
	int seconds = (ptm->tm_hour*60*60) + (ptm->tm_min*60) + ptm->tm_sec;
	
	
	std::cout << "Julian GMT Time (julian days): " << gmtJulianTimeNow << std::endl;;
	
	gTime epoch(gmtJulianTimeNow);
	gVector posVector;
	posVector = computeSolarPosition(epoch);
	
	// eci to ecr matrix.
	double eciToEcr[3][3];
	SCoord::makeEciToEcrMatrix<double>(seconds, eciToEcr);
	
	// eci pos to ecr pos.
	double eciPos[3];
	eciPos[0] = posVector[0];
	eciPos[1] = posVector[1];
	eciPos[2] = posVector[2];
	double ecrPos[3];
	SCoord::posEciToEcr(eciToEcr, eciPos, ecrPos);

	// ecr pos to  geodesic (LLA)
	double llaPos[3];
	SCoord::posEcrToGeodetic(ecrPos, llaPos);
	
	double degL = llaPos[0] * (180/M_PI);
	double degLL = llaPos[1] * (180/M_PI);
	
	printf("Lat: %lf Long: %lf \r\n", degL, degLL);
	for( int i=0; i<3 ; i++)
	{
		printf("ECI PosVector[%i] = %f\n", i, posVector[i]);
	}

	
	return 0;
}

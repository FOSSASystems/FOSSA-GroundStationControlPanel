/***************************************************************************
 *   Copyright (C) 2006 by j. l. Canales   *
 *   jlcanales@users.sourceforge.net   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "gSatTEME.hpp"
#include "gTime.hpp"
#include "gVector.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void printlog(double minFromEpoch, gVector Position, gVector Vel, gTime calEpoch){

	int   year;
	int   mon;
	int   day;
	int   hr;
	int   min;
	double sec;

	invjday( calEpoch.getGmtTm(), year,mon,day,hr,min, sec );


    printf(" %16.8f %16.8f %16.8f %16.8f %12.9f %12.9f %12.9f %5i%3i%3i %2i:%2i:%9.6f\n",
    		minFromEpoch, Position[0],Position[1],Position[2],Vel[0],Vel[1],Vel[2],year,mon,day,hr,min,sec );

}



int main(int argc, char* argv[]){

	gSatTEME *pSatellite;
	gVector Position;
	gVector Vel;
	gTime   Epoch, calEpoch;
	gTimeSpan tsince;
	char pstrTleLine1[130]="1 00005U 58002B   00179.78495062  .00000023  00000-0  28098-4 0  4753";
	char pstrTleLine2[130]="2 00005  34.2682 348.7242 1859667 331.7664  19.3264 10.82419157413667";



 	std::cout << "Satellite SGP4 calculation Example\n";

	std::cout<<"comienzo muestra"<<std::endl;


	std::cout<<"00005 xx"<<std::endl;
	pSatellite = new gSatTEME("TEST1", pstrTleLine1, pstrTleLine2);
	Epoch.setTime( 2000, 179.78495062);

	for( int minFromEpoch = 0; minFromEpoch <= 4320; minFromEpoch+=360){
		tsince   = (double)minFromEpoch/KMIN_PER_DAY;
		calEpoch =Epoch + tsince;

		pSatellite->setEpoch( calEpoch);
		Position = pSatellite->getPos();
		Vel      = pSatellite->getVel();
		printlog(minFromEpoch, Position, Vel, calEpoch);

	}
	delete pSatellite;


	strcpy(pstrTleLine1, "1 04632U 70093B   04031.91070959 -.00000084  00000-0  10000-3 0  9955");
	strcpy(pstrTleLine2, "2 04632  11.4628 273.1101 1450506 207.6000 143.9350  1.20231981 44145");

	std::cout<<"04632 xx"<<std::endl;
	pSatellite = new gSatTEME("TEST1", pstrTleLine1, pstrTleLine2);
	Epoch.setTime( 2004, 31.91070959);

	pSatellite->setEpoch( Epoch);
	Position = pSatellite->getPos();
	Vel      = pSatellite->getVel();
	printlog(0, Position, Vel, calEpoch);


	for( int minFromEpoch = -5184; minFromEpoch <= -4896; minFromEpoch+=120){
		tsince   = (double)minFromEpoch/KMIN_PER_DAY;
		calEpoch =Epoch + tsince;

		pSatellite->setEpoch( calEpoch);
		Position = pSatellite->getPos();
		Vel      = pSatellite->getVel();
		printlog(minFromEpoch, Position, Vel, calEpoch);

	}

	tsince   = 4896.0/KMIN_PER_DAY;
	calEpoch =Epoch - tsince;

	pSatellite->setEpoch( calEpoch);
	Position = pSatellite->getPos();
	Vel      = pSatellite->getVel();
	printlog(-4896, Position, Vel, calEpoch);


	delete pSatellite;



}


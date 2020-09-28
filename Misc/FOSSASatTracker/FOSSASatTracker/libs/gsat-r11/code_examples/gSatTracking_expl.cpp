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
#include "gObserver.hpp"
#include "gTime.hpp"
#include "gVector.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>




int main(int argc, char* argv[]){

	gSatTEME *pSatellite;
	gObserver observer;
	gTime	  currentTime;
	struct tm *timeinfo;
	time_t rawtime;

	gVector Position;
	gVector Vel;
	gVector LatLong;
	gVector azElPos;
	char pstrTleLine1[130]="1 25544U 98067A   10357.88699256  .00020709  00000-0  15829-3 0  5137";
	char pstrTleLine2[130]="2 25544  51.6489 339.5126 0005277  66.0764  17.4680 15.72128206693316";

 	std::cout << "Satellite SGP4 calculation Example\n";


	pSatellite = new gSatTEME("ISS", pstrTleLine1, pstrTleLine2);
	observer.setPosition( 40.43, -3.683, 700);

	while(1){
		int a=system("clear");

		currentTime=gTime::getCurrentTime();
		rawtime = currentTime.toTime();


		pSatellite->setEpoch( currentTime);
		Position = pSatellite->getPos();
		Vel      = pSatellite->getVel();
		LatLong  = pSatellite->getSubPoint();

		std::cout<<std::endl<<"Time data"<<std::endl;

        timeinfo = localtime ( &rawtime );
		std::cout<<"Local Time: "<<std::fixed<<asctime ( timeinfo );
        timeinfo = gmtime ( &rawtime );
		std::cout<<"Gmt   Time: "<<std::fixed<<asctime ( timeinfo )<<std::endl;


		std::cout.setf ( std::ios_base::right, std::ios_base::floatfield);
		std::cout.width( 16 );
		std::cout<<std::endl<<"TEME Position"<<std::endl;
		std::cout<<std::setiosflags(std::ios::fixed) <<std::setprecision(8)
				 <<"X: "<<Position[0]<<" Y: "<<Position[1]<<" Z:  "<<Position[2]<<std::endl;

		std::cout.setf ( std::ios_base::right, std::ios_base::floatfield);
		std::cout.width( 16 );
		std::cout<<std::endl<<"TEME Velocity"<<std::endl;
		std::cout<<std::setiosflags(std::ios::fixed) <<std::setprecision(9)
				<<"X: "<<Vel[0]<<" Y: "<<Vel[1]<<" Z: "<<Vel[2]<<std::endl;
		
		std::cout<<std::endl;
        std::cout<<"Latitude:  "<<std::setprecision(8)<<LatLong[0]<<std::endl;
		std::cout<<"Longitude: "<<std::setprecision(8)<<LatLong[1]<<std::endl;
		std::cout<<"Altitude:  "<<std::setprecision(8)<<LatLong[2]<<std::endl;



		azElPos = observer.calculateLook( *pSatellite, currentTime);
        std::cout<<std::endl<<"Geodetic Reference System"<<std::endl;
        std::cout<<"Azimuth: "<<std::fixed<<std::setw(12)<<azElPos[ AZIMUTH]/KDEG2RAD<<"     ";
        std::cout<<"Elevation: "<<std::fixed<<std::setw(12)<<azElPos[ ELEVATION]/KDEG2RAD<<"     ";
        std::cout<<"Slant Range: "<<std::fixed<<std::setw(12)<<azElPos[ RANGE]<<std::endl;

        sleep(1);
	}

	delete pSatellite;

}

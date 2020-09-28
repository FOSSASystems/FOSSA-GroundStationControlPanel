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




int main(int argc, char* argv[]){

	gSatTEME *pSatellite;
	gVector Position;
	gVector Vel;
	gTime Epoch;
	char pstrTleLine1[130]="1 00005U 58002B   00179.78495062  .00000023  00000-0  28098-4 0  4753";
	char pstrTleLine2[130]="2 00005  34.2682 348.7242 1859667 331.7664  19.3264 10.82419157413667";

	Epoch.setTime(2000, 179.78495062);

 	std::cout << "Satellite SGP4 calculation Example\n";

	std::cout<<"comienzo muestra"<<std::endl;



	pSatellite = new gSatTEME("TEST1", pstrTleLine1, pstrTleLine2);

	pSatellite->setEpoch( Epoch);
		Position = pSatellite->getPos();
		Vel      = pSatellite->getVel();
		std::cout.setf ( std::ios_base::right, std::ios_base::floatfield);
		std::cout.width( 16 );
		std::cout<<std::setiosflags(std::ios::fixed) <<std::setprecision(8)
		         //<<(double)minFromEpoch
		         <<"   "<<Position[0]<<"   "<<Position[1]<<"   "<<Position[2]
		         <<std::setprecision(9)<<"   "<<Vel[0]<<"   "<<Vel[1]<<"   "<<Vel[2]<<std::endl;



		gTimeSpan timeSpan( (double)(360.0 / KMIN_PER_DAY));
		gTime  diffEpoch = Epoch + timeSpan;

		std::cout<<"Sonda timeSpan "<<timeSpan.getDblDays()<<std::endl;
		std::cout<<"Sonda DiffEPoch "<<diffEpoch.getGmtTm()<<std::endl;

		pSatellite->setEpoch( diffEpoch);
		Position = pSatellite->getPos();
			Vel      = pSatellite->getVel();
			std::cout.setf ( std::ios_base::right, std::ios_base::floatfield);
			std::cout.width( 16 );
			std::cout<<std::setiosflags(std::ios::fixed) <<std::setprecision(8)
			         //<<(double)minFromEpoch
			         <<"   "<<Position[0]<<"   "<<Position[1]<<"   "<<Position[2]
			         <<std::setprecision(9)<<"   "<<Vel[0]<<"   "<<Vel[1]<<"   "<<Vel[2]<<std::endl;




	delete pSatellite;

}

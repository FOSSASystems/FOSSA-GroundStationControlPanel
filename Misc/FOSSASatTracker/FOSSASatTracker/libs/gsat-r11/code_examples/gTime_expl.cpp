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

#include "gTime.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>



int main(int argc, char* argv[]){

	  time_t rawtime, testtime;
	  struct tm * timeinfo;

	  time ( &rawtime );

	  timeinfo = localtime ( &rawtime );
	  printf ( "The local date/time is: %s", asctime (timeinfo) );

	  timeinfo = gmtime ( &rawtime );
	  printf ( "The GMT date/time is: %s", asctime (timeinfo) );

	gTime Time(*timeinfo);

	testtime = Time.toTime();
	timeinfo = gmtime ( &testtime );
	printf ( "The tested date/time is: %s", asctime (timeinfo) );



	Time = gTime::getCurrentTime();
	testtime = Time.toTime();
	timeinfo = gmtime ( &testtime );
	printf ( "The New tested date/time is: %s", asctime (timeinfo) );


 	std::cout << "Keplerian Data file parsing example\n";

	std::cout<<"comienzo muestra";


}

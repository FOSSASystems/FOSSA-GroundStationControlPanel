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

#include "gTleParser.hpp"
#include "gTleFileAccessor.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>



int main(int argc, char* argv[]){

	GTleFileAccessor objKeplerianMap;

 	std::cout << "Keplerian Data file parsing example\n";

 	objKeplerianMap.Parse("/home/PH03696/workingfolder/gsat-trunk/code_examples/SGP4-test.TLE");
 	//objKeplerianMap.Parse("/home/cdeveloper/workspace/gsat-trunk/code_examples/SGP4-test.TLE");
 	std::cout<<"comienzo muestra";
 	std::cout<<objKeplerianMap;

}

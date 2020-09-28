/**********************************************************************
** Name: gTleFileAccessor.cpp
** 
** $Date$
** $Revision$
** $HeadURL$
**
** Description: gTleFileAccessor class is a TLE files parser that
**			     extract the satellites keplerian data to generate
** 				 a satellite list.
**   			
/**********************************************************************/

/***************************************************************************
 *   Copyright (C) 2004 by J.L. Canales                                    *
 *   ph03696@homeserver                                                    *
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

#include <fstream>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
// GKepFile
#include "gTleFileAccessor.hpp"


using namespace std;


void GTleFileAccessor::Parse(std::string ai_FileName){
	
	char 			szNameBuffer[71];
	char 			szNasaLine1Buffer[130];
	char 			szNasaLine2Buffer[130];
	KeplerianData_t KepData;
	string 			sSatName;
	ifstream 		isFile( ai_FileName.c_str());
	assert(isFile); //TODO change this by exeption management


	do{	
		isFile.getline(szNameBuffer,71);
		if(!isFile.eof()){
			sSatName=szNameBuffer;
			isFile.getline(szNasaLine1Buffer,130);
			if( szNasaLine1Buffer[0]=='1' && !isFile.eof()){
				isFile.getline(szNasaLine2Buffer,130);
				if( szNasaLine2Buffer[0]=='2')
					KepData=ParseTleData(szNameBuffer, szNasaLine1Buffer, szNasaLine2Buffer, 'c');
			}
		}

		
		operator[]( KepData.IntDesign)= KepData;
	
		
	}while( !isFile.eof());	
	
	isFile.close();	
}

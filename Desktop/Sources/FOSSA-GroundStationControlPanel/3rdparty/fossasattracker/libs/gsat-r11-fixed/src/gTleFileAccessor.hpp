/**********************************************************************
** Name: GKeple.h
**
** $Date$
** $Revision$
** $HeadURL$
**
** Description:
**
**********************************************************************/



/***************************************************************************
 *   Copyright (C) 2004 by JL Canales                                      *
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


#ifndef GKEPFILE_H
#define GKEPFILE_H 1

#include <string>
#include <map>
#include <iostream> // for operator<<(), see below
#include "gTleParser.hpp"
#include "gTime.hpp"  // for operator <<() MeassurementDate.
#include "stdsat.h"

typedef std::map<std::string,KeplerianData_t> KeplerianMap;


class GTleFileAccessor:public KeplerianMap{
	
//Constructor

	public:

		
//operations

	public:
		void Parse( std::string ai_FileName);

};


inline std::ostream& operator<<(std::ostream& s, GTleFileAccessor& ai_GKepFile ) {
	
	GTleFileAccessor::iterator pos ;
	
	int satnum=0;
	for(pos = ai_GKepFile.begin();pos != ai_GKepFile.end();++pos){
		satnum++;

		s << "Inter. Design: "<< (*pos).first<<"-"<< (*pos).second.SatName<< std::endl<< std::endl;
	
		s << "N Catalogo:   "<<  (*pos).second.SatNum<< std::endl;
		s << "Inter. Design: "<< (*pos).second.IntDesign<< std::endl;
		s << "Epoch:         "<< (*pos).second.MessurementDate<< std::endl;
		s << "Decay:         "<< (*pos).second.ndot*(XPDOTP*1440.0)<< std::endl;
		s << "DecayVar:      "<< (*pos).second.nddot*(XPDOTP*1440.0*1440)<< std::endl;
		s << "PressureCoef:  "<< (*pos).second.bstar<< std::endl;
		s << "EphemType:     "<< (*pos).second.EphemType<< std::endl;
		s << "ElsetNum:      "<< (*pos).second.ElsetNum<< std::endl;
	
		s << "Inclination:   "<< (*pos).second.Inclination<< std::endl;
		s << "RAAN:          "<< (*pos).second.RAAN<< std::endl;
		s << "Eccentricity:  "<< (*pos).second.Eccentricity<< std::endl;
		s << "ArgPerigee:    "<< (*pos).second.ArgPerigee<< std::endl;
		s << "MeanAnomaly:   "<< (*pos).second.MeanAnomaly<< std::endl;
		s << "MeanMotion:    "<< (*pos).second.MeanMotion<< std::endl;
		s << "OrbitNum:      "<< (*pos).second.OrbitNum<< std::endl;
					
		s << std::endl<<std::endl;			
	}
	
	s << std::endl << ai_GKepFile.size() <<" : ";	
	
	
	s << std::endl;
	return s;
	}




#endif

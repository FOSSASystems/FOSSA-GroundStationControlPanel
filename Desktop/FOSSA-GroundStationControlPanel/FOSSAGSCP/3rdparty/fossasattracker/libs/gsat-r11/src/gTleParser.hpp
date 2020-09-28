/**********************************************************************
** Name: gTleParser.hpp
**
** $Date: 2007-03-15 10:53:33 +0100 (Thu, 15 Mar 2007) $
** $Revision: 7 $
** $HeadURL: http://gsat.svn.sourceforge.net/svnroot/gsat/tags/gsat-0.1.0/src/GKepfile.h $
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


#ifndef GTLEPARSER_H
#define GTLEPARSER_H 1

#include <string>
#include <map>
#include <iostream> // for operator<<(), see below
#include "gTime.hpp"

typedef struct tagKeplerianData
{
	long int SatNum;
	char	Classification;
	std::string IntDesign;
	gTime	MessurementDate;	// Keplerian data Messurement Date
	double 	ndot; //Ballistic Coefficient
	double 	nddot; //Ballistic Coefficient Derivative
	double 	bstar; //BSTAR drag 
	int 	EphemType;
	long 	ElsetNum;
	double	Inclination;	//Radians
	double 	RAAN;			//Radians
	double	Eccentricity;
	double	ArgPerigee;		//Radians
	double	MeanAnomaly;    //Radians
	double 	MeanMotion;		//Radians per minute
	long 	OrbitNum;
	std::string	SatName;				// Satellite Name


}KeplerianData_t;

KeplerianData_t ParseTleData(const char *pstrName, 
			     char *pstrTleLine1, char *pstrTleLine2,
			     char ai_typerun);
#endif

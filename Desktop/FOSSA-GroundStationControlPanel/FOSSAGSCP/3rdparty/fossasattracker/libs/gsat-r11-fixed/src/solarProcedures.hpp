/***************************************************************************
 * Name: solarProcedures.hpp
 *
 * Description: Solar System calculation procedures.
 *              Define some procedures needed for satellite functionality
 *
 *
 ***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2004 by J. L. Canales                                   *
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

#ifndef _SOLARPROCEDURES_HPP_
#define _SOLARPROCEDURES_HPP_ 1

#include "gTime.hpp"
#include "gVector.hpp"

	// Operation sunECIPosition
	//! @brief This operation compute the sun TEME (True Equator Main Equinox)
	//! coordinates for the ai_epoch time
	//! @details
	//! References:
	//!  Fundamentals of Astrodynamics and Applications
	//!   David A. Vallado
	//!   Chapter 5.1 Solar Phenomena
	//! @param[in] ai_epoch  GTime. Epoch for the ECI reference system calculation
	//! @param[out] ao_position gVector Sun position vector

gVector computeSolarPosition( gTime ai_epoch);

#endif // _SOLARPROCEDURES_HPP_

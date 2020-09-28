/*
 * gTime_Test.hpp
 *
 *  Created on: 27/07/2010
 *      Author: cdeveloper
 */

#ifndef GSATVISIBILITY_TEST_HPP_
#define GSATVISIBILITY_TEST_HPP_

#include <cppunit/extensions/HelperMacros.h>
#include <string.h>
#include <math.h>

#include "gTime.hpp"
#include "stdsat.h"
#include "solarProcedures.hpp"


#include <iostream>
#include <iomanip>


class gSatVisibility_Test: public CppUnit::TestFixture{
CPPUNIT_TEST_SUITE(gSatVisibility_Test);
CPPUNIT_TEST(testSunECIPos);
CPPUNIT_TEST_SUITE_END();

private:

public:
	void setUp(){
		  ;
	}

	void tearDown(){
	}

	void testSunECIPos(){
		gTime epoch(2453827.5);
		gVector posVector;
		posVector = computeSolarPosition( epoch);

		CPPUNIT_ASSERT_DOUBLES_EQUAL( posVector[0], 146186178.7, 0.1);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( posVector[1], 28789122.5, 0.1);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( posVector[2], 12481127.0, 0.1);
	}
};



#endif /* GSATEME_TEST_HPP_ */

/*
 * gTime_Test.hpp
 *
 *  Created on: 27/07/2010
 *      Author: cdeveloper
 */

#ifndef GOBSERVER_TEST_HPP_
#define GOBSERVER_TEST_HPP_

#include <cppunit/extensions/HelperMacros.h>
#include <string.h>
#include <math.h>

#include "gTime.hpp"
#include "stdsat.h"
#include "gObserver.hpp"


#include <iostream>
#include <iomanip>


class gObserver_Test: public CppUnit::TestFixture{
CPPUNIT_TEST_SUITE(gObserver_Test);
CPPUNIT_TEST(testECIPos);
CPPUNIT_TEST_SUITE_END();

private:
	gObserver site;

public:
	void setUp(){
		  site.setPosition( 40, -75, 0);
	}

	void tearDown(){
	}

	void testECIPos(){
		gTime epoch(2449991.875);
		gVector posVector, velVector;
		site.getECIPosition( epoch, posVector, velVector);

		CPPUNIT_ASSERT_DOUBLES_EQUAL( posVector[0], 1703.295, 0.001);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( posVector[1], 4586.649, 0.001);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( posVector[2], 4077.984, 0.001);
	}
};



#endif /* GSATEME_TEST_HPP_ */

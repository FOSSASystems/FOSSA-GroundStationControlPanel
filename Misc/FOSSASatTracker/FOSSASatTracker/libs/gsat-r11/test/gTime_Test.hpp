/*
 * gTime_Test.hpp
 *
 *  Created on: 27/07/2010
 *      Author: cdeveloper
 */

#ifndef GTIME_TEST_HPP_
#define GTIME_TEST_HPP_

#include <cppunit/extensions/HelperMacros.h>
#include <string.h>
#include <math.h>

#include "gTime.hpp"
#include "stdsat.h"


#include <iostream>
#include <iomanip>


class gTime_Test: public CppUnit::TestFixture{
CPPUNIT_TEST_SUITE(gTime_Test);
CPPUNIT_TEST(testGMST);
CPPUNIT_TEST(testConstructor01);
CPPUNIT_TEST(testSetTime);
CPPUNIT_TEST_SUITE_END();

private:
	gTime Time;

public:
	void setUp(){
		  Time=( 2449991.875);

	}

	void tearDown(){
	}
	 inline std::string stringify(double x)
	 {
	   std::ostringstream o;
	   o << x;
	   return o.str();
	 }

	void testGMST(){
		gTimeSpan Delay(1,0,0,0);


		CPPUNIT_ASSERT_DOUBLES_EQUAL( Time.toThetaGMST(),2.524218, 0.00001);

	}

	void testConstructor01(){
		  time_t rawtime, testtime;
		  struct tm * timeinfo;

		  time ( &rawtime );
		  timeinfo = gmtime ( &rawtime );

		gTime Time(*timeinfo);

		testtime = Time.toTime();

		CPPUNIT_ASSERT(rawtime == testtime);

	}

	void testSetTime(){
		gTime  Time;
		double jdTime;

		Time.setTime(1995,0);
		jdTime=Time.getGmtTm();

		CPPUNIT_ASSERT_DOUBLES_EQUAL(2449717.5, jdTime, 0.1);

	}
};



#endif /* GSATEME_TEST_HPP_ */

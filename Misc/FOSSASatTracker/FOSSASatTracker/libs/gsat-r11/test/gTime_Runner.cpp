/*
 * gSatTEME_Runner.cpp
 *
 *  Created on: 27/07/2010
 *      Author: cdeveloper
 */


#include <cppunit/ui/text/TestRunner.h>
#include "gTime_Test.hpp"


int main( int argc, char**argv){

	CppUnit::TextUi::TestRunner runner;
	runner.addTest(gTime_Test::suite());

	runner.run();
	return 0;
}

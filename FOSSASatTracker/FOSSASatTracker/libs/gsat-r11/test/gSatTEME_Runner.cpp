/*
 * gSatTEME_Runner.cpp
 *
 *  Created on: 27/07/2010
 *      Author: cdeveloper
 */


#include <cppunit/ui/text/TestRunner.h>
#include "gSatTEME0005_Test.hpp"


int main( int argc, char**argv){

	CppUnit::TextUi::TestRunner runner;
	runner.addTest(gSatTEME0005_Test::suite());

	runner.run();
	return 0;
}

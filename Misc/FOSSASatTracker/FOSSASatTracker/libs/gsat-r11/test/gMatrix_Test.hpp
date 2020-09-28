/*
 * gTime_Test.hpp
 *
 *  Created on: 27/07/2010
 *      Author: cdeveloper
 */

#ifndef GMATRIX_TEST_HPP_
#define GMATRIX_TEST_HPP_

#include <cppunit/extensions/HelperMacros.h>
#include <string.h>
#include <math.h>

#include "gMatrix.hpp"
#include "gVector.hpp"


#include <iostream>
#include <iomanip>


class gMatrix_Test: public CppUnit::TestFixture{
CPPUNIT_TEST_SUITE(gMatrix_Test);
CPPUNIT_TEST(testVectProduct);
CPPUNIT_TEST_SUITE_END();

private:
	gMatrix testMatrix;

public:
	void setUp(){
		  testMatrix.SetSize(3,3);
		  testMatrix[0][0]=1;
		  testMatrix[0][1]=0;
		  testMatrix[0][2]=0;
		  testMatrix[1][0]=0;
		  testMatrix[1][1]=1;
		  testMatrix[1][2]=0;
		  testMatrix[2][0]=0;
		  testMatrix[2][1]=0;
		  testMatrix[2][2]=1;
	}

	void tearDown(){
	}

	void testVectProduct(){
		gVector testVector(3), resultVector(3);
		testVector[0]=4;
		testVector[1]=5;
		testVector[2]=6;
		resultVector = testMatrix*testVector;

		CPPUNIT_ASSERT( testVector==resultVector);

	}
	
};



#endif /* GSATEME_TEST_HPP_ */

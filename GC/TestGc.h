/*
 * TestGc.h
 *
 *  Created on: 08.12.2013
 *      Author: christoph
 */

#ifndef TESTGC_H_
#define TESTGC_H_

#include <string>
#include <list>
#include <stdint.h>
#include <iostream>
#include "TypeDescriptor.h"
#include "Heap.h"
#include "AppClasses.h"

#define LECTURE_AMT 17

class TestGc {
private:
	Heap* h;
	uint64_t runs;
	uint64_t errors;
	void testGc1();
	void testGc2();
	void testGc3();
	void testGc4();
	void testGc5();
	void setUp();
public:
	uint64_t run();
};

#endif /* TESTGC_H_ */

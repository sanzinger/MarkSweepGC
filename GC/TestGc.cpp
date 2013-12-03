/*
 * TestGc.cpp
 *
 *  Created on: Nov 16, 2013
 *      Author: stefan
 */

#include "Heap.h"
#include "AppClasses.h"

int main(int argc, char** argv) {

	Heap* h = new Heap();
	StudentList::registerMe(h);
	StudNode::registerMe(h);
	LectNode::registerMe(h);
	Student::registerMe(h);
	Lecture::registerMe(h);

	h->alloc("Student");

	return 0;
}


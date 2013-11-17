/*
 * TestGc.cpp
 *
 *  Created on: Nov 16, 2013
 *      Author: stefan
 */

#include "Heap.h"

int main(int argc, char** argv) {

	Heap* h = new Heap();
	h->alloc("HELLO");
	//h->gc(void);
	h->registerx("hallofu", &h);
	return 0;
}


/*
 * Heap.h
 *
 *  Created on: Nov 16, 2013
 *      Author: stefan
 */

#ifndef HEAP_H_
#define HEAP_H_
#include <string>
using namespace std;

class Heap {
public:
	Heap();
	virtual ~Heap();
	void alloc(string typeId);
	void gc();
	void registerx(string typeId, void* clazz);
};

#endif /* HEAP_H_ */

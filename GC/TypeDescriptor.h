/*
 * TypeDescriptor.h
 *
 *  Created on: Dec 3, 2013
 *      Author: stefan
 */

#ifndef TYPEDESCRIPTOR_H_
#define TYPEDESCRIPTOR_H_

#include <string>
#include <stdint.h>

using namespace std;

class TypeDescriptor {
private:
	uint8_t desc[128];
	string name;
	uint64_t *descPosition;
	void addToObjectSize(uint64_t length);
	uint64_t getObjectSize();
public:
	TypeDescriptor(string name);
	void addPointer(string name);
	void addInteger(string name);
	void addString(string name, uint64_t length);
	void typeCompleted();
	void* getDescriptor();
	string getName();
};

#endif /* TYPEDESCRIPTOR_H_ */

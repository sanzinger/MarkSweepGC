/*
 * AppClasses.h
 *
 *  Created on: Dec 3, 2013
 *      Author: stefan
 */

#ifndef APPCLASSES_H_
#define APPCLASSES_H_

#include "Heap.h"

class Lecture {
private:
	uint64_t id;
	char name[255];
	uint64_t semester;
public:
	static void registerMe(Heap *h);
};

class Student {
private:
	uint64_t id;
	char name[255];
	Lecture *lect;
public:
	static void registerMe(Heap *h);
};

class StudNode {
private:
	StudNode *next;
	Student *stud;
public:
	static void registerMe(Heap *h);
};

class StudentList {
private:
	StudNode *first;
public:
	static void registerMe(Heap *h);
};

class LectNode {
private:
	LectNode *next;
	Lecture *lect;
public:
	static void registerMe(Heap *h);
};



#endif /* APPCLASSES_H_ */

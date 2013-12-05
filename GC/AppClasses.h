/*
 * AppClasses.h
 *
 *  Created on: Dec 3, 2013
 *      Author: stefan
 */

#ifndef APPCLASSES_H_
#define APPCLASSES_H_

#include "Heap.h"

class AppHeap {
private:
	static Heap* h;
public:
	static Heap* getInstance();
};

class Object {
};

class Lecture : Object {
private:
public:
	static string TYPE_NAME;
	uint64_t id;
	char name[255];
	uint64_t semester;
	static void registerMe(Heap *h);
};

class Student : Object {
private:
public:
	static string TYPE_NAME;
	uint64_t id;
	char name[255];
	Lecture *lect = NULL;
	static void registerMe(Heap *h);
	void add(Lecture *l);
	void remove(Lecture *l);
};

class StudNode : Object {
private:
	StudNode *next = NULL;
	Student *stud = NULL;
public:
	static string TYPE_NAME;
	static void registerMe(Heap *h);
};

class StudentList : Object {
private:
	StudNode *first = NULL;
public:
	static string TYPE_NAME;
	static void registerMe(Heap *h);
	void add(Student *s);
	void remove(Student *s);
};

class LectNode : Object {
private:
	LectNode *next;
	Lecture *lect;
public:
	static string TYPE_NAME;
	static void registerMe(Heap *h);
};



#endif /* APPCLASSES_H_ */

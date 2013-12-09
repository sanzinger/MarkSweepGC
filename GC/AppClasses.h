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

class LectNode;

class Lecture {
private:
public:
	static string TYPE_NAME;
	uint64_t id;
	char name[255];
	uint64_t semester;
	static void registerMe(Heap* h);
};

class Student {
private:
public:
	static string TYPE_NAME;
	uint64_t id;
	char name[255];
	LectNode* lect;
	Student();
	static void registerMe(Heap* h);
	void add(Lecture *l);
	void remove(Lecture *l);
	string toString();
};

class StudNode {
public:
	StudNode();
	Student *stud;
	StudNode *next;
	static string TYPE_NAME;
	static void registerMe(Heap* h);
};

class StudentList {
private:
public:
	StudNode *first;
	StudentList();
	static string TYPE_NAME;
	static void registerMe(Heap* h);
	void add(Student *s);
	void remove(Student *s);
	string toString();
};

class LectNode {
private:
public:
	LectNode();
	Lecture *lect;
	LectNode *next;
	static string TYPE_NAME;
	static void registerMe(Heap* h);
};

#endif /* APPCLASSES_H_ */

/*
 * TestGc.cpp
 *
 *  Created on: Nov 16, 2013
 *      Author: stefan
 */

#include "TestGc.h"

#include <cstdlib>
#include <cstring>

int main(int argc, char** argv) {
	TestGc *testGc = new TestGc();
	testGc->run();
	return 0;
}

uint64_t TestGc::run() {
	runs = 0;
	errors = 0;

	testGc4();

	return errors;
}

void TestGc::setUp() {
	h = AppHeap::getInstance();
	StudentList::registerMe(h);
	StudNode::registerMe(h);
	LectNode::registerMe(h);
	Student::registerMe(h);
	Lecture::registerMe(h);
}

void TestGc::testGc5() {
	runs++;
	setUp();
	StudNode* sn1 = new (h->alloc(StudNode::TYPE_NAME)) StudNode();
	h->gc();
	h->dumpHeap();
}

void TestGc::testGc4() {
	runs++;
	setUp();
	StudNode* sn1 = new (h->alloc(StudNode::TYPE_NAME)) StudNode();
	StudNode* sn2 = new (h->alloc(StudNode::TYPE_NAME)) StudNode();
	StudNode* sn3 = new (h->alloc(StudNode::TYPE_NAME)) StudNode();
	StudNode* sn4 = new (h->alloc(StudNode::TYPE_NAME)) StudNode();
	h->addRoot(sn4);
	h->gc();
	h->dumpHeap();
	StudentList* sl = new (h->alloc(StudentList::TYPE_NAME)) StudentList();
	StudentList* sl2 = new (h->alloc(StudentList::TYPE_NAME)) StudentList();
	StudentList* sl3 = new (h->alloc(StudentList::TYPE_NAME)) StudentList();
	StudentList* sl4 = new (h->alloc(StudentList::TYPE_NAME)) StudentList();
	h->dumpHeap();
}

void TestGc::testGc3() {
	runs++;
	setUp();
	StudentList* sl = new (h->alloc(StudentList::TYPE_NAME)) StudentList();
	new (h->alloc(StudNode::TYPE_NAME)) StudNode();
	StudNode* sn2 = new (h->alloc(StudNode::TYPE_NAME)) StudNode();
	sl->first = sn2;
	h->addRoot(sl);
	h->gc();
	h->dumpHeap();
	h->removeRoot(sl);
	h->gc();
	h->dumpHeap();
}
void TestGc::testGc2() {
	runs++;
	setUp();
	cout << "Test GC-2" << endl;
	cout << "Heap free bytes: " << h->getFreeBytes() << endl;
	StudentList* sl = new (h->alloc(StudentList::TYPE_NAME)) StudentList();
	int64_t freeBefore = (int64_t)h->getFreeBytes();
	StudNode* sn2 = new (h->alloc(StudNode::TYPE_NAME)) StudNode();
	StudNode* sn = new (h->alloc(StudNode::TYPE_NAME)) StudNode();
	//sl->first = sn;
	h->addRoot(sl);
	//sl->first = NULL;
	h->gc();
	h->dumpHeap();
	int64_t freeAfter = (int64_t)h->getFreeBytes();
	if(freeBefore != h->getFreeBytes()) {
		cout << "ERROR: Free bytes less than before. Changed by " << (freeBefore-freeAfter) << endl;
	}
	cout << "Heap free bytes: " << h->getFreeBytes() << endl;
}

void TestGc::testGc1() {
	runs++;

	cout << "Test GC-1" << endl;

	Heap* h = AppHeap::getInstance();
	StudentList::registerMe(h);
	StudNode::registerMe(h);
	LectNode::registerMe(h);
	Student::registerMe(h);
	Lecture::registerMe(h);
	uint64_t bytesBefore = h->getFreeBytes();
	cout << "Heap free bytes: " << h->getFreeBytes() << endl;
	StudentList* sl = new (h->alloc(StudentList::TYPE_NAME)) StudentList();
	h->addRoot(sl);

	Lecture* lectures[LECTURE_AMT];
	int i = 0;
	for(i=0; i<LECTURE_AMT; i++) {
		Lecture* l = new (h->alloc(Lecture::TYPE_NAME)) Lecture();
		strcpy(l->name, "Lecture ______________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________");
		l->id = i;
		l->semester = 3;
		lectures[i] = l;
		h->addRoot(l);
	}
	srand(1);
	for(i=0; i<50; i++) {
		Student* s = new (h->alloc(Student::TYPE_NAME)) Student();
		strcpy(s->name, "Student ______________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________");
		s->id = i;
		sl->add(s);
		int addLectures  = rand()%4;
		int j = 0;
		for(j=0; j<addLectures; j++) {
			s->add(lectures[j%LECTURE_AMT]);
		}
		for(j=0; j<LECTURE_AMT; j++) {
			s->remove(lectures[rand()%LECTURE_AMT]);
		}
		StudNode* sn = sl->first;
		while(rand()%100 < 80 && sn != NULL) {
			sn = sn->next;
		}
		if(sn != NULL) {
			sl->remove(sn->stud);
		}
	}
	h->dumpHeap();
	h->gc();
	h->dumpHeap();
	//cout << sl->toString() << endl;
	//cout << "FreeBytes: " << h->getFreeBytes() << endl;
	//cout << "Allocated: " << h->allocated << " freed: " << h->freed << " merged: " << h->merged << " gc'd: " << h->gcd << endl;
	h->removeRoot(sl);
	for(i=0; i<LECTURE_AMT; i++) {
		h->removeRoot(lectures[i]);
	}
	h->gc();
	if(bytesBefore != h->getFreeBytes()) {
		cout << "ERROR: Heap is not free before=" << bytesBefore << " after=" << h->getFreeBytes() << endl;
	}
	cout << "End" << endl;
}


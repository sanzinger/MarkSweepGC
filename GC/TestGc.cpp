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

	testGc1();

	return errors;
}

void TestGc::testGc2() {
	runs++;

	cout << "Test GC-2" << endl;

	Heap* h = AppHeap::getInstance();
	StudentList::registerMe(h);
	StudNode::registerMe(h);
	LectNode::registerMe(h);
	Student::registerMe(h);
	Lecture::registerMe(h);
	cout << "Heap free bytes: " << h->getFreeBytes() << endl;
	StudentList* sl = new (h->alloc(StudentList::TYPE_NAME)) StudentList();
	int64_t freeBefore = (int64_t)h->getFreeBytes();
	StudNode* sn = new (h->alloc(StudNode::TYPE_NAME)) StudNode();
	sl->first = sn;
	h->addRoot((uint64_t*)sl);
	sl->first = NULL;
	h->dumpHeap();
	h->gc();
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
	cout << "Heap free bytes: " << h->getFreeBytes() << endl;
	StudentList* sl = new (h->alloc(StudentList::TYPE_NAME)) StudentList();

	cout << "Heap free bytes: " << h->getFreeBytes() << endl;
	Lecture* lectures[LECTURE_AMT];
	int i = 0;
	for(i=0; i<LECTURE_AMT; i++) {
		Lecture* l = new (h->alloc(Lecture::TYPE_NAME)) Lecture();
		strcpy(l->name, "Lecture ______________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________");
		l->id = i;
		l->semester = 3;
		lectures[i] = l;
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
	}
	StudNode* s = sl->first;
	while(s != NULL) {
		if(rand()%2==0) { // Remove one half
			sl->remove(s->stud);
		}
		s = s->next;
	}
	cout << sl->toString() << endl;
	h->dumpHeap();
	cout << "FreeBytes: " << h->getFreeBytes() << endl;
	cout << "End" << endl;

	h->addRoot((uint64_t*)sl);
	h->gc();
	cout << "FreeBytes: " << h->getFreeBytes() << endl;
}


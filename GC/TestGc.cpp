/*
 * TestGc.cpp
 *
 *  Created on: Nov 16, 2013
 *      Author: stefan
 */

#include "TestGc.h"

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
	cout << "FreeBytes: " << h->getFreeBytes() << endl;
	cout << "End" << endl;

	h->addRoot((uint64_t*)sl);
	h->gc();
}


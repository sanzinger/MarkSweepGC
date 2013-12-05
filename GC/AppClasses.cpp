/*
 * AppClasses.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: stefan
 */

#ifndef APPCLASSES_CPP_
#define APPCLASSES_CPP_

#include "AppClasses.h"

Heap* AppHeap::h = NULL;

Heap* AppHeap::getInstance() {
	if(h == NULL) {
		h = new Heap();
	}
	return h;
}

string StudentList::TYPE_NAME = "StudentList";

void StudentList::registerMe(Heap *h) {
	TypeDescriptor *studentList = new TypeDescriptor("StudentList");
	studentList->addPointer("first");
	studentList->typeCompleted();
	h->registerType(studentList);
}

void StudentList::add(Student *s) {
	StudNode* sn = (StudNode*)AppHeap::getInstance()->alloc(StudNode::TYPE_NAME);
	sn->stud = s;
	if(this->first != NULL) {
		sn->next = this->first;
	}
	this->first = sn;
}

void StudentList::remove(Student *s) {
	StudNode* i = this->first;
	StudNode* prev = NULL;
	while(i != NULL && i->stud != s) {
		prev = i;
		i = i->next;
	}
	if(i != NULL) {
		if(i == this->first) {
			this->first = this->first->next;
		} else {
			prev->next = i->next;
		}
	}
}

string StudNode::TYPE_NAME = "StudNode";

void StudNode::registerMe(Heap *h) {
	TypeDescriptor *studNode = new TypeDescriptor("StudNode");
	studNode->addPointer("next");
	studNode->addPointer("stud");
	studNode->typeCompleted();
	h->registerType(studNode);
}

string LectNode::TYPE_NAME = "LectNode";

void LectNode::registerMe(Heap *h) {
	TypeDescriptor *lectNode = new TypeDescriptor("LectNode");
	lectNode->addPointer("next");
	lectNode->addPointer("lect");
	lectNode->typeCompleted();
	h->registerType(lectNode);
}

string Student::TYPE_NAME = "Student";

void Student::registerMe(Heap *h) {
	TypeDescriptor *student = new TypeDescriptor("Student");
	student->addInteger("id");
	student->addString("name", 255);
	student->addPointer("lect");
	student->typeCompleted();
	h->registerType(student);
}

void Student::add(Lecture *l) {
	LectNode* ln = (LectNode*)AppHeap::getInstance()->alloc(LectNode::TYPE_NAME);
	ln->lect = l;
	if(this->lect != NULL) {
		ln->next = this->lect;
	}
	this->lect = ln;
}

void Student::remove(Lecture *l) {
	LectNode* i = this->lect;
	LectNode* prev = NULL;
	while(i != NULL && i->lect != l) {
		prev = i;
		i = i->next;
	}
	if(i != NULL) {
		if(i == this->lect) {
			this->lect = this->lect->next;
		} else {
			prev->next = i->next;
		}
	}
}

string Lecture::TYPE_NAME = "Lecture";

void Lecture::registerMe(Heap *h) {
	TypeDescriptor *lecture = new TypeDescriptor("Lecture");
	lecture->addInteger("id");
	lecture->addString("name", 255);
	lecture->addInteger("semester");
	lecture->typeCompleted();
	h->registerType(lecture);
}

#endif /* APPCLASSES_CPP_ */

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

}

void StudentList::remove(Student *s) {

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
	if(this->lect == NULL) {

	}
}

void Student::remove(Lecture *l) {

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

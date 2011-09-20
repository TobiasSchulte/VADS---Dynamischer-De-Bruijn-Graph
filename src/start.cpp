/*
 * start.cpp
 *
 *  Created on: 14.07.2011
 *      Author: tobias
 */

#include <iostream>
#include <ctime>

#include "../include/sha1.h"
#include "../include/Subjects1-6.h"

#define NODES 10

#include "Helpers.cpp"

#include "Objects.cpp"

#include "SubjectTypes.h"

#include "SubjectType_List.cpp"
#include "SubjectType_Node.cpp"
#include "SubjectType_Supervisor.cpp"

using namespace std;

int main()
{
  	NumObj* num = new NumObj(10);
  	// start with creating a supervisor, run for (at most) 300 rounds
  	runSubjects(Supervisor,num,6000);
  	sys_pause();
}


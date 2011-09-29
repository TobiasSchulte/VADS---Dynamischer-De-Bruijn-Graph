/*
 * start.cpp
 *
 *  Created on: 14.07.2011
 *      Author: tobias
 */

#include <iostream>
#include <ctime>
#include <string>

#include "../include/sha1.h"
#include "../include/Subjects1-6.h"

#define NODES 10

#define VIRTUAL_NODE_WAKEPUP 10
#define SIMULATION_TIME 2000
#define SUPERVISOR_WAKEUP 100

#define DEBUG_OUTPUT true
#define TUP true
#define TDOWN false

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
  	runSubjects(Supervisor,num,SIMULATION_TIME);
  	//sys_pause();
}


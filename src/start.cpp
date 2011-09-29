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

#define NODES 100

#define VIRTUAL_NODE_WAKEPUP 10
#define SIMULATION_TIME 70000
#define SUPERVISOR_WAKEUP 50000
#define PERIODIC_PROBE_TIME 10

#define SEARCH 0
#define DELETE 1

/*
TESTMODE defines the tests to be done:
SEARCH - Test Search
DELETE - Test Delete
*/
#define TESTMODE DELETE

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


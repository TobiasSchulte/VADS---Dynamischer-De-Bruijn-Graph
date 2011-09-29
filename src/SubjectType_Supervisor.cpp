Action Supervisor::Init(NumObj *numo){
	
	count = 0;
	
	srand(time(0));
	char *str = new char[12];
	
	SHA1 sha;
	unsigned message_digest[5];
	
	for(int i = 0; i < NODES; i++){
		
		sha.Reset();
		itoa(rand(), str);
		sha << str;
		sha.Result(message_digest);
		
		NumObj *tnum = new NumObj(message_digest[0]);
		new(Node, tnum);
	}
	delete str;
	
}

Action Supervisor::SetLink(IDPair *idop){
	

	ListObjects[count] = idop;
	NodeNames[count] = idop->ido1->num;
	count++;
	
	if(count == NODES*3){
	
		
		for(int i = 0; i < NODES*3; i++){
			Nodes[i] = new Relay(ListObjects[i]->ido1->id);
			//delete ListObjects[i]->ido1;

			// periodically wakeup list objects
			NumObj *numo = new NumObj(VIRTUAL_NODE_WAKEPUP);
			Nodes[i]->call(List::Wakeup, numo);
			
			if(i % 3 == 0 && i < NODES*3-3)
				Nodes[i]->call(List::Insert, ListObjects[i+3]->ido2);
			
			std::cout << "Node " << NodeNames[i] << ": on Index " << i << "\n";
		
		}
		
		NumObj *numo = new NumObj(SUPERVISOR_WAKEUP);
		call(Supervisor::Wakeup, numo);
	}
}

Action Supervisor::Wakeup(NumObj *numo){

    if (numo->num > 0) {
		numo->num--;
		call(Supervisor::Wakeup, numo);
    }
    else {
		// test Search
		srand(time(0));
		unsigned sourceIndex = NODES; // (((rand() % NODES)*3)+1);
		unsigned destIndex =  NODES+1; // (((rand() % NODES)*3)+1);
		std::cout << "Starting Search.\nFrom Node Index " << sourceIndex << " to Index " << destIndex << '\n';
		/*RoutingInformation *r = new RoutingInformation(NodeNames[sourceIndex], NodeNames[destIndex] );

		MessageObj *m = new MessageObj("bla", r);
 		Nodes[sourceIndex]->call(List::Search, m);*/
    }
}

Action Supervisor::Init(NumObj *numo){
	
	virtualCount = 0;
	realCount = 0;
	
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
	

	
	//real Node
	if(idop->real){
		RealListObjects[realCount] = idop;
		RealNodeNames[realCount] = idop->ido1->num;
		realCount++;
	}
	//virtual Node
	else{
		VirtualListObjects[virtualCount] = idop;
		VirtualNodeNames[virtualCount] = idop->ido1->num;
		virtualCount++;
	}
	if(virtualCount+realCount == NODES*3){
	
		//Wakeup Real Nodes
		for(int i = 0; i < realCount; i++){
			RealNodes[i] = new Relay(RealListObjects[i]->ido1->id);
			
			// wakeup list objects
			NumObj *numo = new NumObj(VIRTUAL_NODE_WAKEPUP);
			RealNodes[i]->call(List::Wakeup, numo);
			// wakeup probing
			NumObj *pnumo = new NumObj(PERIODIC_PROBE_TIME);
			RealNodes[i]->call(List::StartProbe, pnumo);
			
			std::cout << "Real Node " << RealNodeNames[i] << ": on Index " << i << "\n";
		
			if(i < realCount-1)
				RealNodes[i]->call(List::Insert, RealListObjects[i+1]->ido2);
		}	
		
		//Wakeup Virtual Nodes
		for(int i = 0; i < virtualCount; i++){
			VirtualNodes[i] = new Relay(VirtualListObjects[i]->ido1->id);
			
			// wakeup list objects
			NumObj *numo = new NumObj(VIRTUAL_NODE_WAKEPUP);
			VirtualNodes[i]->call(List::Wakeup, numo);
			
			std::cout << "Virtual Node " << VirtualNodeNames[i] << ": on Index " << i << "\n";
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
		unsigned sourceIndex = 0; // (((rand() % NODES)*3)+1);
		unsigned destIndex =  1; // (((rand() % NODES)*3)+1);
		std::cout << "Starting Search.\nFrom Node Index " << sourceIndex << " to Index " << destIndex << '\n';
		RoutingInformation *r = new RoutingInformation(RealNodeNames[sourceIndex], RealNodeNames[destIndex] );

		MessageObj *m = new MessageObj("bla", r);
 		RealNodes[sourceIndex]->call(List::Search, m);
    }
}

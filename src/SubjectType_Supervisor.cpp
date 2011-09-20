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
	
}

Action Supervisor::SetLink(IDPair *idop){
	
	ListObjects[count] = idop;
	std::cout << "Object inserted. " << count << '\n';
	count++;
	
	
	if(count == NODES*3){
	
		Relay *r;
	
		for(int i = 0; i < NODES*3; i++){
			r = new Relay(ListObjects[i]->ido1->id);
			
			// periodically wakeup list objects
			NumObj *numo = new NumObj(100);
			r->call(List::Wakeup, numo);
			
			if(i < NODES*3-1)
				r->call(List::Insert, ListObjects[i+1]->ido2);
				
			//ListObjects[i] = new IDObj(extractIdentity(r));
			std::cout << "Object inserted. " << i << '\n';
		}
	}
}
Action Supervisor::Wakeup(NumObj *numo){}
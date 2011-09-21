Action List::Init(NumObj *numo)
{
    IDPair *idp;
	
    num = numo->num;
    delete numo;
    left = NULL;
    right = NULL;
    
    real = false;
    
    in = new Relay;
    idp = new IDPair(new IDObj(num, new Identity(in)), new IDObj(num, new Identity(in)));
    parent->call(Node::SetLink, idp);
   
    
    IDObj *tidobj = new IDObj(num, new Identity(in));
    
    parent->call(Node::RegisterChild, tidobj);
    std::cout << "Node " << num << ": Hello World."<< "\n";
} 

Action List::Wakeup(NumObj *numo)
{
    // check whether it's time to activate Debruijn Probing (if real) and BuildList again
    if (numo->num==0){
    	
    	if(real){
			
			ProbeObj *po0 = new ProbeObj(num >> 1, num, 									TDOWN, new Identity(in));
			ProbeObj *po1 = new ProbeObj((num >> 1) | (1 << (8*sizeof(unsigned)-1)), num, 	TUP, new Identity(in));
			
			//start Probe to the left
			if(left != NULL){
				left->out->call(List::Probe, po0);
			}
			// Probe fails on first step
			else{
				IDObj *io = new IDObj(num, new Identity(in));
				ChildLinkObj *clo = new ChildLinkObj(0, io);
				std::cout << "Probe failed. From " << po0->sourceHash << " to " << po0->destinationHash << " (First Step)" << '\n';
				parent->call(Node::EstablishChildLink, clo);
			}
			//start Probe to the right
			if(right != NULL){
				right->out->call(List::Probe, po1);
			}
			//Probe fails on first step
			else{
				IDObj *io = new IDObj(num, new Identity(in));
				ChildLinkObj *clo = new ChildLinkObj(1, io);
				std::cout << "Probe failed. From " << po1->sourceHash << " to " << po1->destinationHash << " (First Step)" << '\n';
				parent->call(Node::EstablishChildLink, clo);
			}
		}
    	
    	BuildList(NULL);
    }
    else {
		numo->num--;
		call(List::Wakeup, numo);
    }
}

Action List::BuildList(IDObj *ido)
{
    Identity *id;
    IDObj *tempido;
    NumObj *counter;
	
    // check if link from left dead
    if (left!=NULL && outdeg(left->out)==0) { 
		std::cout << "Node " << num << ": link to " << left->num << " broken.\n";
		delete left->out; delete left; left=NULL; 
    }
    // check if link from right dead
    if (right!=NULL && outdeg(right->out)==0) { 
		std::cout << "Node " << num << ": link to " << right->num << " broken.\n";
		delete right->out; delete right; right=NULL; 
    } 
    // check if left link still valid
    if (left!=NULL && left->num > num) {
		std::cout << "Node " << num << ": removing left link to " << left->num << ".\n";
		tempido = new IDObj(left->num, extractIdentity(left->out));
		delete left;
		left = NULL;
		call(List::BuildList, tempido);
    }
    // check if right link still valid
    if (right!=NULL && right->num < num) {
		std::cout << "Node " << num << ": removing right link to " << right->num << ".\n";
		tempido = new IDObj(right->num, extractIdentity(right->out));
		delete right;
		right = NULL;
		call(List::BuildList, tempido);
    }
    if (ido==NULL) {
		// timeout: ask neighbors to create return links
		if (left!=NULL) {
			tempido = new IDObj(num, new Identity(in));
			left->out->call(List::BuildList, tempido);
		}
		if (right!=NULL) {
			tempido = new IDObj(num, new Identity(in));
			right->out->call(List::BuildList, tempido);
		}
		// prepare next timeout
		counter = new NumObj(5);
		call(List::Wakeup, counter);
    }
    else {
		if (ido->num > num) {
			if (right==NULL) {                // right link not yet defined
				std::cout << "Node " << num << ": creating link to " << ido->num << ".\n";
				right = new ListRelay(ido);
			}
			else {
				if (ido->num > right->num) {    // ido beyond right link
					std::cout << "Node " << num << ": forwarding " << ido->num << " to " << right->num << ".\n";
					right->out->call(List::BuildList, ido);
				}
				else {
					if (right->num > ido->num) {  // ido between node and right link
						if (idle(right->out)) {
							std::cout << "Node " << num << ": new right " << ido->num << ", forwarding " << right->num << ".\n";
							tempido = new IDObj(right->num, extractIdentity(right->out));
							delete right;
							right = new ListRelay(ido);
							right->out->call(List::BuildList, tempido);
						}
						else call(List::BuildList, ido);
					}
					else {                        // ido->num = right->num
						delete ido->id;
						delete ido;  
					}
				}
			}
		}
		else {                              // ido->num < num
			if (ido->num < num) {
				if (left==NULL) {               // left link not yet defined
					std::cout << "Node " << num << ": creating link to " << ido->num << ".\n";
					left = new ListRelay(ido);
				}
				else {
					if (ido->num < left->num) {   // ido below left link
						std::cout << "Node " << num << ": forwarding " << ido->num << " to " << left->num << ".\n";
						left->out->call(List::BuildList, ido);
					}
					else {
						if (left->num < ido->num) { // ido between node and left link
							if (idle(left->out)) {
								std::cout << "Node " << num << ": new left " << ido->num << ", forwarding " << left->num << ".\n";
								tempido = new IDObj(left->num, extractIdentity(left->out));
								delete left;
								left = new ListRelay(ido);
								left->out->call(List::BuildList, tempido);
							}
							else call(List::BuildList, ido);
						}
						else {                      // ido->num = left->num
							delete ido->id; 
							delete ido;  
						}
					}
				}
			}     
			else { delete ido->id; delete ido; } // ido->num = num             
		}
    }
}

Action List::Insert(IDObj *ido)
{
    BuildList(ido);  // just connect to given reference
}

Action List::Search(MessageObj *m)
{
    if (num==m->rInfo->destination) 
		std::cout << "Node " << num << ": message received: " << m->msg << "\n";
    else {
    	//DeBruijn Routing
    	
    	std::cout << '\n' << '\n';
		
        std::cout << "Search reached Node "<< num << ", its " <<  (real?"REAL.":"VIRTUAL.") <<  '\n';
		
    	if(real && m->rInfo->hopCount < (8*(sizeof(unsigned)))){
    		m->rInfo->hopCount++;
    		m->rInfo->hasJustJumped = true;
    		std::cout << "Increased hopCount to " << m->rInfo->hopCount << ", preparing for jump.\n";
    		MessageWrapper *mw;
    		unsigned selector = m->rInfo->hopNums[m->rInfo->hopCount] >> (8*(sizeof(unsigned))-1);
    		mw = new MessageWrapper(selector, m);
    		std::cout << "Using " << selector << "-DeBruijn-Edge." << '\n';
    		std::cout << "Forwarding to Parent.\n";
    		parent->call(Node::ForwardRoutingRequest, mw);
		}
		else if(m->rInfo->hasJustJumped)
		{
			std::cout << "Packet landed.\n";
			std::cout << "Forwarding Search in List. Left is " << left << ", right is " << right << "\n";
			
			m->rInfo->hasJustJumped = false;
			
			if(m->rInfo->hopNums[m->rInfo->hopCount] == num){
				std::cout << "Going to forward to the " << (m->rInfo->destination < num?"left":"right") << ".\n";
			
				if(m->rInfo->destination < num){
					m->rInfo->travelDirection = TDOWN;
					left->out->call(List::Search, m);
				}
				else{
					m->rInfo->travelDirection = TUP;
					right->out->call(List::Search, m);	
				}
			}
			else{
				std::cout << "Going to forward to the " << (m->rInfo->hopNums[m->rInfo->hopCount] < num?"left":"right") << ".\n";
				
				if(m->rInfo->hopNums[m->rInfo->hopCount] < num && left != NULL){
					m->rInfo->travelDirection = TDOWN;
					left->out->call(List::Search, m);
				}
				else if(right != NULL){
					m->rInfo->travelDirection = TUP;
					right->out->call(List::Search, m);
				}
				else{
					m->rInfo->travelDirection = TDOWN;
					left->out->call(List::Search, m);
				}
			}
		}
		else
		{
			std::cout << "Packet has Direction.\n";
			std::cout << "Forwarding Search in List. Left is " << left << ", right is " << right << "\n";
			if((m->rInfo->travelDirection == TDOWN && left == NULL)||(m->rInfo->travelDirection == TUP && right == NULL)){
				std::cout << "Cannot use given Direction.\n";
				m->rInfo->travelDirection = !m->rInfo->travelDirection;
			}
			std::cout << "Going to forward to the " << (m->rInfo->travelDirection == TDOWN?"left":"right") << ".\n";
			
			if(m->rInfo->travelDirection == TDOWN)
				left->out->call(List::Search, m);
			else
				right->out->call(List::Search, m);	
			
		}
    }    
}

Action List::MakeReal(NumObj *dummy){

	// dont use the dummy

	real = true;
	
	std::cout << "Node " << num << ": I´m Real.\n";
}

Action List::Probe(ProbeObj *po){
	
	//Probe successfull
	if(po->destinationHash == num){
		//std::cout << "Probe successfull. From " << po->sourceHash << " to " << po->destinationHash << '\n';
	}
	//Probe is going to jump
	else if(real && po->jump){
		parent->call(Node::ForwardProbe, po);
	}
	//Probe fails
	else if(
			// because it travels up and theres no right neighbour
			(po->travelDirection == TUP && right == NULL) || 
			// because it travels down and theres no left neighbour
			(po->travelDirection == TDOWN && left == NULL) || 
			// because it travels up and it already bypassed the destination
			(po->travelDirection == TUP && po->destinationHash < num) || 
			// because it travels down and it already bypassed the destination
			(po->travelDirection == TDOWN && po->destinationHash > num))
	{
		std::cout << "Probe failed. From " << po->sourceHash << " to " << po->destinationHash ;
		std::cout << "(" << (po->travelDirection == TUP && right == NULL) << (po->travelDirection == TDOWN && left == NULL) << (po->travelDirection == TUP && po->destinationHash < num) << (po->travelDirection == TDOWN && po->destinationHash > num) << ")" << '\n';
		Relay *r = new Relay(po->sender);
		r->call(List::ProbeFailed, po);
	}
	// Forward Probe
	else{
		if(po->travelDirection == TUP)
			right->out->call(List::Probe, po);
		else
			left->out->call(List::Probe, po);
	}
}

Action List::ProbeFailed(ProbeObj *po){
	IDObj *io = new IDObj(num, new Identity(in));
	ChildLinkObj *clo;
	if(po->destinationHash > num)
		clo = new ChildLinkObj(1, io);
	else
		clo = new ChildLinkObj(0, io);
	parent->call(Node::EstablishChildLink, clo);
}
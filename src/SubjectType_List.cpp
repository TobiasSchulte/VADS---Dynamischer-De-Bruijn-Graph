Action List::Init(VirtualNodeInformation *nInfo)
{
    IDPair *idp;
    vInfo = nInfo;
    left = NULL;
    right = NULL;
    
    in = new Relay;
    Identity *ti1 = new Identity(in);
    Identity *ti2 = new Identity(in);
    IDObj *tIdObj1 = new IDObj(vInfo->hash, ti1);
    IDObj *tIdObj2 = new IDObj(vInfo->hash, ti2);

    idp = new IDPair(tIdObj1, tIdObj2, vInfo->belongsToRealNode);

    parent->call(Node::SetLink, idp);
   
    Identity *ti3 = new Identity(in);
    IDObj *tidobj = new IDObj(vInfo->hash, ti3);
    
    parent->call(Node::RegisterChild, tidobj);

    if(DEBUG_OUTPUT)
    	std::cout << "Node " << vInfo->hash  << '\t' << '(' << dec2bin(vInfo->hash) << ')' << ": Hello World."<< "\n";
} 

Action List::Wakeup(NumObj *numo)
{
    // check whether it's time to activate BuildList again
    if (numo->num==0){
    	BuildList(NULL);
    }
    else {
		numo->num--;
		call(List::Wakeup, numo);
    }
}

Action List::StartProbe(NumObj *numo){


    // check whether it's time to activate Debruijn Probing (if real)
    if (numo->num==0){    	
    	if(vInfo->belongsToRealNode){
			
			ProbeObj *po0 = new ProbeObj(vInfo->hash >> 1, vInfo->hash, 									TDOWN, new Identity(in));
			ProbeObj *po1 = new ProbeObj((vInfo->hash >> 1) | (1 << (8*sizeof(unsigned)-1)), vInfo->hash, 	TUP, new Identity(in));
			
			//start Probe to the left
			if(left != NULL){
				left->out->call(List::Probe, po0);
			}
			// Probe fails on first step
			else{
				IDObj *io = new IDObj(vInfo->hash, new Identity(in));
				ChildLinkObj *clo = new ChildLinkObj(0, io);

				if(DEBUG_OUTPUT)
					std::cout << "Probe failed. From " << po0->sourceHash << " to " << po0->destinationHash << " (First Step)" << '\n';

				parent->call(Node::EstablishChildLink, clo);
			}
			//start Probe to the right
			if(right != NULL){
				right->out->call(List::Probe, po1);
			}
			//Probe fails on first step
			else{
				IDObj *io = new IDObj(vInfo->hash, new Identity(in));
				ChildLinkObj *clo = new ChildLinkObj(1, io);
				if(DEBUG_OUTPUT)
					std::cout << "Probe failed. From " << po1->sourceHash << " to " << po1->destinationHash << " (First Step)" << '\n';
				parent->call(Node::EstablishChildLink, clo);
			}
		}
		
		NumObj *pnumo = new NumObj(PERIODIC_PROBE_TIME);
		call(List::StartProbe, pnumo);
    }
    else {
		numo->num--;
		call(List::StartProbe, numo);
    }





}

Action List::BuildList(IDObj *ido)
{
    Identity *id;
    IDObj *tempido;
    NumObj *counter;
	
    // check if link from left dead
    if (left!=NULL && outdeg(left->out)==0) { 
    	if(DEBUG_OUTPUT)
    		std::cout << "Node " << vInfo->hash << ": link to " << left->num << " broken.\n";
		delete left->out; delete left; left=NULL; 
    }
    // check if link from right dead
    if (right!=NULL && outdeg(right->out)==0) {

    	if(DEBUG_OUTPUT)
    		std::cout << "Node " << vInfo->hash << ": link to " << right->num << " broken.\n";
		delete right->out; delete right; right=NULL; 
    } 
    // check if left link still valid
    if (left!=NULL && left->num > vInfo->hash) {

    	if(DEBUG_OUTPUT)
    		std::cout << "Node " << vInfo->hash << ": removing left link to " << left->num << ".\n";
		tempido = new IDObj(left->num, extractIdentity(left->out));
		delete left;
		left = NULL;
		call(List::BuildList, tempido);
    }
    // check if right link still valid
    if (right!=NULL && right->num < vInfo->hash) {

    	if(DEBUG_OUTPUT)
    		std::cout << "Node " << vInfo->hash << ": removing right link to " << right->num << ".\n";
		tempido = new IDObj(right->num, extractIdentity(right->out));
		delete right;
		right = NULL;
		call(List::BuildList, tempido);
    }
    if (ido==NULL) {
		// timeout: ask neighbors to create return links
		if (left!=NULL) {
			tempido = new IDObj(vInfo->hash, new Identity(in));
			left->out->call(List::BuildList, tempido);
		}
		if (right!=NULL) {
			tempido = new IDObj(vInfo->hash, new Identity(in));
			right->out->call(List::BuildList, tempido);
		}
		// prepare next timeout
		counter = new NumObj(5);
		call(List::Wakeup, counter);
    }
    else {
		if (ido->num > vInfo->hash) {
			if (right==NULL) {                // right link not yet defined

				if(DEBUG_OUTPUT)
					std::cout << "Node " << vInfo->hash << ": creating link to " << ido->num << ".\n";
				right = new ListRelay(ido);
			}
			else {
				if (ido->num > right->num) {    // ido beyond right link
					if(DEBUG_OUTPUT)
						std::cout << "Node " << vInfo->hash << ": forwarding " << ido->num << " to " << right->num << ".\n";
					right->out->call(List::BuildList, ido);
				}
				else {
					if (right->num > ido->num) {  // ido between node and right link
						if (idle(right->out)) {
							if(DEBUG_OUTPUT)
								std::cout << "Node " << vInfo->hash << ": new right " << ido->num << ", forwarding " << right->num << ".\n";
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
			if (ido->num < vInfo->hash) {
				if (left==NULL) {               // left link not yet defined
					if(DEBUG_OUTPUT)
						std::cout << "Node " << vInfo->hash << ": creating link to " << ido->num << ".\n";
					left = new ListRelay(ido);
				}
				else {
					if (ido->num < left->num) {   // ido below left link
						if(DEBUG_OUTPUT)
							std::cout << "Node " << vInfo->hash << ": forwarding " << ido->num << " to " << left->num << ".\n";
						left->out->call(List::BuildList, ido);
					}
					else {
						if (left->num < ido->num) { // ido between node and left link
							if (idle(left->out)) {
								if(DEBUG_OUTPUT)
									std::cout << "Node " << vInfo->hash << ": new left " << ido->num << ", forwarding " << left->num << ".\n";
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
    if (vInfo->hash==m->rInfo->destination){
		std::cout << "Node " << vInfo->hash << ": message received: " << m->msg << "\n";
		std::cout << "Node " << vInfo->hash << ": message visited " << m->rInfo->visitedNodes << " and used " << m->rInfo->hopCount << " DeBruijn Edges." << "\n";
	}
    else {
    	//DeBruijn Routing
    	m->rInfo->visitedNodes++;

    	if(DEBUG_OUTPUT){
    		std::cout << '\n' << '\n';
    		std::cout << "Search reached Node "<< vInfo->hash << ", its " <<  (vInfo->belongsToRealNode?"REAL.":"VIRTUAL.") <<  '\n';
    	}

    	if(vInfo->belongsToRealNode && m->rInfo->hopCount < (8*(sizeof(unsigned)))){
    		if(m->rInfo->hopCount == 0){
    			m->rInfo->hopCount++;
    			m->rInfo->hasJustJumped = true;
    			if(DEBUG_OUTPUT)
    				std::cout << "Increased hopCount to " << m->rInfo->hopCount << ", preparing for jump.\n";
    			MessageWrapper *mw;
    			unsigned selector = m->rInfo->hopNums[m->rInfo->hopCount] >> (8*(sizeof(unsigned))-1);
    			mw = new MessageWrapper(selector, m);
    			if(DEBUG_OUTPUT)
    				std::cout << "Using " << selector << "-DeBruijn-Edge." << "Forwarding to Parent.\n";

    			parent->call(Node::ForwardRoutingRequest, mw);
    		}
    		// von links, idealposition links -> springen
    		// von rechts, idealposition rechts -> springen
    		else if((m->rInfo->travelDirection == TUP   && vInfo->hash > m->rInfo->hopNums[m->rInfo->hopCount])
    			||  (m->rInfo->travelDirection == TDOWN && vInfo->hash < m->rInfo->hopNums[m->rInfo->hopCount])){
    			m->rInfo->hopCount++;
    			m->rInfo->hasJustJumped = true;

    			if(DEBUG_OUTPUT)
    				std::cout << "Increased hopCount to " << m->rInfo->hopCount << ", preparing for jump.\n";

    			MessageWrapper *mw;
    			unsigned selector = m->rInfo->hopNums[m->rInfo->hopCount] >> (8*(sizeof(unsigned))-1);
    			mw = new MessageWrapper(selector, m);

    			if(DEBUG_OUTPUT)
    				std::cout << "Using " << selector << "-DeBruijn-Edge." << "Forwarding to Parent.\n";

    			parent->call(Node::ForwardRoutingRequest, mw);
    		}
    		// von rechts, idealposition links -> weiterleiten
    		// von links, idealposition rechts -> weiterleiten
			else{
				if(DEBUG_OUTPUT)
					std::cout << "Packet has Direction.\n" << "Forwarding Search in List. Left is " << left << ", right is " << right << "\n";

				if((m->rInfo->travelDirection == TDOWN && left == NULL)||(m->rInfo->travelDirection == TUP && right == NULL)){
					if(DEBUG_OUTPUT)
						std::cout << "Cannot use given Direction.\n";
					m->rInfo->travelDirection = !m->rInfo->travelDirection;
				}
				if(DEBUG_OUTPUT)
					std::cout << "Going to forward to the " << (m->rInfo->travelDirection == TDOWN?"left":"right") << ".\n";
				
				if(m->rInfo->travelDirection == TDOWN)
					left->out->call(List::Search, m);
				else
					right->out->call(List::Search, m);	
			}
		}
		else if(m->rInfo->hasJustJumped)
		{
			if(DEBUG_OUTPUT)
				std::cout << "Packet landed.\n" << "Forwarding Search in List. Left is " << left << ", right is " << right << "\n";
			
			m->rInfo->hasJustJumped = false;
			
			if(m->rInfo->hopNums[m->rInfo->hopCount] == vInfo->hash){
				if(DEBUG_OUTPUT)
					std::cout << "Going to forward to the " << (m->rInfo->destination < vInfo->hash?"left":"right") << ".\n";
			
				if(m->rInfo->destination < vInfo->hash){
					m->rInfo->travelDirection = TDOWN;
					left->out->call(List::Search, m);
				}
				else{
					m->rInfo->travelDirection = TUP;
					right->out->call(List::Search, m);	
				}
			}
			else{
				if(DEBUG_OUTPUT)
					std::cout << "Going to forward to the " << (m->rInfo->hopNums[m->rInfo->hopCount] < vInfo->hash?"left":"right") << ".\n";
				
				if(m->rInfo->hopNums[m->rInfo->hopCount] < vInfo->hash && left != NULL){
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
			if(DEBUG_OUTPUT)
				std::cout << "Packet has Direction.\n" << "Forwarding Search in List. Left is " << left << ", right is " << right << "\n";
			if((m->rInfo->travelDirection == TDOWN && left == NULL)||(m->rInfo->travelDirection == TUP && right == NULL)){
				if(DEBUG_OUTPUT)
					std::cout << "Cannot use given Direction.\n";
				m->rInfo->travelDirection = !m->rInfo->travelDirection;
			}
			if(DEBUG_OUTPUT)
				std::cout << "Going to forward to the " << (m->rInfo->travelDirection == TDOWN?"left":"right") << ".\n";
			
			if(m->rInfo->travelDirection == TDOWN)
				left->out->call(List::Search, m);
			else
				right->out->call(List::Search, m);	
			
		}
    }    
}

Action List::Probe(ProbeObj *po){
	
	//Probe successfull
	if(po->destinationHash == vInfo->hash){

		if(PROBE_SUCCESS_OUTPUT)
			std::cout << "Probe successfull. From " << po->sourceHash << " to " << po->destinationHash << '\n';
	}
	//Probe is going to jump
	else if(vInfo->belongsToRealNode && po->jump){
		parent->call(Node::ForwardProbe, po);
	}
	//Probe fails
	else if(
			// because it travels up and theres no right neighbour
			(po->travelDirection == TUP && right == NULL) || 
			// because it travels down and theres no left neighbour
			(po->travelDirection == TDOWN && left == NULL) || 
			// because it travels up and it already bypassed the destination
			(po->travelDirection == TUP && po->destinationHash < vInfo->hash) ||
			// because it travels down and it already bypassed the destination
			(po->travelDirection == TDOWN && po->destinationHash > vInfo->hash))
	{
		if(DEBUG_OUTPUT){
			std::cout << "Probe failed. From " << po->sourceHash << " to " << po->destinationHash ;
			std::cout << "(" << (po->travelDirection == TUP && right == NULL) << (po->travelDirection == TDOWN && left == NULL) << (po->travelDirection == TUP && po->destinationHash < vInfo->hash) << (po->travelDirection == TDOWN && po->destinationHash > vInfo->hash) << ")" << '\n';
		}
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
	IDObj *io = new IDObj(vInfo->hash, new Identity(in));
	ChildLinkObj *clo;
	if(po->destinationHash > vInfo->hash)
		clo = new ChildLinkObj(1, io);
	else
		clo = new ChildLinkObj(0, io);
	parent->call(Node::EstablishChildLink, clo);

	delete po;
}

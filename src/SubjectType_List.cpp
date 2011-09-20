Action List::Init(NumObj *numo)
{
    IDPair *idp;
	
    num = numo->num;
    delete numo;
    left = NULL;
    right = NULL;
    in = new Relay;
    idp = new IDPair(new IDObj(num, new Identity(in)), new IDObj(num, new Identity(in)));
    parent->call(Node::SetLink, idp);
    std::cout << "Node " << num << ": Hello World.\n";
    
    IDObj *tidobj = new IDObj(num, new Identity(in));
    
    parent->call(Node::RegisterChild, tidobj);
} 

Action List::Wakeup(NumObj *numo)
{
    // check whether it's time to activate BuildList again
    if (numo->num==0) BuildList(NULL);
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


Action List::Search(NumObj *numo)
{
    if (num==numo->num) 
		std::cout << "Node " << num << ": message received.\n";
    else {
		if (numo->num <= left->num) {
			std::cout << "Node " << num << ": forwarding message.\n";
			left->out->call(List::Search, numo);
		}
		else {
			if (numo->num >= right->num) {
				std::cout << "Node " << num << ": forwarding message.\n";
				right->out->call(List::Search, numo);
			}
			else std::cout << "Node " << num << ": message cannot be delivered.\n";
		}
    }         
}
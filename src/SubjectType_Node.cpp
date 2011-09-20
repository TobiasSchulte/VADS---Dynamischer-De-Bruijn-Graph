Action Node::Init(NumObj *numo){
	
	num = numo->num;
	
	NumObj* n0 = new NumObj(num >> 1);
	NumObj* n  = new NumObj(num);
	NumObj* n1 = new NumObj((num >> 1) | (1 << (8*sizeof(unsigned)-1)));
	
	new(List, n0);
	new(List, n);
	new(List, n1);
}

Action Node::SetLink(IDPair *idp){

	parent->call(Supervisor::SetLink, idp);
}

Action Node::RegisterChild(IDObj *ido){

	if(ido->num == num){
		v = new ListRelay(ido);
	}
	else if(ido->num == (num >> 1)){
		v0 = new ListRelay(ido);
	}
	else if(ido->num == ((num >> 1) | (1 << (8*sizeof(unsigned)-1)))){
		v1 = new ListRelay(ido);
	}
	
	if(v != NULL && v0 != NULL && v1 != NULL)
		std::cout << "Node " << num << " ready." << '\n';
	
}
Action Node::Init(NumObj *numo){
	
	num = numo->num;
	delete numo;
	VirtualNodeInformation* vInfo_v0 = new VirtualNodeInformation(num >> 1, false);
	VirtualNodeInformation* vInfo_v  = new VirtualNodeInformation(num, true);
	VirtualNodeInformation* vInfo_v1 = new VirtualNodeInformation((num >> 1) | (1 << (8*sizeof(unsigned)-1)), false);
	
	new(List, vInfo_v0);
	new(List, vInfo_v);
	new(List, vInfo_v1);
}

Action Node::SetLink(IDPair *idp){

	parent->call(Supervisor::SetLink, idp);
}

Action Node::RegisterChild(IDObj *ido){

	if(ido->num == num)
		v = new ListRelay(ido);
	else if(ido->num == (num >> 1))
		v0 = new ListRelay(ido);
	else if(ido->num == ((num >> 1) | (1 << (8*sizeof(unsigned)-1))))
		v1 = new ListRelay(ido);
	
	if(v != NULL && v0 != NULL && v1 != NULL)
		std::cout << "Node " << num << " ready." << '\n';
	
}

Action Node::EstablishChildLink(ChildLinkObj *co){
	//establish Link from v0 to v
	if(co->childNo == 0){
		//std::cout << "Using 0-DeBruijn Edge to connect " << num << " and " << (num >> 1) << "." << '\n';
		v0->out->call(List::Insert, co->ido);
	}
	//establish Link from v1 to v
	else{
		//std::cout << "Using 1-DeBruijn Edge to connect " << num << " and " << ((num >> 1) | (1 << (8*sizeof(unsigned)-1))) << "." << '\n';
		v1->out->call(List::Insert, co->ido);	
	}
	delete co;
}

Action Node::ForwardProbe(ProbeObj *po){

	po->jump = false;
	
	//jump to the right
	if(po->travelDirection == TUP){
		po->travelDirection = TDOWN;
		v1->out->call(List::Probe, po);
	}
	//jump to the left
	else{
		po->travelDirection = TUP;
		v0->out->call(List::Probe, po);
	}

}

Action Node::ForwardRoutingRequest(MessageWrapper *mw){
	
	if(mw->selector == 1)
		v1->out->call(List::Search, mw->m);
	else
		v0->out->call(List::Search, mw->m);
	
}

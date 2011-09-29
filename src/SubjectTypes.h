SubjectType(List)
{
  	Relay *in;
  	ListRelay *left;
  	ListRelay *right;
  	VirtualNodeInformation *vInfo;
  
public:
  	FirstAction(List,Init)

	Action Init(VirtualNodeInformation *vInfo);
	Action Wakeup(NumObj *numo);
  	Action BuildList(IDObj *ido);
  	Action Insert(IDObj *ido);
  	Action Search(MessageObj *m);

	Action Probe(ProbeObj *prob);
	Action ProbeFailed(ProbeObj *po);

};

SubjectType(Node)
{
	unsigned num;
	ListRelay *v0;
	ListRelay *v;
	ListRelay *v1;

public:
	FirstAction(Node,Init);

	Action Init(NumObj *numo);
	Action Wakeup(NumObj *numo);
	
	Action SetLink(IDPair* idp);
	
	Action RegisterChild(IDObj *ido);
	
	Action EstablishChildLink(ChildLinkObj *ido);
	
	Action ForwardProbe(ProbeObj *po);

	Action ForwardRoutingRequest(MessageWrapper *mw);
};

SubjectType(Supervisor)
{
	IDPair* ListObjects[NODES*3];
	unsigned NodeNames[NODES*3];
	int count;
	Relay* Nodes[NODES*3];
	
public:
	FirstAction(Supervisor, Init)
	
	Action Init(NumObj *numo);
	Action SetLink(IDPair *idop);
	Action Wakeup(NumObj *numo);
};

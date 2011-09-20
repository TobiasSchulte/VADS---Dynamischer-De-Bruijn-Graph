SubjectType(List)
{
  	unsigned num;
  	Relay *in;
  	ListRelay *left;
  	ListRelay *right;
  	bool real;
  
public:
  	FirstAction(List,Init)

	Action Init(NumObj *numo);
	Action Wakeup(NumObj *numo);
  	Action BuildList(IDObj *ido);
  	Action Insert(IDObj *ido);
  	Action Search(NumObj *numo);

	Action Probe(ProbeObj *prob);
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
};

SubjectType(Supervisor)
{
	IDPair* ListObjects[NODES*3];
	int count;
	
public:
	FirstAction(Supervisor, Init)
	
	Action Init(NumObj *numo);
	Action SetLink(IDPair *idop);
	Action Wakeup(NumObj *numo);
};
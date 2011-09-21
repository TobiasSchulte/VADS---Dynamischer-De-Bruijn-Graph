ObjectType(NumObj)
{
public:
  unsigned num;

  NumObj(unsigned i) { num=i; }
};

ObjectType(ProbeObj)
{
public:
  unsigned destinationHash;
  unsigned sourceHash;
  bool travelDirection;
  bool jump;
  Identity *sender;

  ProbeObj(unsigned tar, unsigned src, bool dir, Identity *s) { destinationHash=tar; sourceHash=src; travelDirection=dir; sender=s; jump=true; }
};

ObjectType(IDObj)
{
public:
  unsigned num;
  Identity *id;

  IDObj(unsigned n, Identity *d) { num=n; id=d; }
  //IDObj(Identity *d) {id=d; num=NULL; }
};

ObjectType(ChildLinkObj)
{
public:
	IDObj *ido;
	unsigned childNo;
	
	ChildLinkObj(unsigned child, IDObj *io) {childNo = child; ido=io; }
};

// Type to store list relays within subjects

ObjectType(ListRelay)
{
public:
  unsigned num;
  Relay *out;
  
  ListRelay(IDObj *ido) 
  { 
    num=ido->num; out = new Relay(ido->id);
    delete ido; 
  }
};

ObjectType(IDPair)
{
public:
	IDObj *ido1;
	IDObj *ido2;
	
	IDPair(IDObj *i1, IDObj *i2) {ido1=i1; ido2=i2; }
};

ObjectType(RoutingInformation)
{
public:
	unsigned hopNums[33];
	unsigned source;
	unsigned destination;
	unsigned hopCount;
	bool hasJustJumped;
	bool travelDirection;
	
	RoutingInformation(unsigned s, unsigned d){
		hopCount = 0;
		source = s;
		destination = d;
		hasJustJumped = false;
		travelDirection = true;
		for(int i = 0; i < 33; i++){
			hopNums[i] = (source >> i) | (destination << (32-i));
		}
	}
};

ObjectType(MessageObj)
{
public:
	const char *msg;
	RoutingInformation *rInfo;
	
	MessageObj(const char *s, RoutingInformation *r){ msg=s; rInfo=r; }
};

ObjectType(MessageWrapper)
{
public:
	unsigned selector;
	MessageObj *m;
	
	MessageWrapper(unsigned s, MessageObj *mo) {selector=s; m=mo;}
};
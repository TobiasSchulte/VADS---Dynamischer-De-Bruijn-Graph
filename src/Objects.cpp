ObjectType(NumObj)
{
public:
  unsigned num;

  NumObj(unsigned i) { num=i; }
};

ObjectType(ProbeObj)
{
public:
  unsigned target;
  bool type;
  Identity *sender;

  ProbeObj(unsigned tar, bool t, Identity *s) { target=tar; type=t; sender=s; }
};

ObjectType(IDObj)
{
public:
  unsigned num;
  Identity *id;

  IDObj(unsigned n, Identity *d) { num=n; id=d; }
  IDObj(Identity *d) {id=d; num=NULL; }
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
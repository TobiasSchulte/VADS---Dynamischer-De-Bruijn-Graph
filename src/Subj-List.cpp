// Subjects-List

#include <iostream>
#include "Subjects1-6.h"

#define MAX 1000

// Objects

ObjectType(NumObj)
{
public:
  int num;

  NumObj(int i) { num=i; }
};

ObjectType(IDObj)
{
public:
  int num;
  Identity *id;

  IDObj(int n, Identity *d) { num=n; id=d; }
};

ObjectType(IDPair)
{
public:
  IDObj *ido1;
  IDObj *ido2;
  
  IDPair(IDObj *i1, IDObj *i2) {ido1=i1; ido2=i2; }
};

// Type to store list relays within subjects

ObjectType(ListRelay)
{
public:
  int num;
  Relay *out;
  
  ListRelay(IDObj *ido) 
  { 
    num=ido->num; out = new Relay(ido->id);
    delete ido; 
  }
};

// List subject

SubjectType(List)
{
  int num;
  Relay *in;
  ListRelay *left;
  ListRelay *right;
  
public:
  FirstAction(List,Init)

  Action Init(NumObj *numo);
  Action Wakeup(NumObj *numo);
  Action BuildList(IDObj *ido);
  Action Insert(IDObj *ido);
  Action Delete();
  Action Search(NumObj *numo);
};
  
// Supervisor initiates and controls List subjects

SubjectType(Supervisor)
{
  int count;
  int total;
  Relay* ListNode[100];
  IDObj* StartID[100];
                       
public:
  FirstAction(Supervisor, Init)
  
  Action Init(NumObj *numo);
  Action SetLink(IDPair *idop);
  Action Wakeup(NumObj *numo);
};
  
  
  Action List::Init(NumObj *numo)
  {
    IDPair *idp;
         
    num = numo->num;
    delete numo;
    left = NULL;
    right = NULL;
    in = new Relay;
    idp = new IDPair(new IDObj(num, new Identity(in)), new IDObj(num, new Identity(in)));
    parent->call(Supervisor::SetLink, idp);
    std::cout << "Node " << num << ": Hello World.\n";
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
      counter = new NumObj(0);
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


  Action List::Delete()
  {
    std::cout << "Node " << num << ": preparing to leave system.\n";    
    num = num+MAX;    // increase num to get to end of list
    delete in;       // invalidate existing links/identities to 'in'
    in = new Relay;  // create new 'in' to be used for new 'num'
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


//=====================================================================
  
  Action Supervisor::Init(NumObj *numo)
  {
    int i;
    NumObj *tempnum;
    
    for (i=1; i<=numo->num; i++) {
      tempnum = new NumObj(i);
      new(List,tempnum);
    }
    count = 0;
    total = numo->num;
    delete numo;
  } 

  Action Supervisor::SetLink(IDPair *idop)
  {
    int i;     
    NumObj *numo;
         
    ListNode[idop->ido1->num] = new Relay(idop->ido1->id);
    delete idop->ido1;
    StartID[idop->ido2->num] = idop->ido2;
    delete idop;
    count++;
    if (count == total) {
      for (i=1; i<=total; i++) {
        numo = new NumObj(5);
        // make sure that node i periodically wakes up
        ListNode[i]->call(List::Wakeup, numo);
        // initially connect node i to nodes 2i and 2i+1
        if (2*i<=total) ListNode[i]->call(List::Insert, StartID[2*i]);
        if (2*i+1<=total) ListNode[i]->call(List::Insert, StartID[2*i+1]);
      }
      // wait 100 rounds till testing Delete or Search
      numo = new NumObj(100);
      call(Supervisor::Wakeup, numo);
    }
  }

  Action Supervisor::Wakeup(NumObj* numo)
  {
    NumObj *searchnum;
    
    if (numo->num > 0) {
      numo->num--;
      call(Supervisor::Wakeup, numo);
    }
    else {
      // test Delete or Search
      ListNode[3]->call(List::Delete, NONE);
      //searchnum = new NumObj(9);
      //ListNode[3]->call(List::Search, searchnum);
    }
  }


// main() function

int main()
{
  NumObj* num = new NumObj(10);
  // start with creating a supervisor, run for (at most) 300 rounds
  runSubjects(Supervisor,num,300);
  sys_pause();
}

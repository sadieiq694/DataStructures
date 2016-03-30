#include <iostream>

using namespace std;

class ListNode
{
public:
    string value;
    ListNode* next;
public:
    ListNode(string v);
    ~ListNode();
    void addNode(std::string value);
};
ListNode::ListNode(string v)
{
    value = v;
    next = nullptr;
}
ListNode::~ListNode()
{
    cout<< "Node with value: "<< value << " destructed"<<endl;
    delete next;
}
void ListNode::addNode(std::string value)
{
    ListNode* newNode = new ListNode(value);
    ListNode* currentNode = this;
    while(currentNode->next != nullptr)
    {
        currentNode=currentNode->next;
    }
    currentNode->next=newNode;
}

class List
{
public:
    ListNode* head;
public:
    List();
    ~List();
    void add(string value);
    void display();

    string get(int index);

    void insert(int index, string value);
    void remove(int index);

    void addFront(string value) {insert(0, value);}
};
List::List()
{
    head=nullptr;
}
List::~List()
{
    delete head;
}
void List::add(string value)
{
    if(head==nullptr)
    {
        head = new ListNode(value);
    }
    else
    {
        head->addNode(value);
    }
}
void List::display()
{
    ListNode* currentNode=head;
    cout << "This list contains:" << endl;
    while(currentNode != nullptr)
    {
        cout << currentNode->value << endl;
        currentNode = currentNode->next;
    }
    cout<<endl;
}
string List::get(int index)
{
    ListNode* currentNode = head;
    for(int i=0; i<index; i++)
    {
        currentNode = currentNode->next;
    }
    return currentNode->value;
}
void List::insert(int index, string value)
{
    ListNode* currentNode = head;
    ListNode* newNode = new ListNode(value);
    if(index==0)
    {
        head=newNode;
        newNode->next=currentNode;
    }
    else
    {
        for(int i=0; i<index-1; i++)
        {
            currentNode = currentNode->next;
        }
        ListNode* tail = currentNode->next;
        currentNode->next=newNode;
        newNode->next=tail;
    }

}
void List::remove(int index)
{
    ListNode* currentNode = head;
    if(index==0)
    {
        ListNode*currentNode = head;
        ListNode* nodeTBD = head;
        nodeTBD->next = nullptr;
        currentNode=currentNode->next;
        head = currentNode;
    }
    else
    {
        for(int i = 0; i<index-1; i++)
        {
            currentNode=currentNode->next;
        }
        ListNode* nodeBefore = currentNode;
        currentNode=currentNode->next;
        ListNode* nodeTBD = currentNode;
        currentNode=currentNode->next;
        nodeTBD->next=nullptr;
        nodeBefore->next=currentNode;
    }
}


int main()
{
    List List1;
    List1.add("hello");
    List1.add("my");
    List1.add("name");
    List1.add("is");
    cout<< List1.get(2) << endl;
    List1.display();
    List1.insert(0, "first");
    List1.display();
    List1.remove(2);
    List1.display();
    return 0;
}


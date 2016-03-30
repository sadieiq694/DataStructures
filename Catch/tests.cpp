#include <iostream>
#include "catch.hpp"
#include "basicarray.h"
#include <string>
using namespace std;

class Queue
{
private:
    BasicArray<string> strings;
    int front;
    int capacity;
    int capInc;
    int sizze;
public:
    Queue();
    void enqueue(string v);
    string dequeue();
    int size();
    bool isEmpty();
private:
    void resize(int newSize);
};
Queue::Queue()
{
    capInc = 100;
    strings = BasicArray<string>(capInc);
    capacity = capInc;
    front = -1;
    sizze = front+1;
}
void Queue::resize(int newSize)
{
    BasicArray<string> newArray(newSize);

    for (int i = 0; i < sizze; i++)
    {
        newArray[i] = strings[i];
    }
    strings = newArray;
}
void Queue::enqueue(string v)
{
    if(front+1==capacity)
    {
        resize(capacity + capInc);
    }
    for (int n = front; n >= 0 ; n--)
    {
        strings[n+1] = strings[n];
    }
    front++;
    sizze++;
    strings[0]=v;

}
string Queue::dequeue()
{
    if(capacity-sizze == capInc)
    {
        resize(capacity-capInc);
    }
    string result = strings[front];
    front--;
    sizze--;
    return result;
}
int Queue::size()
{
    return sizze;
}
bool Queue::isEmpty()
{
    return front==-1;
}


TEST_CASE("various queue tests", "[queue]") {

    Queue queue1;

    SECTION("isEmpty and size test") {
        queue1.enqueue("sadie");
        REQUIRE( queue1.isEmpty()==false);
        REQUIRE(queue1.size()==1);
    }

    SECTION("isEmpty Test") {
        REQUIRE(queue1.isEmpty()==true);
        queue1.enqueue("sadie");
        queue1.enqueue("allen");
        REQUIRE( queue1.isEmpty()==false);
        queue1.dequeue();
        queue1.dequeue();
        REQUIRE(queue1.isEmpty()==true);
    }

    SECTION("dequeue test") {
        queue1.enqueue("sadie");
        queue1.enqueue("allen");
        REQUIRE(queue1.dequeue() == "sadie");
        REQUIRE(queue1.dequeue() == "allen");
    }

}

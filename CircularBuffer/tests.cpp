#include <iostream>
#include "catch.hpp"
#include "basicarray.h"
#include <string>
using namespace std;

class Queue
{
private:
    BasicArray<double> values;
    int front;
    int back;
    int capacity;
    int sizze;
public:
    Queue();
    void enqueue(double v);
    double dequeue();
    int size();
    bool isEmpty();
private:
    void resize(int newSize);
};
Queue::Queue()
{
    capacity = 10;
    values = BasicArray<double>(capacity);
    front = 0;
    back = -1;
    sizze = 0;
}
 void Queue::resize(int newSize)
{
    BasicArray<double> newArray(newSize);
    int i;
    for (i = 0; i < sizze && front < capacity; i++)
    {
        newArray[i] = values[front++];
    }
    if (i<sizze)
    {
        for(int j = 0; j<=back; j++)
        {
            newArray[i] = values[j];
            i++;
        }
    }
    values = newArray;
    front = 0;
    back = sizze-1;
    capacity = newSize;

}
void Queue::enqueue(double v)
{
    if(sizze==capacity)
    {

        resize(2*capacity);
    }

    sizze++;
    values[++back]=v;
    if(back>=capacity-1)
    {
        back = -1;
    }

}
double Queue::dequeue()
{
    if(capacity-sizze == (3*capacity)/4)
    {
        resize(capacity/2);
    }
    double result = values[front];
    front++;
    if(front>=capacity-1)
    {
        front=0;
    }
    sizze--;
    return result;
}
int Queue::size()
{
    return sizze;
}
bool Queue::isEmpty()
{
    return sizze == 0;
}


TEST_CASE("various queue tests", "[queue]") {

    Queue queue1;

    SECTION("isEmpty and size test") {
        queue1.enqueue(4);
        REQUIRE( queue1.isEmpty()==false);
        REQUIRE(queue1.size()==1);
    }

    SECTION("isEmpty Test") {
        REQUIRE(queue1.isEmpty()==true);
        queue1.enqueue(4);
        queue1.enqueue(6);
        REQUIRE( queue1.isEmpty()==false);
        queue1.dequeue();
        queue1.dequeue();
        REQUIRE(queue1.isEmpty()==true);
    }

    SECTION("dequeue test") {
        queue1.enqueue(2);
        queue1.enqueue(1);
        REQUIRE(queue1.dequeue() == 2);
        REQUIRE(queue1.dequeue() == 1);
    }

    SECTION("circular buffer test") {
        queue1.enqueue(3);
        queue1.enqueue(4);
        queue1.enqueue(2);
        queue1.enqueue(6);
        queue1.enqueue(8);
        queue1.dequeue();
        queue1.dequeue();
        REQUIRE(queue1.dequeue()==2);
        queue1.dequeue();
        REQUIRE(queue1.size()==1);
        queue1.enqueue(6);
        queue1.enqueue(8);
        queue1.enqueue(9);
        queue1.enqueue(3);
        queue1.enqueue(0);
        queue1.enqueue(12);
        queue1.enqueue(55);
        queue1.enqueue(7);
        queue1.dequeue();
        queue1.dequeue();
        queue1.dequeue();
        queue1.dequeue();
        REQUIRE(queue1.dequeue()==3);
        queue1.dequeue();
        queue1.dequeue();
        queue1.dequeue();
        REQUIRE(queue1.size()==1);
        REQUIRE(queue1.dequeue()==7);
    }

    SECTION("resize test") {
        queue1.enqueue(1);
        queue1.enqueue(2);
        queue1.enqueue(3);
        queue1.enqueue(4);
        queue1.enqueue(5);
        queue1.enqueue(6);
        queue1.enqueue(7);
        queue1.enqueue(8);
        queue1.enqueue(9);
        queue1.enqueue(10);
        queue1.enqueue(11);
        queue1.enqueue(12);
        queue1.enqueue(13);
        queue1.enqueue(14);
        queue1.enqueue(15);
        queue1.enqueue(16);
        queue1.enqueue(17);
        queue1.enqueue(18);
        queue1.enqueue(19);
        queue1.enqueue(20);
        queue1.enqueue(21);
        queue1.enqueue(22);
        queue1.enqueue(23);
        queue1.enqueue(24);
        queue1.enqueue(25);
        queue1.enqueue(26);
        queue1.enqueue(27);
        queue1.enqueue(28);
        queue1.enqueue(29);
        REQUIRE(queue1.dequeue()==1);
        REQUIRE(queue1.dequeue()==2);
        REQUIRE(queue1.dequeue()==3);
        REQUIRE(queue1.dequeue()==4);
        REQUIRE(queue1.dequeue()==5);
        REQUIRE(queue1.dequeue()==6);
        REQUIRE(queue1.dequeue()==7);
        REQUIRE(queue1.dequeue()==8);
        REQUIRE(queue1.dequeue()==9);
        REQUIRE(queue1.dequeue()==10);
        REQUIRE(queue1.dequeue()==11);
        REQUIRE(queue1.dequeue()==12);
        REQUIRE(queue1.dequeue()==13);
        REQUIRE(queue1.dequeue()==14);
        REQUIRE(queue1.dequeue()==15);
        REQUIRE(queue1.dequeue()==16);
        REQUIRE(queue1.dequeue()==17);
        REQUIRE(queue1.dequeue()==18);
        REQUIRE(queue1.dequeue()==19);
        REQUIRE(queue1.dequeue()==20);
        REQUIRE(queue1.dequeue()==21);
        REQUIRE(queue1.dequeue()==22);
        REQUIRE(queue1.dequeue()==23);
        REQUIRE(queue1.dequeue()==24);
        REQUIRE(queue1.dequeue()==25);
        REQUIRE(queue1.dequeue()==26);
        REQUIRE(queue1.dequeue()==27);
        REQUIRE(queue1.dequeue()==28);
        REQUIRE(queue1.dequeue()==29);
        REQUIRE(queue1.size()==0);
    }


}


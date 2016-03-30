#include <iostream>
#include "basicarray.h"

using namespace std;

class MyStack
{
   private:
    BasicArray<double> values;
    int size;
    int capacity;
    int capInc;
   public:
    MyStack();
    void   push(double v);
    double pop();
    bool   isEmpty();
    double top();
   private:
    void resize(int newSize);
};

MyStack::MyStack()
{
    capInc = 100;
    values = BasicArray<double>(capInc);
    capacity = capInc;
    size = 0;


}

void MyStack::resize(int newSize)
{
    BasicArray<double> newArray(newSize);

    for (int i = 0; i < size; i++)
    {
       newArray[i] = values[i];
    }

    values = newArray;

    capacity = newSize;
}

void   MyStack::push(double v)
{
    if (size == capacity)
    {
        resize(capacity + capInc);
    }
    values[size++] = v;
}

double MyStack::pop()
{
    double result = values[--size];
    if(capacity - size == capInc)
    {
        resize(capacity - capInc);
    }
    return result;
}

bool   MyStack::isEmpty()
{
    return size == 0;
}

double MyStack::top()
{
    return values[size-1];
}

int main()
{
    // test-driven design


    MyStack st;

    cout << "Should Return Empty: " << (st.isEmpty() ? "Empty" : "Not Empty") << endl;

    st.push(10);

    cout << "Top should be 10: " << st.top() << endl;

    st.push(11);



    st.push(14);
    st.push(2);



    cout << "Should Return Not Empty: " <<  (st.isEmpty() ? "Empty" : "Not Empty") << endl;

    cout << "Should Return 2: " <<  st.pop() << endl;
    cout << "Should Return 14: " <<  st.pop() << endl;

    cout << "Top should be 11: " << st.top() << endl;


    cout << "Should Return 11: " <<  st.pop() << endl;
    cout << "Should Return 10: " << st.pop() << endl;

    cout << "Should Return Empty: " <<  (st.isEmpty() ? "Empty" : "Not Empty") << endl;

    MyStack stack;
    for (int i=0;i<=10;i++)
    {
        stack.push(i);
    }

    bool success = true;
    for (int i=10;i>=0;i--)
    {

        if(stack.pop()!=i)
        {
            cout<< "ERROR" << endl;
            success = false;
        }

    }
    if (success = true)
    {
        cout << "it worked!!!!" << endl;
    }

    return 0;
}


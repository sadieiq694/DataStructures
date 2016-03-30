#include <iostream>
#include <vector>
#include <chrono>
#include <random>

using namespace std;
template<typename T>
bool isSorted(std::vector<T> values)
{
    for (int i=0;i<values.size()-1;i++)
    {
        if (values[i] > values[i+1])
        {
            return false;
        }
    }

    return true;
}

class randomGenerator
{
private:
    std::random_device randDevice;
    std::mt19937 mersenneTwister;
public:
    randomGenerator(int seed = 0); // seed == 0 will use system clock for seed
    int getInt(int minVal, int maxVal);
    double getDouble(double minVal, double maxVal);
    std::string getString(int minLength = 8, int maxLength = 8, const std::string& symbols = "abcdefghijklmnopqrstuvwxyz");
};
randomGenerator::randomGenerator(int seed) : mersenneTwister(randDevice())
{
    if (seed)
    {
        mersenneTwister.seed(seed); // fixed seed to get repeatable pseudorandom values for testing
    }
    else
    {
        mersenneTwister.seed((std::chrono::system_clock::now().time_since_epoch()).count());
    }
}

int randomGenerator::getInt(int minVal, int maxVal)
{
    std::uniform_int_distribution<int> dist(minVal, maxVal);
    return dist(mersenneTwister);
}

double randomGenerator::getDouble(double minVal, double maxVal)
{
    std::uniform_real_distribution<double> dist(minVal, maxVal);
    return dist(mersenneTwister);
}

std::string randomGenerator::getString(int minLength, int maxLength, const std::string& symbols)
{
    std::uniform_int_distribution<int> dist(0, symbols.size()-1);
    std::string txt(getInt(minLength, maxLength), 'x');
    for (size_t i=0;i<txt.size();i++)
    {
        txt[i] = symbols[dist(mersenneTwister)];
    }
    return txt;
}

#include <functional>

template<typename T>
std::vector<T> buildVec(int count, function<T()> generator)
{
    std::vector<T> values(count);

    for (int i=0;i<count;i++)
    {
        values[i] = generator();
    }

    return std::move(values);
}

bool isPartitioned(vector<double>& values, int pivotIndex)
{
    for (int i = 0; i < pivotIndex; i++)
    {
        if (values[i] > values[pivotIndex])
        {
        return false;
        }
    }
    for (int i = pivotIndex+1; i < values.size(); i++)
    {
        if (values[i] <= values[pivotIndex])
        {
        return false;
        }
    }
    return true;
}

int partition(int start, int end, vector<double>& values)
{
    int size = end - start+1;
    int pivot = start + (size/2-1);
    int pivotVal=values[pivot];
    values[pivot]=values[start];
    values[start]=pivotVal;
    pivot = start;
    for(int i = start+1; i<=end; i++)
    {
        if(values[i]>values[pivot])
        {

        }
        else if(values[i]<=values[pivot])
        {
            if(i==pivot+1)
            {
                int iVal = values[i];
                values[i]=values[pivot];
                values[pivot]=iVal;
                pivot++;
            }
            else if(i>pivot+1)
            {
                int pivVal = values[pivot];
                values[pivot]=values[pivot+1];
                values[pivot+1]=pivVal;
                pivot++;
                int oldPivPlaceVal = values[pivot-1];
                values[pivot-1]=values[i];
                values[i]=oldPivPlaceVal;
                //switch pivot with number on its immediate right(change pivot index), then switch i with the number on the old index of pivot
            }
        }
     }
    return pivot;
}


class Functor
{
    randomGenerator &foo;
public:
    Functor(randomGenerator& f) : foo(f) {}

    double operator()(int q) { return foo.getDouble(1,100); }
};

int main()
{
    vector<double> values {5,7,2,8,-1,9,300000,1,0,11,88,4,6,3};
    randomGenerator random;

    for (int x = 0; x < 100; x++)
    {

    vector<double> newVec = buildVec<double>(20,[&random](){return random.getDouble(1,50);});
    int p = partition(0,newVec.size()-1, newVec);
    if (isPartitioned(newVec, p))
    {
        cout<< "yayyy!" << endl;
    }
    else
    {
        cout<< "you are a failure... just quit now" << endl;
    }

    }
    return 0;
}



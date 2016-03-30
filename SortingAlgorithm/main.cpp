#include <iostream>
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
void mySortRecursive(int start, int end, vector<double>& values)
{
    int size= end- start + 1;
    if(size<=2)
    {
        if(values[start]<values[end])
        {
            return; // Maybe?
        }
        else if(values[start]>values[end])
        {
            int small = values[end];
            values[end] = values[start];
            values[start] = small;
            return;
        }
    }
    if(size>2)
    {
        int halfIndex=(start+end)/2;
        mySortRecursive(start, halfIndex, values);
        mySortRecursive(halfIndex+1, end, values);
        int leftIndex = start;
        int rightIndex = halfIndex+1;
        vector<double> sortedVec;
        while(size>sortedVec.size())
        {
            if(leftIndex==halfIndex+1)
            {
                for(int i= rightIndex; i<values.size(); i++)
                {
                    sortedVec.push_back(values[i]);
                }
            }
            else if(rightIndex==end+1)
            {
                for(int i= leftIndex; i<=halfIndex; i++)
                {
                    sortedVec.push_back(values[i]);
                }
            }
            else if(values[leftIndex]<=values[rightIndex])
            {
                sortedVec.push_back(values[leftIndex]);
                leftIndex++;
            }
            else
            {
                sortedVec.push_back(values[rightIndex]);
                rightIndex++;
            }

        }
        for(int i = 0; i<size; i++)
        {
            values[start+i] = sortedVec[i];
        }
    }
}
void mySort(vector<double>& values)
{
    mySortRecursive(0,values.size()-1, values);
}



int main()
{
    //randomGenerator rand(10);

    //auto stuff = buildVec<string>(10, [&]() { return rand.getString(8,8); } );

    vector<double> plzSort {1,3,7,300,90,5.7,2,-3,4,2};
    mySort(plzSort);

    for (auto v : plzSort)
    {
        cout << v << endl;
    }

    return 0;
}

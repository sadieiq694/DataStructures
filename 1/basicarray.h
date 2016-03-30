#ifndef BASICARRAY
#define BASICARRAY

#include <memory>

template<typename T>
class BasicArray
{
private:
    std::unique_ptr<T[]> values;
    int count;
public:
    BasicArray(int size = 0) : values(new T[size]), count(size) {}
    BasicArray(const BasicArray<T>& other) { *this = other; }
    T& operator[] (int idx) { return values[idx]; }
    BasicArray<T>& operator= (const BasicArray<T>& other);
};

template<typename T>
BasicArray<T>& BasicArray<T>::operator= (const BasicArray<T>& other)
{
    values.reset(new T[other.count]);
    count = other.count;
    for (int i=0;i<count;i++)
    {
        values[i] = other.values[i];
    }
    return *this;
}

#endif // BASICARRAY


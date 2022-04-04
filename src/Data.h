#pragma once
#include <string>
class Data
{
private:
    /* data */
public:
    Data(/* args */);
    Data( int i){
        this->i = i;
    }
    virtual ~Data();

    void Recv();

    int test2parm(int a, std::string c);

    int i = 0;
};



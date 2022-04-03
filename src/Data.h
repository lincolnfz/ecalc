#pragma once
#include <string>
class Data
{
private:
    /* data */
public:
    Data(/* args */);
    virtual ~Data();

    void Recv();

    int test2parm(int a, std::string c);
};



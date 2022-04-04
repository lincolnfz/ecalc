#include "Data.h"
#include <stdio.h>
#include <iostream>

Data::Data(/* args */)
{
    int i = 0;
}

Data::~Data()
{
    std::cout << "~Data call" << i <<  std::endl;
}

void Data::Recv(){
    fprintf(stderr,  "11data recv call \n");
    int i = 0;
}

int Data::test2parm(int a, std::string c){
    return 99;
}
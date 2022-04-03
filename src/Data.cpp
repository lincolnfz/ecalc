#include "Data.h"
#include <stdio.h>

Data::Data(/* args */)
{
    int i = 0;
}

Data::~Data()
{
}

void Data::Recv(){
    fprintf(stderr,  "11data recv call \n");
    int i = 0;
}

int Data::test2parm(int a, std::string c){
    return 99;
}
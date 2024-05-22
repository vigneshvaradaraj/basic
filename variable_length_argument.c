#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void print(int count,...)
{
    int i = 0,int_var;
    double float_var;
    char* pt;
    va_list vList;

    va_start(vList,count);

    int_var = va_arg(vList,int);
    float_var = va_arg(vList,double);
    pt = va_arg(vList,char*);

    printf("%d %f %s\n",int_var,(float)float_var,pt);

    va_end(vList);
}

void test_fun(int line[])
{

}

void main(int n,char* ar[])
{
    int* pt;
    print(3,56,9.8,"gfhf");
    test_fun(pt);
}
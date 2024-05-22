#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>

int n = 0;
char* ptr[3] = {NULL};

int main(int num,char* str[])
{
    if(num > 1)
    {
        n = atoi(str[1]);
        printf("%d\n",n);
    }
    for(n = 0;n<2;n++)
    ptr[n] = (char*)malloc(10 * sizeof(char));

    if(ptr == NULL)
    {
        printf("it is null!\n");
    }
    else
    {
        for(n = 0;n<2;n++)
        {
            printf("enter string: ");
            scanf("%s",ptr[n]);
        }
        for(n = 0;n<2;n++)
        {
            printf("%s\n",ptr[n]);
        }

        for(n = 0;n<2;n++)
        free(ptr[n]);
    }
    return 0;
}



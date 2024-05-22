#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void main(int cnt,char* st[])
{
    FILE* pf = fopen(st[1],"w");

    if(pf != NULL)
    {
        printf("fle has been created succefully!\n");
    }

    fprintf(pf,"nothing in file!\n");

    if(remove(st[1]) == 0)
    printf("file deleted successfully!\n");
    else
    printf("file not deleted!\n");

    fclose(pf);
}
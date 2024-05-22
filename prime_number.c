#include <stdio.h>
#include <stdlib.h>

int main(int n, char* st[])
{
    int i,num = atoi(st[1]);
    int isPrime= 1;
    
    
    // 0, 1 and negative numbers are not prime
    if(num < 2)
    {
        isPrime = 0;
    }
    else
    {   
        // shouldn't have any divisors in b/w 2 & num-1 
        for(i=2; i < num; i++)
        {
            if(num % i == 0)
            {
                isPrime = 0;
                break;
            }
        }
    }

    if(isPrime)
        printf("%d is Prime", num);
    else
        printf("%d is not Prime", num);
 
    return 0;
}
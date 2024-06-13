#include <stdio.h>
//selection shorting
int array[10] = {45,1,7,2,200,3,67,190,5,289};

int len = 10;
int i,j,temp;

int* select_short(char len)
{
   for(i = 0;i < len;i++)
   {
	   for(j = i+1; j < 10;j++)
	   {
		   if(array[i] > array[j])
		   {
			    temp = array[i];
				array[i] = array[j];
				array[j] = temp;
		   }
	   }
   }
   return array;
   
}

void main()
{
	printf("%d\n",sizeof(void));
   for(i = 0;i < 10;i++)
   {
	   printf("%d ",array[i]);
   }
   printf("\n");
   select_short(10);
   
   for(i = 0;i < 10;i++)
   {
	   printf("%d ",array[i]);
   }
   printf("\n");

}
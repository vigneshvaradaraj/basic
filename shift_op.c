#include<stdio.h>

void print_bit(int* pt)
{
   int x = 31; 
   for(;x >= 0;x--)
   {
      printf("%d ",(*pt>>x) & 1);
   }
   printf("\n");
}

int main()
{
  int num = 0x80000000;
  print_bit(&num);
  printf("%d\n",num);
  num >>=1;
  print_bit(&num);
  printf("%d\n",num);
  num >>=1;
  print_bit(&num);
  printf("%d\n",num);
  num >>=1;
  print_bit(&num);
  printf("%d\n",num);
  num >>=1;
  print_bit(&num);
  printf("%d\n",num);
  return 0;
}
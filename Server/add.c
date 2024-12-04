#include "libheaders.h"
#include "limits.h"
//math
int add(int a, int b){

    int ret =a+b;
    return ret;
}
int multiply(int a, int b)
{
    int ret = a*b;
    return ret;
}
float max_array(float *array, int size)
{
  float max =(float)INT_MIN;
  for(int i = 0; i < size ;i++)
  {
    if(max < array[i])
        max = array[i];
  }
  return max;
}
// string

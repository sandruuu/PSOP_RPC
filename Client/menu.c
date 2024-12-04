#include "menu.h"

void menu_list(TCPConnection* connection)
{
    printf("\n-----------------------------\n");
    printf("Functions list: \n");
    printf("1) int add(int a, int b)\n");
    printf("2) int multiply(int a, int b)\n");
    printf("3) float max_array(float* array, int size)");
    printf("\n-----------------------------\n");
    printf("Choose function:\n");
    
}

void choose_function(TCPConnection* connection)
{
    int function = 0;
    scanf("%d",&function);
    PacketType pckT;

    printf("\n-----------------------------\n");
    printf("Choose call mode:\n");
    printf("1) ASYNCRON\n");
    printf("2) SYNCRON\n");
    printf("\n-----------------------------\n");
    scanf("%u",(unsigned int*)(&pckT));
    pckT--;

    int input1 = 0;
    int input2 = 0;
    int response = 0;

    switch (function)
    {
    case 1:
        printf("a = ");
        scanf("%d",&input1);
        printf("b = ");
        scanf("%d",&input2);

        if(pckT == SENDSYNC)
            {
                response = add(connection,input1,input2);
                printf("%d + %d = %d",input1,input2,response);
            }    
            else 
                if(pckT == SENDASYNC)
                    add_async(connection,input1,input2);
           
        
        break;
    case 2:
        char c;

        printf("a = ");
        scanf("%d",&input1);
        printf("b = ");
        scanf("%d",&input2);

        if(pckT == SENDSYNC)
            {
                response = multiply(connection,input1,input2);
                printf("%d * %d = %d",input1,input2,response);
            }
            else 
                if(pckT == SENDASYNC)
                    multiply_async(connection,input1,input2);
           

        break;
    case 3:
        float array[100] ={0};
        int size = 0;
        int i = 0;
        printf("array = ");
        do{
            scanf("%f",&array[i]);
            i++;
            c = getchar(); 
    } while (c != '\n' && i < 100);
        
       size = sizeof(array) / sizeof(float);
        if(pckT == SENDSYNC)
        {
            response = max_array(connection,array,size);
            printf("max = response");
        }
        else 
            if(pckT == SENDASYNC)
                max_array_async(connection,array,size);
        
            
        break;
    default:
        break;
    }
}

void test(TCPConnection *connection)
{
    if(continue_)
        add_async(connection,10,29);
    if(continue_)   
    {
        add_async(connection,21,43);
    }
    sleep(2);
    int a;
    if(continue_)
    {
        a = add(connection,2,10);
        printf("a = %u\n",a);
    }
    if(continue_)
        add_async(connection,10,9);
    if(continue_)
        add_async(connection,32,11);
    if(continue_)
        multiply_async(connection,2,3);
    if(continue_)
    {
        a = multiply(connection,4,5);
        printf("a = %u\n",a);
    }
    float array[] = {1.1,2.3,6.8,5,6.7};
    int size = sizeof(array) / sizeof(float);
     if(continue_)
    {
        float max = max_array(connection,array,size);
        printf("max = %.2f\n",max);
    }
    if(continue_)
        max_array_async(connection,array,size);
}

void HandleSignal(int sig)
{
   
    if(sig == SIGINT)
    {
        pthread_mutex_lock(&count_mutex);
        
        while(thread_count_reached != THREAD_SIZE + 1)
            pthread_cond_wait(&cond_stop,&count_mutex);

        continue_ = false;
        
        while (stop != THREAD_SIZE + 1) {
            pthread_cond_wait(&cond_stop, &count_mutex);
        }
        pthread_mutex_unlock(&count_mutex);
        
    }
}

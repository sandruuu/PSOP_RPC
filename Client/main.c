#include "menu.h"

int main(){

    TCPConnection* connection;
   
    struct sigaction signalAcc;
    sigemptyset(&signalAcc.sa_mask);
    signalAcc.sa_handler = HandleSignal;
    sigaction(SIGINT,&signalAcc,NULL);

    initConnection(&connection);

    //initilizeaza bariera care sincronizeaza stergerea cererilor cu gestionarea acetora 
    initialize_barrier();

    //creem threadurile care trimiterea de requesturi
    create_thread((void *)connection);

    //creem threadul care se ocupa de stergerea cererilor la care s-a primit raspuns
    pthread_t cleanup_thread;
    pthread_create(&cleanup_thread, NULL, clearQueueOfProcessedPackets, NULL);

    //testarea trimiterii de cereri si primirea de raspunsuri in mod sincron si asincron 
    test(connection);
   
    //asteptam terminarea executiei threadurilor
    waitThreads();
    pthread_join(cleanup_thread,NULL);
    
    CloseConnection(connection);
    

    return 0;
}
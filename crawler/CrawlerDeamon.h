#ifndef JOBEXECUTOR_H
#define JOBEXECUTOR_H
#include "ListOfWorkersDir.h"
#include "Map.h"
#include "Worker.h"
#include "NetworkFunctions.h"
#include "FifoFD.h"
#include "Statistics.h"

class CrawlerDeamon {
public:
    CrawlerDeamon( char*, int, int, int);
    ~CrawlerDeamon();
    void initialize();
    void createThreads();
    void stats();
    void Exit(char*);
    void restart(pid_t who);
    
    void enqueue(char * uri);
    void submit(int fd);

    int getCommandfd() ;
    void setCommandfd(int commandfd);
    
    static Statistics statistics;
    static bool online;
    static FifoFD fifoFD;
    static        char* savedirName;

private:
    int servingport;
    int commandport;
    int commandfd;   
    int numOfThreads;
    ListOfThreads** arrayWithListOfThreads; 
    NetworkFunctions messenger;
       long int startms;
    long int endms;

    int openListeningSocket(int port);
    void perror_exit(const char *message) ;
   
};
#endif /* JOBEXECUTOR_H */


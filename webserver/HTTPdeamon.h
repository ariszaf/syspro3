#ifndef JOBEXECUTOR_H
#define JOBEXECUTOR_H
#include "ListOfWorkersDir.h"
#include "Map.h"
#include "Worker.h"
#include "NetworkFunctions.h"
#include "FifoFD.h"
#include "Statistics.h"

class HTTPdeamon {
public:
    HTTPdeamon(char*, int, int, int);
    ~HTTPdeamon();
    void initialize();
    void createThreads();
    void stats();
    void Exit(char*);
    void restart(pid_t who);

    void enqueue(int fd);
    void submit(int fd);

    int getCommandfd();
    void setCommandfd(int commandfd);

    int getServingfd();

    void setServingfd(int servingfd);

    static Statistics statistics;
    static bool online;
    static FifoFD fifoFD;
    static char* dirName;

private:
    int servingport, commandport;
    int servingfd, commandfd;
    int numOfThreads;
    ListOfThreads** arrayWithListOfThreads;
    NetworkFunctions messenger;

    int openListeningSocket(int port);
    void perror_exit(const char *message);
    long int startms;
    long int endms;

};
#endif /* JOBEXECUTOR_H */


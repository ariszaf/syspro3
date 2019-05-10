#ifndef MAPOFWORKERS_H
#define MAPOFWORKERS_H

#include <unistd.h>
#include <pthread.h>

class ListOfThreads {
private:

    class FilePathNode {
    public:
        char* filePath;
        FilePathNode * next;
    };
public:
    pthread_t threadid;
    int lengthOfPaths;
    char* merge();
    ListOfThreads();
    ~ListOfThreads();
    bool hasCompletedCmdCommunication;
    FilePathNode* WorkerNodeHeadPtr;

};



#endif /* MAPOFWORKERS_H */


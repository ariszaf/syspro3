

#ifndef STATISTICS_H
#define STATISTICS_H

#include <signal.h>


class Statistics {
public:
    Statistics();
    virtual ~Statistics();
    void setBytes(int bytes);

    void setPages(int pages);
    char* convertToString(long int);
                    
    pthread_mutex_t lock;
private:
    long int bytes;    
    int pages;
};

#endif /* STATISTICS_H */




#ifndef FIFOFD_H
#define FIFOFD_H

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define SIZE_LIMIT 6

class FifoFD {
private:

    class Node {
    public:
        int fd;
        Node * next;
    };
    
    pthread_mutex_t mtx;
    pthread_cond_t cond_nonempty;

public:
    Node * frontPtr;
    Node * rearPtr;
    int numOfItems;
    FifoFD();
    virtual ~FifoFD();
    void place(int fd);
    int obtain();
    void addItem(int fd);
    int removeItem();
    bool isEmpty();
    void display();
};

#endif /* FIFOFD_H */


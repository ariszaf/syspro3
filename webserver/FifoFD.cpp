#include "FifoFD.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

FifoFD::FifoFD() {
    numOfItems = 0;
    frontPtr = NULL;
    rearPtr = NULL;
}

FifoFD::~FifoFD() {
    Node* temp = frontPtr;
    while (temp != NULL) { // 
        Node* t1 = temp;
        temp = temp->next;
        delete t1;
    }
}

bool FifoFD::isEmpty() {
    if (numOfItems) {
        return false;
    } else {
        return true;
    }
}

void FifoFD::place(int fd) {
    pthread_mutex_lock(&mtx);

    addItem(fd);

    pthread_cond_broadcast(&cond_nonempty);

    pthread_mutex_unlock(&mtx);
}

int FifoFD::obtain() {
    pthread_mutex_lock(&mtx);
    while (isEmpty()) {
        pthread_cond_wait(&cond_nonempty, &mtx);
    }

    int fd =removeItem();

    pthread_cond_broadcast(&cond_nonempty);

    pthread_mutex_unlock(&mtx);

    return fd;

    return 0;
}

//http://btechsmartclass.com/DS/U2_T9.html

void FifoFD::addItem(int fd) {
    Node * newNode = new Node();
    if (isEmpty()) {
        newNode->fd = fd;
        newNode->next = NULL;
        frontPtr = newNode;
        rearPtr = newNode;
    } else {
        newNode->fd = fd;
        newNode->next = NULL;
        rearPtr->next = newNode;
        rearPtr = newNode;

    }
    numOfItems++;
}

int FifoFD::removeItem() {
    if (!isEmpty()) {
        Node* temp;
        int fd;
        fd = frontPtr->fd;
        temp = frontPtr;
        frontPtr = frontPtr->next;
        delete temp;
        numOfItems--;
        return fd;
    } else {
        cout<<" can not remove, is empty "<<endl;   
        return -1;
    }
}

void FifoFD::display() {
    Node* tempPtr = frontPtr;
    while (tempPtr) {
        cout << "fd " << tempPtr->fd << endl;
        tempPtr = tempPtr->next;
    }

    cout << "-----------------" << endl;
}

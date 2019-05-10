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

void FifoFD::place(char * url) {
    pthread_mutex_lock(&mtx);

    addItem(url);

    pthread_cond_broadcast(&cond_nonempty);

    pthread_mutex_unlock(&mtx);
}

char * FifoFD::obtain() {
    pthread_mutex_lock(&mtx);
    while (isEmpty()) {
        pthread_cond_wait(&cond_nonempty, &mtx);
    }

    char * uri =removeItem();

    pthread_cond_broadcast(&cond_nonempty);

    pthread_mutex_unlock(&mtx);

    return uri;
}

//http://btechsmartclass.com/DS/U2_T9.html

void FifoFD::addItem(char * url) {
    Node * newNode = new Node();
    if (isEmpty()) {
        newNode->url = url;
        newNode->next = NULL;
        frontPtr = newNode;
        rearPtr = newNode;
    } else {
        newNode->url = url;
        newNode->next = NULL;
        rearPtr->next = newNode;
        rearPtr = newNode;

    }
    numOfItems++;
}

char * FifoFD::removeItem() {
    if (!isEmpty()) {
        Node* temp;
        char * url;
        url = frontPtr->url;
        temp = frontPtr;
        frontPtr = frontPtr->next;
        delete temp;
        numOfItems--;
        return url;
    } else {
        cout<<" can not remove, is empty "<<endl;   
        return 0;
    }
}

void FifoFD::display() {
    Node* tempPtr = frontPtr;
    while (tempPtr) {
        cout << "url " << tempPtr->url << endl;
        tempPtr = tempPtr->next;
    }

    cout << "-----------------" << endl;
}

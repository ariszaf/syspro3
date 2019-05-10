#include <cstring>
#include <fstream>
#include <cstdio>
#include <cstdlib>


#include "Statistics.h"

Statistics::Statistics() : bytes(0), pages(0) {
    pthread_mutex_init(&lock, 0);
}

Statistics::~Statistics() {
    pthread_mutex_destroy(&lock);
}

char* Statistics::convertToString(long int time) {
    char* totalRes;
    int resultLenght = 130;
    
    totalRes = new char [resultLenght]();
//  https://stackoverflow.com/questions/10874048/from-milliseconds-to-hour-minutes-seconds-and-milliseconds
    char* chartime;
    chartime = (char*) malloc(sizeof time + 1); //   https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(chartime, "%ld", time);

    long int seconds = (time / 1000) % 60;
    char* charseconds;
    charseconds = (char*) malloc(sizeof seconds + 1); //   https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charseconds, "%ld", seconds);

    long int minutes = (time / (1000 * 60)) % 60;
    char* charminutes;
    charminutes = (char*) malloc(sizeof minutes + 1); //   https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charminutes, "%ld", minutes);

    char* timer = new char [40]();
    strcat(timer, charminutes);
    strcat(timer, ":");
    strcat(timer, charseconds);
    strcat(timer, ".");
    strcat(timer, chartime);


    pthread_mutex_lock(&lock);

    char* charbytes;
    charbytes = (char*) malloc(sizeof bytes + 1); //   https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charbytes, "%ld", bytes);

    char* charpages;
    charpages = (char*) malloc(sizeof pages + 1); //   https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charpages, "%d", pages);

    pthread_mutex_unlock(&lock);

    strcat(totalRes, "Crawler up for ");
    strcat(totalRes, timer);
    strcat(totalRes, ", served ");
    strcat(totalRes, charpages);
    strcat(totalRes, " pages, ");
    strcat(totalRes, charbytes);
    strcat(totalRes, " bytes");

    return totalRes;
}

void Statistics::setBytes(int bytes) {
    pthread_mutex_lock(&lock);
    this->bytes += bytes;
    pthread_mutex_unlock(&lock);

}

void Statistics::setPages(int pages) {
    pthread_mutex_lock(&lock);
    this->pages += pages;
    pthread_mutex_unlock(&lock);
}


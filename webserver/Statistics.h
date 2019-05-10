/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Statistics.h
 * Author: 
 *
 * Created on May 26, 2018, 7:16 PM
 */

#ifndef STATISTICS_H
#define STATISTICS_H

#include <signal.h>


class Statistics {
public:
    Statistics();
    virtual ~Statistics();
    void setBytes(long int bytes);

    void setPages(int pages);
    char* convertToString(long int);
                    
    pthread_mutex_t lock;
private:
    long int bytes;    
    int pages;
};

#endif /* STATISTICS_H */


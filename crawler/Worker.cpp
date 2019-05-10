#include "Worker.h"
#include "Map.h"
#include "Trie.h"
#include "SearchRes.h"
#include "PostingList.h"
#include "NetworkFunctions.h"
#include "MaxMinCountRes.h"
#include "WC.h"
#include "CrawlerDeamon.h"
#include "ResultParser.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h> 
#include <unistd.h>
using namespace std;

ofstream ofs;
Map ** mapArray;

Worker::Worker() {

}

Worker::Worker(int e2wfd, int w2efd, int workerNum) : e2wfd(e2wfd), w2efd(w2efd) {
    numOffiles = 0;
    successfulSearches = 0;
    struct stat st = {0}; //https://stackoverflow.com/questions/7430248/creating-a-new-directory-in-c

    char* charWorkerNum;
    charWorkerNum = (char*) malloc(sizeof workerNum + 1); //  https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charWorkerNum, "%d", workerNum);

    char charPid[100];
    //charPid = (char*) malloc(sizeof pid +1);              // https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charPid, "%d", getpid());

    logDir = new char [strlen(" log/worker/ .txt  ") + strlen(charWorkerNum) + strlen(charPid) + 1]();

    if (stat("log", &st) == -1) {
        mkdir("log", 0700);
    }

    strcat(logDir, "log/");
    strcat(logDir, "worker");


    strcat(logDir, charWorkerNum);

    if (stat(logDir, &st) == -1) {
        mkdir(logDir, 0700);
    }

    strcat(logDir, "/");

    //    char pid[1024];
    //    sprintf(pid, "%d", getpid());

    strcat(logDir, charPid);
    strcat(logDir, ".txt");

    ofstream ofs(logDir);
    free(charWorkerNum);
    //  ofs << logDir << endl;
}

Worker::~Worker() {
//    close(e2wfd);
//    close(w2efd);
//
//    if (ofs.is_open()) {
//        ofs.close();
//    }
//
//    for (int i = 0; i < numOffiles; i++) {
//        delete mapArray[i];
//    }
//    delete[] mapArray;
//
//    delete[] logDir;
//    delete trie;
}

bool Worker::receivePaths() { //lamvanei ta paths
    char * payload = messenger.receiveMessage(e2wfd);
    // cout << "Worker " << getpid() << " received payload " << payload << endl;
    char* str = new char [strlen(payload) + 1];
    strcpy(str, payload);

    char * pch;
    pch = strtok(str, " \n");

    numOffiles += countNumOfFiles(pch); //metrisi gia to posa arxeia exei na analavei

    while (pch != NULL) {
        pch = strtok(NULL, " \n");
        if (pch != NULL) {
            numOffiles += countNumOfFiles(pch);
        }
    }

    if (numOffiles == 0) {
        cout << "Wrong directory or empty" << endl;
        delete[] str;
        return false;
    }

    //dimioyrgia enos pinaka apo maps
    mapArray = new Map*[numOffiles]; //!!!! //https://stackoverflow.com/questions/20303820/creation-of-dynamic-array-of-dynamic-objects-in-c


    int i = 0;
    char* fil;
    fil = strtok(payload, " \n");
    i += importFilesToMaps(fil, i);

    while (i < numOffiles && fil != NULL) {
        fil = strtok(NULL, " \n");
        if (fil != NULL) {
            i = importFilesToMaps(fil, i); //dimioyrgia twn maps, ena map gia kathe arxeio
        }
    }
    delete [] payload;
    delete [] str;

    return true;
}

int Worker::importFilesToMaps(char* pch, int i) {
    DIR *dir_ptr; //mema pdf
    struct dirent *direntp;
    if ((dir_ptr = opendir(pch)) == NULL)
        fprintf(stderr, "cannot open %s \n", pch);

    else {
        while ((direntp = readdir(dir_ptr)) != NULL) {
            //printf("%s\n", direntp->d_name);
            if (strcmp(direntp->d_name, ".") != 0 && strcmp(direntp->d_name, "..") != 0) {
                char *fullPath = new char [strlen(direntp->d_name) + strlen(pch) + 2]();
                strcat(fullPath, pch);
                strcat(fullPath, "/");
                strcat(fullPath, direntp->d_name);
                mapArray[i] = new Map(fullPath);
                i++;
                delete [] fullPath;
            }
        }
        closedir(dir_ptr);
    }
    return i;
}

int Worker::countNumOfFiles(char* pch) { //pigi , simeiwseis Mema
    int counter = 0;
    DIR *dir_ptr;
    struct dirent *direntp;
    if ((dir_ptr = opendir(pch)) == NULL)
        fprintf(stderr, "cannot open %s \n", pch);

    else {
        while ((direntp = readdir(dir_ptr)) != NULL) {
            if (strcmp(direntp->d_name, ".") != 0 && strcmp(direntp->d_name, "..") != 0) {
                counter++;
            }
        }
        closedir(dir_ptr);
    }
    return counter;
}

bool Worker::importData() {
    bool import = false;
    trie = new Trie();
    for (int i = 0; i < numOffiles; i++) {
        if (mapArray[i]->readFile()) { //diavasma tou arxeiou, prin to perasma twn keimenwn se enan pinaka ginetai elegxos gia lathi sto keimeno 
            trie->insertDoc(mapArray[i]); //isagogi sto trie olwn twn leksewn apo ola ta maps
            //  mapArray[i]->print();
            import = true;
        }
    }
    return import;
}

void Worker::start() {
    cout << "worker " << pthread_self() << " etoimos gia iserxomena minimata " << endl;

    while (1) {
        // obtain from fifo
        char * uri = CrawlerDeamon::fifoFD.obtain();

        cout << "worker " << pthread_self() << " consumed " << uri << endl;

        if (uri == 0) {
            return;
        }
        
        // ...
        
        
        
//
//        // read GET request
//        char * header = messenger.readHTTPheader(uri);
//
//        cout << ("--------------------- \n");
//        cout << header;
//        cout << ("--------------------- \n");
//
//        char* URL;
//
//
//        char *r;
//        r = strchr(header, '\r');
//        if (strchr(header, '\r')) {
//            *r = ' ';
//        }
//        r = strchr(header, '\r');
//        while (r) { //https://stackoverflow.com/questions/16677800/strtok-not-discarding-the-newline-character 
//            *r = ' ';
//            r = strchr(header, '\r');
//        }
//
//        char *token;
//        char *rest = header;
//        bool fhost = false;
//        int position = 1;
//        int line = 0;
//        ResultParser *resParser = new ResultParser();
//        bool bdcheck = false;
//
//        while ((token = strtok_r(rest, "\n", &rest)) && bdcheck != true) {
//
//            if (line == 0) {
//                //ex strcmp(argv[1], "-p") == 0
//                //an einai get
//                //while mexri na vreis to arni
//                //prepei na kaliptontai ta zitoumena,  an oxi tote akyro
//                cout << token << endl;
//                char *rest1 = token;
//
//                token = strtok_r(rest1, " ", &rest1);
//                cout << token << endl;
//                if (position == 1 && strcmp(token, "GET") == 0) {
//                    while (token) {
//
//                        token = strtok_r(rest1, " ", &rest1);
//                        cout << token << endl;
//                        position++;
//                        if (token != NULL) {
//                            if (position == 2) {
//                                if (strlen(token) > 3) {
//                                    URL = new char [strlen(token) + 1];
//                                    strcpy(URL, token);
//                                } else {
//                                    cout << "error to deytero token einai mikrotero apo 3" << endl;
//                                    //send error 400 bad request
//                                    char* response = resParser->error400();
//                                    messenger.sendTcpMessage(uri, response);
//                                    bdcheck = true;
//                                    break;
//                                }
//                            } else if (position == 3) {
//                                if (strcmp(token, "HTTP/1.1") == 0) {
//                                    cout << " p3" << endl;
//                                } else {
//                                    cout << "error to trito token den einai to to http" << endl;
//
//                                    //send error 400 bad request
//                                    char* response = resParser->error400();
//                                    messenger.sendTcpMessage(uri, response);
//                                    bdcheck = true;
//                                    break;
//                                }
//                            } else if (position > 3) {
//                                cout << " >p3" << endl;
//                                //send error 400 bad request
//                                cout << "error to get exei epipleon apo 2 " << endl;
//                                char* response = resParser->error400();
//                                messenger.sendTcpMessage(uri, response);
//                                bdcheck = true;
//                                break;
//
//                            }
//                        } else if (position <= 3 && token == NULL) {
//                            cout << "error ftasame se null " << endl;
//                            //send error 400 bad request 
//                            char* response = resParser->error400();
//                            messenger.sendTcpMessage(uri, response);
//                            bdcheck = true;
//                            break;
//                        }
//                    }
//                } else if (position == 1 && strcmp(token, "GET") != 0) {
//                    cout << "error to token den einai sin prwti thesi" << endl;
//                    //send error 400 bad request
//                    char* response = resParser->error400();
//                    messenger.sendTcpMessage(uri, response);
//                    bdcheck = true;
//                    break;
//                }
//
//                //tha tsekareis mexri na vreis to host , if dld
//                //kai tote to antistixo pio panw
//            } else {
//                char *rest1 = token;
//                token = strtok_r(rest1, " ", &rest1);
//                if (token) {
//                    if (strcmp(token, "Host:") == 0) {
//                        token = strtok_r(rest1, " ", &rest1); //local host
//                        cout << "LOCAL HOST:" << token << endl;
//                        token = strtok_r(rest1, " ", &rest1);
//                        if (token != NULL) {
//                            //ERROR 
//                            char* response = resParser->error400();
//                            messenger.sendTcpMessage(uri, response);
//
//                            bdcheck = true;
//
//                        } else {
//                            //ok
//
//                            fhost = true;
//                            break;
//                        }
//                    }
//                }
//            }
//            line++;
//        }
//
//        if (!fhost) {
//            //error
//            char* response = resParser->error400();
//            messenger.sendTcpMessage(uri, response);
//
//        }
//        if (bdcheck == false) {
//            char* fullURL = new char [strlen(URL) + strlen(CrawlerDeamon::savedirName) + 1]();
//            // strcat(fullURL, "/");
//            strcat(fullURL, CrawlerDeamon::savedirName);
//            strcat(fullURL, URL);
//
//            if (access(fullURL, F_OK) != -1) { //https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c-cross-platform
//                cout << fullURL << " file exists" << endl;
//                // file exists
//                if (access(fullURL, R_OK) != -1) {
//                    cout << fullURL << "r ok" << endl;
//                    char* response = resParser->send200(fullURL);
//                    messenger.sendTcpMessage(uri, response);                    
//                } else {
//                    cout << "r no" << endl;
//                    char* response = resParser->error403();
//                    messenger.sendTcpMessage(uri, response);
//                }
//
//            } else {
//                // file doesn't exist
//                cout << "file doesn't exist" << endl;
//                char* response = resParser->error404();
//                messenger.sendTcpMessage(uri, response);
//            }
//        }
        //        GET /site0/p0.html HTTP/1.1
        //Host: localhost:11111
        //User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)
        //Accept-Language: en-us
        //Accept-Encoding: gzip, deflate
        //Connection: Keep-Alive


        //  HTTP/1.1 403 Forbidden\r\n
        //  Date: Mon, 27 May 2018 12:28:53 GMT\r\n
        //  Server: myhttpd/1.0.0 (Ubuntu64)\r\n
        //  Content-Length: 124\r\n
        //  Content-Type: text/html\r\n
        //  Connection: Closed\r\n
        //  \r\n
        //  <html>Trying to access this file but don’t think I can make it.</html>
        //        
        //        char * payload = messenger.receiveMessage(e2wfd);
        //
        //        char* str = new char [strlen(payload) + 1];
        //        strcpy(str, payload);
        //
        //        char * pch;
        //        pch = strtok(str, " \n");
        //        if (!ofs.is_open()) {
        //            ofs.open(logDir, std::ofstream::out | std::ofstream::app);
        //        }
        //
        //        ofs << payload << endl;
        //
        //        if (strcmp(pch, "/search") == 0) {
        //            search((char*) payload);
        //        } else if (strcmp(pch, "/maxcount") == 0) {
        //            maxcount((char*) payload);
        //        } else if (strcmp(pch, "/mincount") == 0) {
        //            mincount((char*) payload);
        //        } else if (strcmp(pch, "/wc") == 0) {
        //            wc();
        //        } else if (strcmp(pch, "/exit") == 0) {
        //            sendSuccessfulSearches();
        //            delete [] str;
        //            delete [] payload;
        //
        //            break;
        //        } else {
        //        }
        //
        //        delete [] str;
        //        delete [] payload;
        //
        //        if (payload == NULL) {
        //            exit(0);
        //        }

        free(uri);
    }

}

void Worker::search(char* query) {
    struct timeval tmnow; //  https://stackoverflow.com/questions/3673226/how-to-print-time-in-format-2009-08-10-181754-811
    struct tm *tm;
    char buf[30], usec_buf[6];
    bool finder = false;

    searchres = new SearchRes();
    char * pch;
    pch = strtok(query, " "); //http://www.cplusplus.com/forum/beginner/8388/
    while (pch != NULL) {
        pch = strtok(NULL, " ");
        if (pch != NULL) {
            PostingList* tempPl = trie->GetPostingListHeadPtr(pch);


            gettimeofday(&tmnow, NULL);
            tm = localtime(&tmnow.tv_sec);
            strftime(buf, 30, "%Y:%m:%dT%H:%M:%S", tm);
            strcat(buf, ".");
            sprintf(usec_buf, "%d", (int) tmnow.tv_usec);
            strcat(buf, usec_buf);
            if (!ofs.is_open()) {
                ofs.open(logDir, std::ofstream::out | std::ofstream::app);
            }
            //ofs << ""<<endl;
            ofs << buf; //katagrafi sto log file ton akrivi xrono
            ofs << " : search : ";
            ofs << pch; //tin leksi pros anazitisi
            if (tempPl) {
                tempPl->exportDataToSearchRes(searchres, tempPl); //an i anazitisi efere apotelesma kapoia posting list , gemizoume to antikeimeno searchres me apotelesmata
                finder = true;
                successfulSearches++;
            } else {
                ofs << " : ";
                ofs << "sorry no result found";
                ofs << " " << endl;
            }
        }
    }
    // searchres->print();
    if (finder) {
        char* result = searchres->convertToString(numOffiles); //siriopisi me typou opws ginetai me xml twn apotelesmatwn
        messenger.sendMessage(w2efd, result);
        delete [] result;
    } else {
        char errorMess[10];
        strcpy(errorMess, "no-result");
        messenger.sendMessage(w2efd, errorMess);
    }
    delete searchres;
}

void Worker::maxcount(char* keyword) {
    struct timeval tmnow; //  https://stackoverflow.com/questions/3673226/how-to-print-time-in-format-2009-08-10-181754-811
    struct tm *tm;
    char buf[30], usec_buf[6];

    gettimeofday(&tmnow, NULL);
    tm = localtime(&tmnow.tv_sec);
    strftime(buf, 30, "%Y:%m:%dT%H:%M:%S", tm);
    strcat(buf, ".");
    sprintf(usec_buf, "%d", (int) tmnow.tv_usec);
    strcat(buf, usec_buf);

    //first word is operation
    char * k;
    k = strtok(keyword, " \n");
    k = strtok(NULL, " \n");
    if (!ofs.is_open()) {
        ofs.open(logDir, std::ofstream::out | std::ofstream::app);
    }

    ofs << buf;
    ofs << " : maxcount : ";
    ofs << k;

    PostingList* tempPl = trie->GetPostingListHeadPtr(k);
    if (tempPl) {
        MaxMinCountRes* maxMinCountRes = new MaxMinCountRes(); // antikeimeno pou tha krataei oti afora to maxcounter
        tempPl->returnMaxCount(tempPl, maxMinCountRes); //enimerwsi tou maxMinCountRes 
        // cout << "Top Path" << maxMinCountRes->GetFilePath() << " Counter" << maxMinCountRes->GetCounter() << endl;
        char* convertToString = maxMinCountRes->convertToString();
        messenger.sendMessage(w2efd, convertToString);
        successfulSearches++;
        delete [] convertToString;
        delete maxMinCountRes;
    } else {
        //cout << "i leksi den yparxei" << endl;
        ofs << " : ";
        ofs << "sorry no result found";
        ofs << " " << endl;
        char errorMess[10];
        strcpy(errorMess, "no-result");
        messenger.sendMessage(w2efd, errorMess);
    }
}

void Worker::mincount(char* keyword) {

    struct timeval tmnow; //  https://stackoverflow.com/questions/3673226/how-to-print-time-in-format-2009-08-10-181754-811
    struct tm *tm;
    char buf[30], usec_buf[6];

    gettimeofday(&tmnow, NULL);
    tm = localtime(&tmnow.tv_sec);
    strftime(buf, 30, "%Y:%m:%dT%H:%M:%S", tm);
    strcat(buf, ".");
    sprintf(usec_buf, "%d", (int) tmnow.tv_usec);
    strcat(buf, usec_buf);

    //first word is operation
    char * k;
    k = strtok(keyword, " \n");
    k = strtok(NULL, " \n");

    if (!ofs.is_open()) {
        ofs.open(logDir, std::ofstream::out | std::ofstream::app);
    }

    ofs << buf;
    ofs << " : mincount : ";
    ofs << k;

    PostingList* tempPl = trie->GetPostingListHeadPtr(k);
    if (tempPl) {
        MaxMinCountRes* minCountRes = new MaxMinCountRes();
        tempPl->returnMinCount(tempPl, minCountRes);
        //        cout << "Top Path" << minCountRes->GetFilePath() << " Counter" << minCountRes->GetCounter() << endl;
        messenger.sendMessage(w2efd, minCountRes->convertToString());
        successfulSearches++;
        delete minCountRes;

    } else {
        //cout << "i leksi den yparxei" << endl;
        ofs << " : ";
        ofs << "sorry no result found";
        ofs << " " << endl;
        char errorMess[10];
        strcpy(errorMess, "no-result");
        messenger.sendMessage(w2efd, errorMess);
    }
}

void Worker::sendSuccessfulSearches() {
    char* charCounter;
    charCounter = (char*) malloc(sizeof successfulSearches + 1); //   https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charCounter, "%d", successfulSearches);

    messenger.sendMessage(w2efd, charCounter);

    free(charCounter);
}

void Worker::wc() {

    struct timeval tmnow; //  https://stackoverflow.com/questions/3673226/how-to-print-time-in-format-2009-08-10-181754-811
    struct tm *tm;
    char buf[30], usec_buf[6];

    gettimeofday(&tmnow, NULL);
    tm = localtime(&tmnow.tv_sec);
    strftime(buf, 30, "%Y:%m:%dT%H:%M:%S", tm);
    strcat(buf, ".");
    sprintf(usec_buf, "%d", (int) tmnow.tv_usec);
    strcat(buf, usec_buf);

    ofs << buf;
    ofs << " : wc : ";

    int byteCounter = 0;
    int lineCounter = 0;
    int wordCounter = trie->totalWords;

    FILE *fp;
    for (int i = 0; i < numOffiles; i++) { //upologismos twn byte kai lines gia kathe map , dld kathe arxeio
        //   if (mapArray[i]->readFile()) {
        fp = fopen(mapArray[i]->getFileName(), "rb");
        fseek(fp, 0, SEEK_END); //https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
        byteCounter += ftell(fp);
        lineCounter += mapArray[i]->getLineCounter();
        fclose(fp);
        //  }
    }

    //  cout << "TOTAL BYTES " << totalBytes << endl;
    ofs << byteCounter;
    ofs << " " << endl;
    WC* wc = new WC();

    wc->setTotalBytes(byteCounter);
    wc->setTotalLines(lineCounter);
    wc->setTotalWord(wordCounter);

    char* wccount = wc->convertToString();

    messenger.sendMessage(w2efd, wccount);

    delete [] wccount;
    delete wc;
}

Map** Worker::getMapArray() {
    return mapArray;
}
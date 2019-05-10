#include "ListOfWorkersDir.h"
#include "CrawlerDeamon.h"
#include "SearchRes.h"
#include "Worker.h"
#include "Map.h"
#include "NetworkFunctions.h"
#include "ResultParser.h"
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/poll.h>
#include "WC.h"
#include "MaxMinCountRes.h"
#include <ctime>

#include <sys/socket.h>      /* sockets */
#include <netinet/in.h>      /* internet sockets */
       #include <sys/time.h>

using namespace std;
 
FifoFD CrawlerDeamon::fifoFD;
bool CrawlerDeamon::online = true;
Statistics CrawlerDeamon::statistics;
char* CrawlerDeamon::savedirName;

CrawlerDeamon::CrawlerDeamon(char * savedirName, int t, int servingport, int commandport) : servingport(servingport), commandport(commandport) {
    signal(SIGPIPE, SIG_IGN); // agnoisi tou simatos sigpipe https://stackoverflow.com/questions/108183/how-to-prevent-sigpipes-or-handle-them-properly 
    savedirName=savedirName;
    numOfThreads = t;
    arrayWithListOfThreads = new ListOfThreads*[t]; //pinakas me listes , kathe lista periexei simantikes plirofories gia tous workers   

    for (int i = 0; i < numOfThreads; i++) {
        arrayWithListOfThreads[i] = new ListOfThreads();
    }
    
        struct timeval tp;
    gettimeofday(&tp, NULL);
    startms = tp.tv_sec * 1000 + tp.tv_usec / 1000;

}

CrawlerDeamon::~CrawlerDeamon() {
    void * status = NULL;

    close(commandfd);

    for (int i = 0; i < numOfThreads; i++) {
        pthread_join(arrayWithListOfThreads[i]->threadid, &status);
    }

    cout << "Close for sockets complete \n";

    for (int i = 0; i < numOfThreads; i++) {
        delete arrayWithListOfThreads[i];
    }

    delete [] arrayWithListOfThreads;
}

int CrawlerDeamon::openListeningSocket(int port) {
    int sock;
    struct sockaddr_in server;

    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror_exit("socket");
    }

    server.sin_family = AF_INET; /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port); /* The given port */

    if (bind(sock, (struct sockaddr *) &server, sizeof (server)) < 0) {
        perror_exit("bind");
    }

    if (listen(sock, 5) < 0) {
        perror_exit("listen");
    }

    return sock;
}

void CrawlerDeamon::initialize() {
    // create sockets
    commandfd = openListeningSocket(commandport);

    cout << "Initialization for sockets complete \n";
}

void * thread_main(void * args) {
    
    Worker* worker = new Worker();
    worker->start();
    delete worker;
    return 0;
}

void CrawlerDeamon::createThreads() { //dimiourgia twn worker 
    for (int i = 0; i < numOfThreads; i++) {
        pthread_t p;
        pthread_create(&p, NULL, thread_main, NULL);

        arrayWithListOfThreads[i]->threadid = p;
    }
}

//void HTTPdeamon::search(char* operation, char* command, int d) { //https://stackoverflow.com/questions/3220477/how-to-use-clock-in-c
//piges gia poll
//http://man7.org/linux/man-pages/man2/poll.2.html
//http://www.unixguide.net/unix/programming/2.1.2.shtml
//
//    bool result = false;
//    int completedWorkers = 0;
//    int hasSent = 0;
//    clock_t start;
//    double duration;
//
//    for (int i = 0; i < numOfWorkers; i++) {
//        arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = false;
//    }
//
//    start = clock();
//
//    for (int i = 0; i < numOfWorkers; i++) {
//        messenger.sendMessage(arrayWithListOfWorkers[i]->execToWorkerFD, command);
//        //cout << "search query sent to workers " << endl;
//    }
//
//    while (completedWorkers < numOfWorkers) {
//        //  cout << "Executor enters poll loop " << completedWorkers << " of " << numOfWorkers << " completed " << endl;
//
//        // ipologismos posoi workers den exoun apantisei
//        int missingWorkers = 0;
//        for (int i = 0; i < numOfWorkers; i++) {
//            if (!arrayWithListOfWorkers[i]->hasCompletedCmdCommunication) {
//                missingWorkers++;
//            }
//        }
//
//        // dimiourgia pinaka me tous ekremeis descriptors
//        struct pollfd * array = new struct pollfd[missingWorkers];
//
//        for (int i = 0, j = 0; i < numOfWorkers; i++) {
//            if (!arrayWithListOfWorkers[i]->hasCompletedCmdCommunication) {
//                array[j].events = POLLIN | POLLHUP;
//                array[j].revents = 0;
//                array[j++].fd = arrayWithListOfWorkers[i]->workerToExecutorFD;
//            }
//        }
//
//
//        // i poll perimenei mexri na erthoun dedomena i mexri enas worker kleisei to pipe poll
//     //   cout << "O executor perimenei apo tous workers na steiloun dedomena ..." << endl;
//        int retval = poll(array, missingWorkers, -1);
//
//        if (retval == -1) {
//            perror("poll critical error");
//            exit(0);
//        }
//
//        //  an exei sumvei kati scan to poll array gia POLLI or POLLHUP
//        if (retval > 0) {
//            for (int j = 0; j < missingWorkers; j++) {
//                if ((array[j].revents & POLLIN) != 0) {
//                    duration = (clock() - start) / (double) CLOCKS_PER_SEC;
//                    char* payload = messenger.receiveMessage(array[j].fd);
//
//                    if (duration <= d) { //metraw mono osa apotelesmata irthan mesa sto sugkekrimeno deadline   
//                        if (strcmp(payload, "no-result") != 0) {
//                            ResultParser* resParser = new ResultParser();
//                            resParser->parseSearch(payload);
//                            result = true;
//                            hasSent++;
//                            delete resParser;
//                        } else {
//                            //  cout << command << " :No result" << endl;
//                        }
//                    } else {
//                        //cout<<"deadline error"<<endl;
//                    }
//                    delete [] payload;
//
//                    // auksisi received_payloads gia kathe worker pou eksipiretithike
//                    for (int i = 0; i < numOfWorkers; i++) {
//                        if (arrayWithListOfWorkers[i]->workerToExecutorFD == array[j].fd) {
//                            if (arrayWithListOfWorkers[i]->hasCompletedCmdCommunication == false) {
//                                arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = true;
//                                completedWorkers++;
//                            }
//                            break;
//                        }
//                    }
//                }
//                if ((array[j].revents & POLLHUP) != 0) {
//                    // auksisi received_payloads gia kathe worker pou eksipiretithike
//                    for (int i = 0; i < numOfWorkers; i++) {
//                        if (arrayWithListOfWorkers[i]->workerToExecutorFD == array[j].fd) {
//                            if (arrayWithListOfWorkers[i]->hasCompletedCmdCommunication == false) {
//                                arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = true;
//                                completedWorkers++;
//                            }
//                            break;
//                        }
//                    }
//                }
//            }
//        }
//        delete [] array;
//    }
//
//    cout << hasSent << " worker apantisan apo tous " << numOfWorkers << endl;
//    if (!result) {
//        cout << command << " :No result" << endl;
//    }
//}

//void HTTPdeamon::wc(char* line) {
//    int completedWorkers = 0;
//    WC* totalWc = new WC();
//
//    for (int i = 0; i < numOfWorkers; i++) { //arxikopoiisi boolean gia to an mas exei steilei data
//        arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = false;
//    }
//
//    // ----------------------------------------------------
//    for (int i = 0; i < numOfWorkers; i++) {
//        messenger.sendMessage(arrayWithListOfWorkers[i]->execToWorkerFD, line);
//    }
//    // ----------------------------------------------------
//
//    while (completedWorkers < numOfWorkers) {
//        //    cout << "Executor enters poll loop " << completedWorkers << " of " << numOfWorkers << " completed " << endl;
//        // ipologismos posoi workers den exoun apantisei
//        int missingWorkers = 0;
//
//        for (int i = 0; i < numOfWorkers; i++) {
//            if (!arrayWithListOfWorkers[i]->hasCompletedCmdCommunication) {
//                missingWorkers++;
//            }
//        }
//
//        // dimiourgia pinaka me tous ekremeis descriptors
//        struct pollfd * array = new struct pollfd[missingWorkers];
//
//        for (int i = 0, j = 0; i < numOfWorkers; i++) {
//            if (!arrayWithListOfWorkers[i]->hasCompletedCmdCommunication) {
//                array[j].events = POLLIN | POLLHUP;
//                array[j].revents = 0;
//                array[j++].fd = arrayWithListOfWorkers[i]->workerToExecutorFD;
//            }
//        }
//
//        // i poll perimenei mexri na erthoun dedomena i mexri enas worker kleisei to pipe poll
//      //  cout << "O executor perimenei apo tous workers na steiloun dedomena ..." << endl;
//        int retval = poll(array, missingWorkers, -1);
//
//        if (retval == -1) {
//            perror("poll critical error");
//            exit(0);
//        }
//
//        //  an exei sumvei kati scan to poll array gia POLLI or POLLHUP
//        if (retval > 0) {
//            for (int j = 0; j < missingWorkers; j++) {
//                if ((array[j].revents & POLLIN) != 0) {
//
//                    ResultParser* resParser = new ResultParser();
//                    char* payload = messenger.receiveMessage(array[j].fd);
//
//                    resParser->parseWc(payload, totalWc);
//
//                    delete resParser;
//                    delete [] payload;
//
//                    // auksisi received_payloads gia kathe worker pou eksipiretithike
//                    for (int i = 0; i < numOfWorkers; i++) {
//                        if (arrayWithListOfWorkers[i]->workerToExecutorFD == array[j].fd) {
//                            if (arrayWithListOfWorkers[i]->hasCompletedCmdCommunication == false) {
//                                arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = true;
//                                completedWorkers++;
//                            }
//                            break;
//                        }
//                    }
//                }
//                if ((array[j].revents & POLLHUP) != 0) {
//                    // auksisi received_payloads gia kathe worker pou eksipiretithike
//                    for (int i = 0; i < numOfWorkers; i++) {
//                        if (arrayWithListOfWorkers[i]->workerToExecutorFD == array[j].fd) {
//                            if (arrayWithListOfWorkers[i]->hasCompletedCmdCommunication == false) {
//                                arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = true;
//                                completedWorkers++;
//                            }
//                            break;
//                        }
//                    }
//                }
//            }
//        }
//
//        delete [] array;
//    }
//    totalWc->print();
//    delete totalWc;
//}

//void HTTPdeamon::maxcount(char* keyword) {
//    bool result = false;
//
//    MaxMinCountRes* maxcountRes = new MaxMinCountRes();
//    int completedWorkers = 0;
//    for (int i = 0; i < numOfWorkers; i++) {
//        arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = false;
//    }
//
//    for (int i = 0; i < numOfWorkers; i++) {
//        messenger.sendMessage(arrayWithListOfWorkers[i]->execToWorkerFD, keyword);
//    }
//
//    while (completedWorkers < numOfWorkers) {
//        //  cout << "Executor enters poll loop " << completedWorkers << " of " << numOfWorkers << " completed " << endl;
//        
//        // ipologismos posoi workers den exoun apantisei
//        int missingWorkers = 0;
//
//        for (int i = 0; i < numOfWorkers; i++) {
//            if (!arrayWithListOfWorkers[i]->hasCompletedCmdCommunication) {
//                missingWorkers++;
//            }
//        }
//
//        // dimiourgia pinaka me tous ekremeis descriptors
//        struct pollfd * array = new struct pollfd[missingWorkers];
//
//        for (int i = 0, j = 0; i < numOfWorkers; i++) {
//            if (!arrayWithListOfWorkers[i]->hasCompletedCmdCommunication) {
//                array[j].events = POLLIN | POLLHUP;
//                array[j].revents = 0;
//                array[j++].fd = arrayWithListOfWorkers[i]->workerToExecutorFD;
//            }
//        }
//
//        // i poll perimenei mexri na erthoun dedomena i mexri enas worker kleisei to pipe poll
//       // cout << "O executor perimenei apo tous workers na steiloun dedomena ..." << endl;
//        int retval = poll(array, missingWorkers, -1);
//
//        if (retval == -1) {
//            perror("poll critical error");
//            exit(0);
//        }
//
//        //  an exei sumvei kati scan to poll array gia POLLI or POLLHUP
//        if (retval > 0) {
//            for (int j = 0; j < missingWorkers; j++) {
//                if ((array[j].revents & POLLIN) != 0) {
//
//
//                    char* payload = messenger.receiveMessage(array[j].fd);
//
//                    if (strcmp(payload, "no-result") != 0) {
//                        ResultParser* resParser = new ResultParser();
//                        resParser->parseMaxCounter(payload, maxcountRes);
//                        result = true;
//                        delete resParser;
//                    } else {
//                        //cout << keyword << " :No result" << endl;
//                    }
//
//                    delete [] payload;
//
//                    // auksisi received_payloads gia kathe worker pou eksipiretithike
//                    for (int i = 0; i < numOfWorkers; i++) {
//                        if (arrayWithListOfWorkers[i]->workerToExecutorFD == array[j].fd) {
//                            if (arrayWithListOfWorkers[i]->hasCompletedCmdCommunication == false) {
//                                arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = true;
//                                completedWorkers++;
//                            }
//                            break;
//                        }
//                    }
//                }
//                if ((array[j].revents & POLLHUP) != 0) {
//                    // auksisi received_payloads gia kathe worker pou eksipiretithike
//                    for (int i = 0; i < numOfWorkers; i++) {
//                        if (arrayWithListOfWorkers[i]->workerToExecutorFD == array[j].fd) {
//                            if (arrayWithListOfWorkers[i]->hasCompletedCmdCommunication == false) {
//                                arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = true;
//                                completedWorkers++;
//                            }
//                            break;
//                        }
//                    }
//                }
//            }
//        }
//        
//        delete [] array;
//    }
//    if (result) {
//        maxcountRes->print();
//    } else {
//        cout << keyword << " :No result" << endl;
//
//    }
//    delete maxcountRes;
//}

//void HTTPdeamon::mincount(char* keyword) {
//
//    bool result = false;
//
//    MaxMinCountRes* mincountRes = new MaxMinCountRes();
//
//    int completedWorkers = 0;
//    for (int i = 0; i < numOfWorkers; i++) {
//        arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = false;
//    }
//
//    for (int i = 0; i < numOfWorkers; i++) {
//        messenger.sendMessage(arrayWithListOfWorkers[i]->execToWorkerFD, keyword);
//    }
//
//
//    while (completedWorkers < numOfWorkers) {
//        // cout << "Executor enters poll loop " << completedWorkers << " of " << numOfWorkers << " completed " << endl;
//        // ipologismos posoi workers den exoun apantisei
//        int missingWorkers = 0;
//
//        for (int i = 0; i < numOfWorkers; i++) {
//            if (!arrayWithListOfWorkers[i]->hasCompletedCmdCommunication) {
//                missingWorkers++;
//            }
//        }
//
//        // dimiourgia pinaka me tous ekremeis descriptors
//
//        struct pollfd * array = new struct pollfd[missingWorkers];
//
//        for (int i = 0, j = 0; i < numOfWorkers; i++) {
//            if (!arrayWithListOfWorkers[i]->hasCompletedCmdCommunication) {
//                array[j].events = POLLIN | POLLHUP;
//                array[j].revents = 0;
//                array[j++].fd = arrayWithListOfWorkers[i]->workerToExecutorFD;
//            }
//        }
//
//        // i poll perimenei mexri na erthoun dedomena i mexri enas worker kleisei to pipe poll
//      // cout << "O executor perimenei apo tous workers na steiloun dedomena ..." << endl;
//        int retval = poll(array, missingWorkers, -1);
//
//        if (retval == -1) {
//            perror("poll critical error");
//            exit(0);
//        }
//
//        //  an exei sumvei kati scan to poll array gia POLLI or POLLHUP
//        if (retval > 0) {
//            for (int j = 0; j < missingWorkers; j++) {
//                if ((array[j].revents & POLLIN) != 0) {
//
//
//                    char* payload = messenger.receiveMessage(array[j].fd);
//
//                    if (strcmp(payload, "no-result") != 0) {
//                        ResultParser* resParser = new ResultParser();
//                        resParser->parseMinCounter(payload, mincountRes);
//                        result = true;
//                        delete resParser;
//                    } else {
//                        // cout << keyword << " :No result" << endl;
//                    }
//
//                    delete [] payload;
//
//                    // auksisi received_payloads gia kathe worker pou eksipiretithike
//                    for (int i = 0; i < numOfWorkers; i++) {
//                        if (arrayWithListOfWorkers[i]->workerToExecutorFD == array[j].fd) {
//                            if (arrayWithListOfWorkers[i]->hasCompletedCmdCommunication == false) {
//                                arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = true;
//                                completedWorkers++;
//                            }
//                            break;
//                        }
//                    }
//                }
//                if ((array[j].revents & POLLHUP) != 0) {
//                    // auksisi received_payloads gia kathe worker pou eksipiretithike
//                    for (int i = 0; i < numOfWorkers; i++) {
//                        if (arrayWithListOfWorkers[i]->workerToExecutorFD == array[j].fd) {
//                            if (arrayWithListOfWorkers[i]->hasCompletedCmdCommunication == false) {
//                                arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = true;
//                                completedWorkers++;
//                            }
//                            break;
//                        }
//                    }
//                }
//            }
//        }
//        delete [] array;
//    }
//    if (result) {
//        mincountRes->print();
//    } else {
//        cout << keyword << " :No result" << endl;
//
//    }
//    delete mincountRes;
//}

void CrawlerDeamon::Exit(char* str) {
    //
    //    int completedWorkers = 0;
    //    int totalSuccessfulSearches = 0;
    //    for (int i = 0; i < numOfThreads; i++) {
    //        arrayWithListOfThreads[i]->hasCompletedCmdCommunication = false;
    //    }
    //
    //    // ----------------------------------------------------
    //    for (int i = 0; i < numOfThreads; i++) {
    //        messenger.sendMessage(arrayWithListOfThreads[i]->execToWorkerFD, str);
    //    }
    //    // ----------------------------------------------------
    //
    //    while (completedWorkers < numOfThreads) {
    //        // cout << "Executor enters poll loop " << completedWorkers << " of " << numOfWorkers << " completed " << endl;
    //        // count how many workers have not responded
    //
    //        int missingWorkers = 0;
    //
    //        for (int i = 0; i < numOfThreads; i++) {
    //            if (!arrayWithListOfThreads[i]->hasCompletedCmdCommunication) {
    //                missingWorkers++;
    //            }
    //        }
    //
    //        // dimiourgia pinaka me tous ekremeis descriptors
    //
    //        struct pollfd * array = new struct pollfd[missingWorkers];
    //
    //        for (int i = 0, j = 0; i < numOfThreads; i++) {
    //            if (!arrayWithListOfThreads[i]->hasCompletedCmdCommunication) {
    //                array[j].events = POLLIN | POLLHUP;
    //                array[j].revents = 0;
    //                array[j++].fd = arrayWithListOfThreads[i]->workerToExecutorFD;
    //            }
    //        }
    //
    //        // i poll perimenei mexri na erthoun dedomena i mexri enas worker kleisei to pipe poll
    //        //  cout << "O executor perimenei apo tous workers na steiloun dedomena ..." << endl;
    //        int retval = poll(array, missingWorkers, -1);
    //
    //        if (retval == -1) {
    //            perror("poll critical error");
    //            exit(0);
    //        }
    //
    //        //  an exei sumvei kati scan to poll array gia POLLI or POLLHUP
    //        if (retval > 0) {
    //            for (int j = 0; j < missingWorkers; j++) {
    //                if ((array[j].revents & POLLIN) != 0) {
    //
    //                    char* payload = messenger.receiveMessage(array[j].fd);
    //                    totalSuccessfulSearches += atoi(payload);
    //                    delete [] payload;
    //
    //                    // auksisi received_payloads gia kathe worker pou eksipiretithike
    //                    for (int i = 0; i < numOfThreads; i++) {
    //                        if (arrayWithListOfThreads[i]->workerToExecutorFD == array[j].fd) {
    //                            if (arrayWithListOfThreads[i]->hasCompletedCmdCommunication == false) {
    //                                arrayWithListOfThreads[i]->hasCompletedCmdCommunication = true;
    //                                completedWorkers++;
    //                            }
    //                            break;
    //                        }
    //                    }
    //                }
    //                if ((array[j].revents & POLLHUP) != 0) {
    //                    // auksisi received_payloads gia kathe worker pou eksipiretithike
    //                    for (int i = 0; i < numOfThreads; i++) {
    //                        if (arrayWithListOfThreads[i]->workerToExecutorFD == array[j].fd) {
    //                            if (arrayWithListOfThreads[i]->hasCompletedCmdCommunication == false) {
    //                                arrayWithListOfThreads[i]->hasCompletedCmdCommunication = true;
    //                                completedWorkers++;
    //                            }
    //                            break;
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //
    //
    //        delete [] array;
    //    }
}

//restart tou xamenou worker kai tou dinoume oti eixe na analavei apo prin

void CrawlerDeamon::enqueue(char * uri) {
    fifoFD.place(uri);
}

void CrawlerDeamon::submit(int fd) {
    struct sockaddr_in client;
    socklen_t clientlen = sizeof (client);
    struct sockaddr *clientptr = (struct sockaddr *) &client;
    NetworkFunctions messenger;

    cout << "Crawler: connection from console" << endl;

    int newsock;
    if ((newsock = accept(fd, clientptr, &clientlen)) < 0) {
        perror_exit("accept");
    }

    char* payload = messenger.receiveTcpMessage(newsock);

    cout << "payload is " << payload;

    char *newline = strchr(payload, '\n'); //https://stackoverflow.com/questions/16677800/strtok-not-discarding-the-newline-character 
    if (newline)
        *newline = 0;

    newline = strchr(payload, '\r');
    if (newline)
        *newline = 0;

    char * operation = payload;

    cout << "operation is '" << operation << "'" << endl;

    if (strcmp(operation, "stats") == 0 || strcmp(operation, "STATS") == 0) {
        
                struct timeval tp1;
        gettimeofday(&tp1, NULL);
        endms = tp1.tv_sec * 1000 + tp1.tv_usec / 1000;
        long int time =endms-startms;

        
        char * response = statistics.convertToString(time);

        messenger.sendTcpMessage(newsock, response);
        
        delete [] response;
    }

    if (strcmp(operation, "shutdown") == 0 ||  strcmp(operation, "SHUTDOWN") == 0) {
        for (int i = 0; i < numOfThreads; i++) {
            fifoFD.place(0);
        }
        online = false;
        cout<< "ciao" << endl;
    }
  
    close(newsock);
}

void CrawlerDeamon::restart(pid_t terminated_pid) {
    //    cout << "restarting worker in place of pid: " << terminated_pid << endl;
    //
    //    for (int i = 0; i < numOfWorkers; i++) {
    //        if (arrayWithListOfWorkers[i]->pid == terminated_pid) {
    //
    //            close(arrayWithListOfWorkers[i]->execToWorkerFD);
    //            close(arrayWithListOfWorkers[i]->workerToExecutorFD);
    //
    //            unlink(arrayWithListOfWorkers[i]->execToWorkerName);
    //            unlink(arrayWithListOfWorkers[i]->workerToExecutorName);
    //
    //            mkfifo(arrayWithListOfWorkers[i]->execToWorkerName, 0666);
    //            mkfifo(arrayWithListOfWorkers[i]->workerToExecutorName, 0666);
    //
    //            arrayWithListOfWorkers[i]->pid = fork();
    //
    //            if (arrayWithListOfWorkers[i]->pid == 0) {
    //                cout << "Hello from worker " << getpid() << endl;
    //
    //                int e2wfd = open(arrayWithListOfWorkers[i]->execToWorkerName, O_RDONLY);
    //                int w2efd = open(arrayWithListOfWorkers[i]->workerToExecutorName, O_WRONLY);
    //
    //                Worker* worker = new Worker(e2wfd, w2efd, i);
    //                if (!worker->receivePaths()) {
    //                    delete worker;
    //                    exit(1);
    //                }
    //                if (!worker->importData()) {
    //                    delete worker;
    //                    exit(1);
    //                }
    //                worker->start();
    //                delete worker;
    //                exit(1);
    //            } else {
    //                cout << "Executor is waiting for new to synch " << endl;
    //
    //                arrayWithListOfWorkers[i]->execToWorkerFD = open(arrayWithListOfWorkers[i]->execToWorkerName, O_WRONLY);
    //                arrayWithListOfWorkers[i]->workerToExecutorFD = open(arrayWithListOfWorkers[i]->workerToExecutorName, O_RDONLY);
    //                cout << "Executor synchronized with new workers " << endl;
    //                char * payload = arrayWithListOfWorkers[i]->merge();
    //                messenger.sendMessage(arrayWithListOfWorkers[i]->execToWorkerFD, payload);
    //                delete [] payload;
    //            }
    //        }
    //    }
}

void CrawlerDeamon::perror_exit(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int CrawlerDeamon::getCommandfd() {
    return commandfd;
}

void CrawlerDeamon::setCommandfd(int commandfd) {
    this->commandfd = commandfd;
}



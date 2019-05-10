#include <fstream>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <cstdlib>
#include "Map.h"
#include "Trie.h"
#include "FifoFD.h"
#include <sys/poll.h>
#include "NetworkFunctions.h"

#include "HTTPdeamon.h"
#include <sys/wait.h>

using namespace std;
bool isNumber(char *number);
void startServing(int w, HTTPdeamon* httpDeamon);

int main(int argc, char** argv) {
    int T = 4;
    char *rootdirName;
    int servingport;
    int commandport;

    //elegxos orismatwn
    if (argc < 9) {
        cout << "Too few arguments .. " << endl;
        return 1;
    } else if (argc > 9) {
        cout << "Too many arguments supplied ... " << endl;
        return 1;
    } else if (argc == 9) {  
        if (strcmp(argv[1], "-p") == 0 && strcmp(argv[3], "-c") == 0 && strcmp(argv[5], "-t") == 0 && strcmp(argv[7], "-d") == 0 && isNumber(argv[6]) && isNumber(argv[2]) && isNumber(argv[4])) { // ektos twn alwn elexete kai an w einai arithmos 
            servingport = atoi(argv[2]);
            commandport = atoi(argv[4]);
            T = atoi(argv[6]);
            rootdirName = argv[8];

            
            
            if (T == 0) {
                cout << "wrong value for workers..." << endl;
            }

        } else {
            cout << "Invalid parameters .. " << endl;
            return 1;
        }
    } else {
        cout << "Invalid parameters .. " << endl;
        return 1;
    }

    HTTPdeamon* httpDeamon = new HTTPdeamon(rootdirName, T, servingport, commandport);
    httpDeamon->initialize();

    httpDeamon->createThreads();

    startServing(T, httpDeamon);

    delete httpDeamon;

    return 0;
}

void startServing(int w, HTTPdeamon* httpDeamon) {
    char* operation = NULL;

    cout << "----------Server activated ----------" << endl;

    char *line = NULL;
    
    do {
        struct pollfd * array = new struct pollfd[2];

        for (int i = 0; i < 2; i++) {
            array[i].events = POLLIN | POLLHUP;
            array[i].revents = 0;
            if (i == 0) {
                array[i].fd = httpDeamon->getServingfd();
            } else if (i == 1) {
                array[i].fd = httpDeamon->getCommandfd();
            }
        }

        int retval = poll(array, 2, -1);

        if (retval == -1) {
            perror("poll critical error");
            exit(0);
        }

        //  an exei sumvei kati scan to poll array gia POLLI or POLLHUP
        if (retval > 0) {
            for (int j = 0; j < 2; j++) {
                if ((array[j].revents & POLLIN) != 0) {
                    switch(j) {
                        case 0:
                            httpDeamon->enqueue(array[j].fd);
                            break;
                        case 1:
                            httpDeamon->submit(array[j].fd);
                    }
                    //ResultParser* resParser = new ResultParser();
                }
                if ((array[j].revents & POLLHUP) != 0) {
                    //
                }
            }
        }

        delete [] array;
    } while (HTTPdeamon::online == true);

    //    cout << "/search" << endl;
    //    cout << "/maxcount" << endl;
    //    cout << "/mincount" << endl;
    //    cout << "/wc" << endl;
    //    cout << "/exit" << endl;
    //
    //    do {
    //
    //        printf("# ");
    //        size_t len = 0;
    //        //        read = getline(&line, &len, stdin);
    //
    //        // ----------- restart workers an xreiastei --------------- /
    //        //https://www.systutorials.com/docs/linux/man/2-waitpid/
    //        //https://stackoverflow.com/questions/47024848/how-does-waitpidpid-t-1-null-wnohang-keep-track-of-child-processes-to-be
    //        //https://stackoverflow.com/questions/28840215/when-and-why-should-you-use-wnohang-with-waitpid
    //
    //        
    //        int status;
    //        pid_t terminated_pid;
    //
    //        cout << "check gia termatismenous workers .... " << endl;
    //        while ((terminated_pid = waitpid(-1, &status, WNOHANG)) > 0) {
    //            cout << "termatismeno pid vrethike: " << terminated_pid << endl;
    //            jobExecutor->restart(terminated_pid);
    //        }
    //
    //        //cout << line << endl;
    //        char *newline = strchr(line, '\n'); //https://stackoverflow.com/questions/16677800/strtok-not-discarding-the-newline-character 
    //        if (newline)
    //            *newline = 0;
    //
    //        char* str = new char [strlen(line) + 1];
    //        strcpy(str, line);
    //
    //        char * pch;
    //        pch = strtok(str, " \n");
    //        if (operation != NULL) {
    //            delete [] operation;
    //        }
    //        operation = new char [strlen(pch) + 1];
    //        strcpy(operation, pch); //i prwti leksi einai to operation pou zitaei o xristis, opote tin apothikeyw sti metavliti operation
    //
    //        while (pch != NULL) {
    //            pch = strtok(NULL, " \n");
    //            if (pch != NULL) { //metrisi twn leksewn
    //                numOfWords++;
    //            }
    //        }
    //
    //        cout << "Operation:" << operation << endl;
    //        if (strcmp(operation, "/search") == 0) {
    //            if (numOfWords < 3) {
    //                cout << "apaitite toulaxiston mia leksi gia na ektelestei to search" << endl;
    //            } else {
    //                char* query = new char [strlen(line) + 1];
    //                char * pch1;
    //                pch1 = strtok(line, " \n");
    //                strcpy(query, pch1);
    //
    //                for (int i = 1; pch1 != NULL; i++) {
    //                    pch1 = strtok(NULL, " \n");
    //                    if (i == numOfWords) {
    //                        if (isNumber(pch1) == false) {
    //                            cout << "pliktrologiste swsta to deadline" << endl;
    //                            break;
    //                        } else {
    //                            int deadline = atoi(pch1);
    //                            if (deadline == 0) {
    //                                deadline = 3;
    //                            }
    //                            //                            jobExecutor->search(operation, query, deadline);
    //                            break;
    //                        }
    //                    } else if (i == numOfWords - 1) {
    //                        if (strcmp(pch1, "-d") != 0) {
    //                            cout << "pliktrologiste swsta to flag tou deadline" << endl;
    //                            break;
    //                        }
    //                    } else {
    //                        sprintf(query, "%s %s", query, pch1);
    //                        // strcat(query," ");
    //                        // strcat(query,pch1);
    //                        // asprintf(&query,"%s%s",query," " );
    //                        // asprintf(&query,"%s%s",query,pch1);
    //                    }
    //                }
    //                delete[] query;
    //            }
    //        } else if (strcmp(operation, "/maxcount") == 0) {
    //            if (numOfWords == 1) {
    //                //                jobExecutor->maxcount(line);
    //            } else {
    //                cout << "I entoli maxcount pernei mia leksi" << endl;
    //            }
    //        } else if (strcmp(operation, "/mincount") == 0) {
    //            if (numOfWords == 1) {
    //                //                jobExecutor->mincount(line);
    //            } else {
    //                cout << "I entoli mincount pernei mia leksi" << endl;
    //            }
    //        } else if (strcmp(operation, "/wc") == 0) {
    //            if (numOfWords == 0) {
    //                //                jobExecutor->stats(line);
    //            } else {
    //                cout << "I entoli mincount pernei mia leksi" << endl;
    //            }
    //        } else if (strcmp(operation, "/exit") == 0) {
    //            jobExecutor->Exit(line);
    //            cout << "Exit from program" << endl;
    //        } else {
    //            cout << "Wrong operation" << endl;
    //        }
    //        if (line != NULL) {
    //            free(line);
    //        }
    //        delete [] str;
    //    } while (strcmp(operation, "/exit") != 0);
    //    delete [] operation;
}

bool isNumber(char *number) //https://stackoverflow.com/questions/29248585/c-checking-command-line-argument-is-integer-or-not
{
    int i = 0;
    //checking for negative numbers
    if (number[0] == '-') {
        i = 1;
        cout << "please not negative numbers" << endl;
        return false;
    }
    for (; number[i] != 0; i++) {
        if (!isdigit(number[i])) {
            cout << "please enter right the number after flag" << endl;
            return false;
        }
    }
    return true;
}
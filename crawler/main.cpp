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

#include "CrawlerDeamon.h"
#include <sys/wait.h>

using namespace std;
bool isNumber(char *number);
void startServing(int w, CrawlerDeamon* httpDeamon);

int main(int argc, char** argv) {
    int T = 4;
    char *startingURL;
    char *hostname;
    char *savedirName;
    int servingport;
    int commandport;

    //elegxos orismatwn
    if (argc < 12) {
        cout << "Too few arguments .. " << endl;
        return 1;
    } else if (argc > 12) {
        cout << "Too many arguments supplied ... " << endl;
        return 1;
    } else if (argc == 12) { // dinetai i dunatotita ta flags na exoun diaforetiki seira  , diladi mporei na valei to -d san prwto flag i san deytero, to idio kai me to -w  
        if (strcmp(argv[1], "-h") == 0 && strcmp(argv[3], "-p") == 0 && strcmp(argv[5], "-c") == 0 && strcmp(argv[7], "-t") == 0 && strcmp(argv[9], "-d") == 0 && isNumber(argv[8]) && isNumber(argv[4]) && isNumber(argv[6])) { // ektos twn alwn elexete kai an w einai arithmos 
            hostname = argv[2];
            servingport = atoi(argv[4]);
            commandport = atoi(argv[6]);
            T = atoi(argv[8]);
            savedirName = argv[10];
            startingURL = argv[11];

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

    CrawlerDeamon* crawlerDeamon = new CrawlerDeamon(savedirName, T, servingport, commandport);
    crawlerDeamon->initialize();

    crawlerDeamon->createThreads();

    char * uri = new char[5000];
    
    strcpy(uri, "http://");
    strcat(uri, hostname);
    strcat(uri, ":");
    strcat(uri, argv[4]);
    strcat(uri, "/");
    strcat(uri, startingURL);
    
    crawlerDeamon->enqueue(uri);
    
    //    crawlerDeamon->enqueue(startingURL);

    startServing(T, crawlerDeamon);

    delete crawlerDeamon;

    return 0;
}

void startServing(int w, CrawlerDeamon* crawlerDeamon) {
    char* operation = NULL;

    cout << "----------Server activated ----------" << endl;

    char *line = NULL;

    do {
        crawlerDeamon->submit(crawlerDeamon->getCommandfd());
    } while (CrawlerDeamon::online == true);

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
#include "ResultParser.h"
#include "Worker.h"
#include "SearchRes.h"
#include "NetworkFunctions.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "WC.h"
#include "Statistics.h"
#include "CrawlerDeamon.h"


using namespace std;

ResultParser::ResultParser() {

}

ResultParser::~ResultParser() {
}

char* ResultParser::searchToString(char* path, int linenum, int numfiles) {

    char* charLineNum;
    charLineNum = (char*) malloc(sizeof linenum + 1); //   https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charLineNum, "%d", linenum);

    int mapPosition = 0; //vriskw se poio map anikei to pathfile, me teliko skopo na vrw to periexomeno tis grammis
    for (mapPosition = 0; mapPosition < numfiles; mapPosition++) {
        if (strcmp(mapArray[mapPosition]->getFileName(), path) == 0) {
            break;
        }
    }
    char* charContentLength;
    charContentLength = (char*) malloc(sizeof strlen(mapArray[mapPosition]->getDocArray()[linenum]) + 1); //   https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charContentLength, "%d", (int) strlen(mapArray[mapPosition]->getDocArray()[linenum]));

    char* result = new char [strlen(" <result>  <path>  </path>  <line>  </line>  </result>  <content>  </content> ") + strlen(path) + strlen(charLineNum) + strlen(charContentLength) + strlen(mapArray[mapPosition]->getDocArray()[linenum]) + 1]();

    strcat(result, "<result> ");
    strcat(result, "<path> ");
    strcat(result, path);
    strcat(result, " </path> ");
    strcat(result, "<line> ");
    strcat(result, charLineNum);
    strcat(result, " </line> ");
    strcat(result, "<content> ");
    strcat(result, charContentLength);
    strcat(result, " ");
    strcat(result, mapArray[mapPosition]->getDocArray()[linenum]);
    strcat(result, " </content> ");
    strcat(result, "</result> ");

    free(charLineNum);
    free(charContentLength);
    return result;
}

void ResultParser::parseSearch(char* payload) {
    char* linenum;
    char* path;
    char* content;
    int numOfRes;
    char* str = new char [strlen(payload) + 1];
    strcpy(str, payload);

    char * pch;
    pch = strtok(str, " \n");
    numOfRes = atoi(pch);

    for (int i = 0; i < numOfRes; i++) {
        pch = strtok(NULL, " \n");
        if (strcmp(pch, "<result>") == 0) {
        } else {
            //lathos eksodo
            cout << "lathos parsing " << "<result>" << endl;
        }

        pch = strtok(NULL, " \n");
        if (pch != NULL) {
            if (strcmp(pch, "<path>") == 0) {
            } else {
                //lathos eksodo
                cout << "lathos parsing " << "<path>" << endl;
            }
        }

        pch = strtok(NULL, " \n");
        if (pch != NULL) {
            path = new char [strlen(pch) + 1];
            strcpy(path, pch);
            cout << "PATH::: " << path << endl;
        }

        pch = strtok(NULL, " \n");
        if (pch != NULL) {
            if (strcmp(pch, "</path>") == 0) {
            } else {
                //lathos eksodo
                cout << "lathos parsing " << "</path>" << endl;
            }
        }

        pch = strtok(NULL, " \n");
        if (pch != NULL) {
            if (strcmp(pch, "<line>") == 0) {
            } else {
                //lathos eksodo
                cout << "lathos parsing " << "<line>" << endl;
            }
        }

        pch = strtok(NULL, " \n");
        if (pch != NULL) {
            linenum = new char [strlen(pch) + 1];
            strcpy(linenum, pch);
            cout << "LINENUM::: " << linenum << endl;
        }

        pch = strtok(NULL, " \n");
        if (pch != NULL) {
            if (strcmp(pch, "</line>") == 0) {
            } else {
                //lathos eksodo
                cout << "lathos parsing " << "</line>" << endl;
            }
        }

        pch = strtok(NULL, " \n");
        if (pch != NULL) {
            if (strcmp(pch, "<content>") == 0) {
            } else {
                //lathos eksodo
                cout << "lathos parsing " << "<content>" << endl;
            }
        }

        pch = strtok(NULL, " \n");
        if (pch != NULL) {
            content = new char [atoi(pch) + 1]();
            // cout << "CONTENT SIZE::: " << pch << endl;
        }

        pch = strtok(NULL, " \n");
        strcat(content, pch);
        while (pch != NULL && strcmp(pch, "</content>") != 0) {
            pch = strtok(NULL, " \n");
            if (pch != NULL && strcmp(pch, "</content>") != 0) {
                strcat(content, " ");
                strcat(content, pch);
            }
        }

        cout << "CONTENT::: " << content << endl;

        pch = strtok(NULL, " \n");
        if (strcmp(pch, "</result>") == 0) {
        } else {
            //lathos eksodo
            cout << "lathos parsing " << "</result>" << endl;
        }

        delete [] linenum;
        delete [] path;
        delete [] content;
    }
    delete [] str;

}

char* ResultParser::maxminToString(char* path, int counter) {
    char* charCounter;
    charCounter = (char*) malloc(sizeof counter + 1); //   https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charCounter, "%d", counter);

    char* result = new char [strlen(" <result>  <path>  </path>  <counter>  </counter>  </result> ") + strlen(path) + strlen(charCounter) + 1]();

    strcat(result, "<result> ");
    strcat(result, "<counter> ");
    strcat(result, charCounter);
    strcat(result, " </counter> ");
    strcat(result, "<path> ");
    strcat(result, path);
    strcat(result, " </path> ");
    strcat(result, "</result> ");

    free(charCounter);
    return result;
}

void ResultParser::parseMaxCounter(char * payload, MaxMinCountRes* maxcountRes) {


    char* counter = 0;
    char* path;

    char* str = new char [strlen(payload) + 1];
    strcpy(str, payload);

    char * pch;

    pch = strtok(str, " \n");
    if (strcmp(pch, "<result>") == 0) {
    } else {
        //lathos eksodo
        cout << "lathos parsing " << "<result>" << endl;
    }


    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        if (strcmp(pch, "<counter>") == 0) {
        } else {
            //lathos eksodo
            cout << "lathos parsing " << "<counter>" << endl;
        }
    }

    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        counter = new char [strlen(pch) + 1];
        strcpy(counter, pch);
        //  cout << "Counter::: " << counter << endl;
    }


    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        if (strcmp(pch, "</counter>") == 0) {
        } else {
            //lathos eksodo
            cout << "lathos parsing " << "</counter>" << endl;
        }
    }

    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        if (strcmp(pch, "<path>") == 0) {
        } else {
            //lathos eksodo
            cout << "lathos parsing " << "<path>" << endl;
        }
    }

    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        path = new char [strlen(pch) + 1];
        strcpy(path, pch);
        //   cout << "PATH::: " << path << endl;
    }

    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        if (strcmp(pch, "</path>") == 0) {
        } else {
            //lathos eksodo
            cout << "lathos parsing " << "</path>" << endl;
        }
    }

    pch = strtok(NULL, " \n");
    if (strcmp(pch, "</result>") == 0) {
    } else {
        //lathos eksodo
        cout << "lathos parsing " << "</result>" << endl;
    }

    if (maxcountRes->GetCounter() < atoi(counter)) {
        maxcountRes->SetCounter(atoi(counter));
        maxcountRes->SetFilePath(path);
    } else if (maxcountRes->GetCounter() == atoi(counter)) {
        if (strcmp(path, maxcountRes->GetFilePath()) < 0) { //to string 1 prohgeitai
            maxcountRes->SetCounter(atoi(counter));
            maxcountRes->SetFilePath(path);
        }
    }

    delete [] counter;
    delete [] str;
    delete [] path;
}

void ResultParser::parseMinCounter(char * payload, MaxMinCountRes* maxcountRes) {

    char* counter;
    char* path;

    char* str = new char [strlen(payload) + 1];
    strcpy(str, payload);

    char * pch;

    pch = strtok(str, " \n");
    if (strcmp(pch, "<result>") == 0) {
    } else {
        //lathos eksodo
        cout << "lathos parsing " << "<result>" << endl;
    }


    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        if (strcmp(pch, "<counter>") == 0) {
        } else {
            //lathos eksodo
            cout << "lathos parsing " << "<counter>" << endl;
        }
    }

    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        counter = new char [strlen(pch) + 1];
        strcpy(counter, pch);
        //cout << "Counter::: " << counter << endl;
    }


    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        if (strcmp(pch, "</counter>") == 0) {
        } else {
            //lathos eksodo
            cout << "lathos parsing " << "</counter>" << endl;
        }
    }

    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        if (strcmp(pch, "<path>") == 0) {
        } else {
            //lathos eksodo
            cout << "lathos parsing " << "<path>" << endl;
        }
    }

    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        path = new char [strlen(pch) + 1];
        strcpy(path, pch);
        // cout << "PATH::: " << path << endl;
    }

    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        if (strcmp(pch, "</path>") == 0) {
        } else {
            //lathos eksodo
            cout << "lathos parsing " << "</path>" << endl;
        }
    }

    pch = strtok(NULL, " \n");
    if (strcmp(pch, "</result>") == 0) {
    } else {
        //lathos eksodo
        cout << "lathos parsing " << "</result>" << endl;
    }

    if (maxcountRes->GetCounter() == 0) {
        maxcountRes->SetCounter(atoi(counter));
        maxcountRes->SetFilePath(path);
    } else if (maxcountRes->GetCounter() > atoi(counter)) {
        maxcountRes->SetCounter(atoi(counter));
        maxcountRes->SetFilePath(path);
    } else if (maxcountRes->GetCounter() == atoi(counter)) {
        if (strcmp(path, maxcountRes->GetFilePath()) < 0) { //to string 1 prohgeitai
            maxcountRes->SetCounter(atoi(counter));
            maxcountRes->SetFilePath(path);
        }
    }

    delete [] counter;
    delete [] str;
    delete [] path;
}

char* ResultParser::wcToString(int totalbytes, int totallines, int totalwords) {

    char* charbytes;
    charbytes = (char*) malloc(sizeof totalbytes + 1); //   https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charbytes, "%d", totalbytes);

    char* charlines;
    charlines = (char*) malloc(sizeof totallines + 1); //   https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charlines, "%d", totallines);

    char* charwords;
    charwords = (char*) malloc(sizeof totalwords + 1); //   https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charwords, "%d", totalwords);


    char* result = new char [strlen(" <result>  <nbytes>  </nbytes>  <nlines>  </nlines>  <nwords>  </nwords>  </result> ") + strlen(charbytes) + strlen(charlines) + strlen(charwords) + 1]();

    strcat(result, "<result> ");

    strcat(result, "<nbytes> ");
    strcat(result, charbytes);
    strcat(result, " </nbytes> ");

    strcat(result, "<nlines> ");
    strcat(result, charlines);
    strcat(result, " </nlines> ");

    strcat(result, "<nwords> ");
    strcat(result, charwords);
    strcat(result, " </nwords> ");

    strcat(result, "</result> ");

    free(charbytes);
    free(charlines);
    free(charwords);
    return result;
}

void ResultParser::parseWc(char* payload, WC * wc) {
    char* counter;
    char* str = new char [strlen(payload) + 1];
    strcpy(str, payload);

    char * pch;

    pch = strtok(str, " \n");
    if (strcmp(pch, "<result>") == 0) {
    } else {
        //lathos eksodo
        cout << "lathos parsing " << "<result>" << endl;
    }

    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        if (strcmp(pch, "<nbytes>") == 0) {
        } else {
            //lathos eksodo
            cout << "lathos parsing " << "<bytes>" << endl;
        }
    }

    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        counter = new char [strlen(pch) + 1];
        strcpy(counter, pch);
        // cout << "BYTE Counter::: " << counter << endl;
        //    wc->setTotalBytes(atoi(counter));
        wc->addByteCounter(atoi(counter));
    }

    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        if (strcmp(pch, "</nbytes>") == 0) {
        } else {
            //lathos eksodo
            cout << "lathos parsing " << "</bytes>" << endl;
        }
    }

    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        if (strcmp(pch, "<nlines>") == 0) {
        } else {
            //lathos eksodo
            cout << "lathos parsing " << "<lines>" << endl;
        }
    }

    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        counter = new char [strlen(pch) + 1];
        strcpy(counter, pch);
        // cout << "LINE Counter::: " << counter << endl;
        // wc->setTotalLines(atoi(counter));
        wc->addLineCounter(atoi(counter));
    }

    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        if (strcmp(pch, "</nlines>") == 0) {
        } else {
            //lathos eksodo
            cout << "lathos parsing " << "</counter>" << endl;
        }
    }


    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        if (strcmp(pch, "<nwords>") == 0) {
        } else {
            //lathos eksodo
            cout << "lathos parsing " << "<counter>" << endl;
        }
    }

    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        counter = new char [strlen(pch) + 1];
        strcpy(counter, pch);
        // cout << "WORD Counter::: " << counter << endl;
        //wc->setTotalWord(atoi(counter));
        wc->addWordCounter(atoi(counter));
    }

    pch = strtok(NULL, " \n");
    if (pch != NULL) {
        if (strcmp(pch, "</nwords>") == 0) {
        } else {
            //lathos eksodo
            cout << "lathos parsing " << "</counter>" << endl;
        }
    }
    pch = strtok(NULL, " \n");
    if (strcmp(pch, "</result>") == 0) {
    } else {
        //lathos eksodo
        cout << "lathos parsing " << "</result>" << endl;
    }

    delete [] counter;
    delete [] str;
}

char* ResultParser::error400() {
    //HTTP / 1.1 400 BAD REQUEST
    //Date : Mon, 27 May 2018 12 : 28 : 53 GMT
    //Server : myhttpd / 1.0.0 (Ubuntu64)
    //Content - Length : 124
    //Content - Type : text / html
    //Connection : Closed
    //[blank line]
    //<html>error 400, BAD REQUEST</ html>

    char outstr[200];
    time_t t;
    struct tm *tmp;
    const char* fmt = "%a, %d %b %y %T %Z";     //https://stackoverflow.com/questions/20619236/how-to-get-utc-time

    t = time(NULL);
    tmp = gmtime(&t);
    if (tmp == NULL) {
        perror("gmtime error");
        exit(EXIT_FAILURE);
    }

    if (strftime(outstr, sizeof (outstr), fmt, tmp) == 0) {
        fprintf(stderr, "strftime returned 0");
        exit(EXIT_FAILURE);
    }
    //printf("%s\n", outstr);

    char* result = new char[4096 * 2]();

    strcat(result, "HTTP/1.1 400 BAD REQUEST\r\nDate: ");
    strcat(result, outstr);
    strcat(result, "\r\n");
    strcat(result, "Server: myhttpd/1.0.0 (Ubuntu64)\r\nContent-Length : ");

    int length = strlen("<html>error 400, BAD REQUEST</html>") + 1;
    char* charlength;
    charlength = (char*) malloc(sizeof length + 1); //   https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charlength, "%d", length);

    strcat(result, charlength);
    strcat(result, "\r\n");
    strcat(result, "Content-Type: text/html\r\nConnection: Closed\r\n\r\n<html>error 400, BAD REQUEST</html>\r\n\r\n");
    return result;
}

char* ResultParser::error404() {

    //HTTP/1.1 404 Not Found
    //Date: Mon, 27 May 2018 12:28:53 GMT
    //Server: myhttpd/1.0.0 (Ubuntu64)
    //Content-Length: 124
    //Content-Type: text/html
    //Connection: Closed
    //[blank line]
    //<html>Sorry dude, couldn’t find this file.</html>

    char outstr[200];
    time_t t;
    struct tm *tmp;
    const char* fmt = "%a, %d %b %y %T %Z";

    t = time(NULL);
    tmp = gmtime(&t);
    if (tmp == NULL) {
        perror("gmtime error");
        exit(EXIT_FAILURE);
    }

    if (strftime(outstr, sizeof (outstr), fmt, tmp) == 0) {
        fprintf(stderr, "strftime returned 0");
        exit(EXIT_FAILURE);
    }
    //printf("%s\n", outstr);

    char* result = new char[4096 * 2]();

    strcat(result, "HTTP/1.1 404 Not Found\r\nDate: ");
    strcat(result, outstr);
    strcat(result, "\r\n");
    strcat(result, "Server: myhttpd/1.0.0 (Ubuntu64)\r\nContent-Length : ");

    int length = strlen("<html>Sorry dude, couldn’t find this file.</html>") + 1;
    char* charlength;
    charlength = (char*) malloc(sizeof length + 1); //   https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charlength, "%d", length);

    strcat(result, charlength);
    strcat(result, "\r\n");
    strcat(result, "Content-Type: text/html\r\nConnection: Closed\r\n\r\n<html>Sorry dude, couldn’t find this file.</html>\r\n\r\n");

    return result;

}

char* ResultParser::error403() {

    //HTTP/1.1 403 Forbidden
    //Date: Mon, 27 May 2018 12:28:53 GMT
    //Server: myhttpd/1.0.0 (Ubuntu64)
    //Content-Length: 124
    //Content-Type: text/html
    //Connection: Closed
    //[blank line]
    //<html>Trying to access this file but don’t think I can make it.</html> 

    char outstr[200];
    time_t t;
    struct tm *tmp;
    const char* fmt = "%a, %d %b %y %T %Z";

    t = time(NULL);
    tmp = gmtime(&t);
    if (tmp == NULL) {
        perror("gmtime error");
        exit(EXIT_FAILURE);
    }

    if (strftime(outstr, sizeof (outstr), fmt, tmp) == 0) {
        fprintf(stderr, "strftime returned 0");
        exit(EXIT_FAILURE);
    }
    //printf("%s\n", outstr);

    char* result = new char[4096 * 2]();

    strcat(result, "HTTP/1.1 403 Forbidden\r\nDate: ");
    strcat(result, outstr);
    strcat(result, "\r\nServer: myhttpd/1.0.0 (Ubuntu64)\r\nContent-Length : ");

    int length = strlen("<html>Trying to access this file but don’t think I can make it.</html>") + 1;
    char* charlength;
    charlength = (char*) malloc(sizeof length + 1); //   https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charlength, "%d", length);

    strcat(result, charlength);
    strcat(result, "\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n<html>Trying to access this file but don’t think I can make it.</html>\r\n\r\n");
    return result;

}

char* ResultParser::send200(char* url) {

    //HTTP/1.1 200 OK
    //Date: Mon, 27 May 2018 12:28:53 GMT
    //Server: myhttpd/1.0.0 (Ubuntu64)
    //Content-Length: 8873
    //Content-Type: text/html
    //Connection: Closed
    //[blank line]
    //[content of the requested file here... e.g. <html>hello one two ...</html>]


    //    char * buffer;
    //    long size;
    //    FILE * f = fopen(url, "rb");
    //
    //    if (f) {
    //        fseek(f, 0, SEEK_END);
    //        size = ftell(f);
    //        fseek(f, 0, SEEK_SET);
    //        buffer = new char[size];
    //        fread(buffer, 1, size, f);
    //        fclose(f);
    //    }

    //https://stackoverflow.com/questions/174531/easiest-way-to-get-files-contents-in-c
    FILE * fp = fopen(url, "rb");
    char* buffer = NULL;
    size_t len;
    ssize_t bytes_read = getdelim(&buffer, &len, '\0', fp);
    //if ( bytes_read != -1)
    CrawlerDeamon::statistics.setBytes(ftell(fp));
    CrawlerDeamon::statistics.setPages(1);
    
    char outstr[200];
    time_t t;
    struct tm *tmp;
    const char* fmt = "%a, %d %b %y %T %Z";

    t = time(NULL);
    tmp = gmtime(&t);
    if (tmp == NULL) {
        perror("gmtime error");
        exit(EXIT_FAILURE);
    }

    if (strftime(outstr, sizeof (outstr), fmt, tmp) == 0) {
        fprintf(stderr, "strftime returned 0");
        exit(EXIT_FAILURE);
    }
    //printf("%s\n", outstr);

    char* result = new char[4096 + strlen(buffer)]();

    strcat(result, "HTTP/1.1 200 OK\r\nDate: ");
    strcat(result, outstr);
    strcat(result, "\r\nServer: myhttpd/1.0.0 (Ubuntu64)\r\nContent-Length : ");

    long int length = strlen(buffer) + 1;
    char* charlength;
    charlength = (char*) malloc(sizeof length + 1); //   https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charlength, "%ld", length);

    strcat(result, charlength);
    strcat(result, "\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n");
    strcat(result, buffer);
    strcat(result, "\r\n\r\n");
    return result;
}


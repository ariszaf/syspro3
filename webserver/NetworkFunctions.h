
#ifndef NETWORKFUNCTIONS_H
#define NETWORKFUNCTIONS_H

class NetworkFunctions {
public:
    void sendMessage(int fd, char * msg);
    char * receiveMessage(int fd);

    void sendTcpMessage(int fd, char * msg);
    char * receiveTcpMessage(int fd);
    
    char * readHTTPheader(int fd);
};

#endif /* NETWORKFUNCTIONS_H */


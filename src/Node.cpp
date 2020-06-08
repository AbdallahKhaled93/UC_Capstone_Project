#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>

#include "Node.h"

Node::Node()
{
    _socketFD = socket(AF_INET, SOCK_STREAM, 0);
    /* throw exception if socket creation failed */
    if(-1 == _socketFD)
    {
        throw std::runtime_error("Could not create socket");
    }
}

Node::~Node()
{
    close(_socketFD);
}

int Node::getSockedFD()
{
    return _socketFD;
}

void Node::setSockedFD(int socket_fd)
{
    _socketFD = socket_fd;
}
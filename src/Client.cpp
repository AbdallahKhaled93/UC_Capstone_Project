#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>

#include "Client.h"



Client::Client()
{
}

void Client::connectToServer(std::string ip_address, uint16_t port)
{
    sockaddr_in serverSocketAddress;
    serverSocketAddress.sin_family = AF_INET;
    serverSocketAddress.sin_port = htons(port);
    if(inet_pton(AF_INET, ip_address.c_str(), &(serverSocketAddress.sin_addr)) <= 0)
    {
        throw std::runtime_error("Invalid server address");
    }

    if (connect(_socketFD, (struct sockaddr *)&serverSocketAddress, sizeof(serverSocketAddress)) < 0)
    {
        throw std::runtime_error("error during connection");
    }

    std::cout << "Connection with socket " << _otherSocketFD << " established" << std::endl;
    receptionThread = std::thread(&Client::Polling, this);
}


void Client::sendMessage(std::string &s)
{
    /* check if there is a connection */
    send(_socketFD, s.c_str(), s.size(), 0);

}

char* Client::receiveMessage()
{
    int byteCount = read(_socketFD, _sendBuffer, 1024);
    if(byteCount)
    {
        _sendBuffer[byteCount] = '\0';
        return _sendBuffer;
    }
    else
    {
        _sendBuffer[0] = '\0';
        return _sendBuffer;
    }
}

void Client::Polling()
{
    while(true)
    {
        /* liberate cpu */
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        _sendString = std::string(receiveMessage());
        if(_sendString != "")
        {
            std::cout << _sendString << std::endl;
            _receivedMsgs->send(std::move(_sendString));
        }

    }
}

void Client::terminateConnection()
{
}


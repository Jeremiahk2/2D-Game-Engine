#include <zmq.hpp>
#include <string>
#include <iostream>
#include <list>
#include "common.h"
#define MESSAGE_LIMIT 1024

#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>

#define sleep(n)	Sleep(n)
#endif


int main()
{
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t reqSocket(context, zmq::socket_type::req);
    zmq::socket_t subSocket(context, zmq::socket_type::sub);
    reqSocket.connect("tcp://localhost:5555");

    subSocket.set(zmq::sockopt::conflate, "");
    subSocket.connect("tcp://localhost:5556");
    subSocket.set(zmq::sockopt::subscribe, "");

    zmq::message_t request;
    char buffer[] = "Connecting";
    memcpy(request.data(), buffer, 11);
    reqSocket.send(request, zmq::send_flags::none);
    sleep(10000);
    while (true) {
        //  Get the reply.
        zmq::message_t reply(MESSAGE_LIMIT);
        subSocket.recv(reply, zmq::recv_flags::none);
        Client* newData = (Client*)reply.data();
        for (int i = 0; i < reply.size() / sizeof(Client); i++) {
            if (newData[i].id < 0) {
                std::cout << std::endl;
                break;
            }
            std::cout << "Client " << newData[i].id << " Iteration " << newData[i].iterations << std::endl;
        }
    }
    return 0;
}
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


int main() {
    //  Prepare our context and socket
    zmq::context_t context(2);
    zmq::socket_t repSocket(context, zmq::socket_type::rep);
    zmq::socket_t pubSocket(context, zmq::socket_type::pub);
    repSocket.bind("tcp://localhost:5555");
    pubSocket.bind("tcp://localhost:5556");

    std::vector<Client> clients;
    Client sentinel;
    sentinel.id = -1;
    clients.push_back(sentinel);
    while (true) { 
        //  Wait for next request from client
        zmq::message_t reply;
        zmq::recv_result_t received(repSocket.recv(reply, zmq::recv_flags::dontwait));
        if ( ( received.has_value() && ( EAGAIN != received.value() ) ) ) {
            zmq::message_t reply;
            memcpy(reply.data(), "Connection Received", 20);
            repSocket.send(reply, zmq::send_flags::none);

            Client newClient;
            newClient.id = (int)clients.size();
            std::cout << newClient.id;
            newClient.iterations = 0;
            clients.insert(clients.end() - 1, newClient);
        }
        sleep(500);
        for (int i = 0; i < clients.size() - 1; i++) {
            clients.at(i).iterations++;
        }
        //Slow down for readability
        //Send reply back to client
        zmq::message_t message(MESSAGE_LIMIT);
        memcpy(message.data(), clients.data(), clients.size() * sizeof(Client));
        pubSocket.send(message, zmq::send_flags::none);
    }
    return 0;
}
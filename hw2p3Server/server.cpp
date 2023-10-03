#include <zmq.hpp>
#include <string>
#include <iostream>
#include <list>
#define MESSAGE_LIMIT 1024

#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>

#define sleep(n)	Sleep(n)
#endif

struct Client{
    int offset = 1;
    int id = -1;
};


int main() {
    //  Prepare our context and socket
    zmq::context_t context(2);
    zmq::socket_t repSocket(context, zmq::socket_type::rep);
    zmq::socket_t pubSocket(context, zmq::socket_type::pub);
    repSocket.bind("tcp://localhost:5555");
    pubSocket.bind("tcp://localhost:5556");

    int iterations = 0;
    int numClients = 0;
    std::list<Client> clients;
    while (true) { 
        //  Wait for next request from client
        zmq::message_t reply;
        zmq::recv_result_t received(repSocket.recv(reply, zmq::recv_flags::dontwait));
        if ( ( received.has_value() && ( EAGAIN != received.value() ) ) ) {
            zmq::message_t reply;
            memcpy(reply.data(), "Connection Received", 20);
            repSocket.send(reply, zmq::send_flags::none);

            Client newClient;
            newClient.id = numClients++;
            newClient.offset = iterations;
            clients.push_front(newClient);
        }
        char rtnString[MESSAGE_LIMIT] = "";
        for (Client i : clients) {
            char clientString[MESSAGE_LIMIT];
            sprintf_s(clientString, "Client %d: Iteration %d\n\n", i.id, iterations - i.offset);
            strcat_s(rtnString, clientString);
        }
        sleep(500);
        //Slow down for readability
        //  Send reply back to client
        if (numClients > 0) {
            zmq::message_t message(MESSAGE_LIMIT);
            memcpy(message.data(), rtnString, strlen(rtnString) + 1);
            pubSocket.send(message, zmq::send_flags::none);
        }
        iterations++;
    }
    return 0;
}
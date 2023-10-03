#include <zmq.hpp>
#include <string>
#include <iostream>
#define MESSAGE_LIMIT 1024

int main()
{
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t reqSocket(context, zmq::socket_type::req);
    zmq::socket_t subSocket(context, zmq::socket_type::sub);

    reqSocket.connect("tcp://localhost:5555");
    subSocket.connect("tcp://localhost:5556");

    const char *opt = "Client";

    zmq::message_t request;
    char buffer[] = "Connecting";
    memcpy(request.data(), buffer, 11);
    reqSocket.send(request, zmq::send_flags::none);

    subSocket.set(zmq::sockopt::subscribe, "");
    while (true) {
        //  Get the reply.
        zmq::message_t reply(MESSAGE_LIMIT);
        subSocket.recv(reply, zmq::recv_flags::none);
        std::cout << (char*)reply.data();
    }
    return 0;
}
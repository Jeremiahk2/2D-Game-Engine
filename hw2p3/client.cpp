#include <zmq.hpp>
#include <string>
#include <iostream>
#define MESSAGE_LIMIT 1024

int main()
{
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, zmq::socket_type::dealer);

    socket.connect("tcp://localhost:5555");

    zmq::message_t request;
    char buffer[] = "Connecting";
    memcpy(request.data(), buffer, 11);
    socket.send(request, zmq::send_flags::none);
    while (true) {
        //  Get the reply.
        zmq::message_t reply;
        socket.recv(reply, zmq::recv_flags::none);
        std::cout << (char *)reply.data() << std::endl;
    }
    return 0;
}
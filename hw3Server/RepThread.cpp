#include "RepThread.h"



RepThread::RepThread(int port, int id) {
    this->port = port;
    this->id = id;
}

bool RepThread::isBusy() {
}

void RepThread::run() {

    //Bind ourself to the port.
    char portString[21];
    sprintf_s(portString, "%s%d", "tcp://localhost:", port);

    zmq::context_t context(2);
    zmq::socket_t repSocket(context, zmq::socket_type::rep);
    repSocket.bind("tcp://localhost:5556");

    char characterString[MESSAGE_LIMIT];
    sprintf_s(characterString, "Connect");
    zmq::message_t request(strlen(characterString) + 1);
    memcpy(request.data(), &characterString, strlen(characterString) + 1);
    repSocket.send(request, zmq::send_flags::none);

    while (true) {
        //Receive message from client
        zmq::message_t update;
        zmq::recv_result_t received(repSocket.recv(update, zmq::recv_flags::none));
        char* current = (char*)update.data();

        CharStruct newCharacter;
        char status; // 'd' for disconnect, 'c' for connect
        //Get client info
        int matches = sscanf_s(current, "%d %f %f %c", &(newCharacter.id), &(newCharacter.x), &(newCharacter.y), &status, 1);

        //If it is a client that is disconnecting, set their ID as negative, so that other clients know not to render them
        if (status == 'd' && numClients != 0 && newCharacter.id >= 0 && newCharacter.id < 10) {
            characters[newCharacter.id].id = (characters[newCharacter.id].id + 1) * -1;
            numClients--;
        }
        else if (status == 'c') { //If it's a returning client, update it's position only.
            characters[newCharacter.id].x = newCharacter.x;
            characters[newCharacter.id].y = newCharacter.y;
        }
        //Send a response with the character's new ID
        char response[MESSAGE_LIMIT];
        sprintf_s(response, "%d", newCharacter.id);
        zmq::message_t reply(strlen(response) + 1);
        memcpy(reply.data(), response, strlen(response) + 1);
        repSocket.send(reply, zmq::send_flags::none);
    }
}
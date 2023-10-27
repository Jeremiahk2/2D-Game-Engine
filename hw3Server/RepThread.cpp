#include "RepThread.h"



RepThread::RepThread(int port, int id, std::map<int, std::shared_ptr<GameObject>> *characters, std::mutex *m, Timeline *time) {
    this->port = port;
    this->id = id;
    this->characters = characters;
    this->mutex = m;
    this->time = time;
}

bool RepThread::isBusy() {
}

void RepThread::run() {

    //Bind ourself to the port.
    char portString[21];
    sprintf_s(portString, "%s%d", "tcp://localhost:", port);

    zmq::context_t context(2);
    zmq::socket_t repSocket(context, zmq::socket_type::rep);
    repSocket.bind(portString);

    int64_t tic = 0;
    int64_t currentTic = 0;
    float ticLength;
    while (true) {

        ticLength = time->getRealTicLength();
        currentTic = time->getTime();
        if (currentTic > tic) {
            //Receive message from client
            zmq::message_t update;
            zmq::recv_result_t received(repSocket.recv(update, zmq::recv_flags::none));
            std::string updateString((char *)update.data());

            //Make the character from the string we were given.
            std::shared_ptr<GameObject> character(new Character);
            *character = *(character->constructSelf(updateString));
            Character* charPtr = (Character*)character.get();

            //If it is a client that is disconnecting, remove them from the map
            if (!(charPtr->isConnecting())) {
                std::lock_guard<std::mutex> lock(*mutex);
                characters->erase(id);
            }
            else if (charPtr->isConnecting()) { //If it's a returning client, update it with the new information
                std::lock_guard<std::mutex> lock(*mutex);
                characters->insert_or_assign(id, character);
            }
            //Send a response with the character's new ID

            std::stringstream stream;
            stream << id;
            std::string rtnString;
            getline(stream, rtnString);

            zmq::message_t reply(rtnString.size() + 1);
            memcpy(reply.data(), rtnString.data(), rtnString.size() + 1);
            repSocket.send(reply, zmq::send_flags::none);
        }
        tic = currentTic;
    }
}

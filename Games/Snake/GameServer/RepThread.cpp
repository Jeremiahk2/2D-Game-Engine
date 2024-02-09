#include "RepThread.h"



RepThread::RepThread(int port, int id, int *highScore, std::mutex *m, Timeline *time, EventManager *manager) {
    this->port = port;
    this->id = id;
    this->highScore = highScore;
    this->mutex = m;
    this->time = time;
    this->manager = manager;
}

void RepThread::run() {
    //Bind ourself to the port.
    char portString[21];
    sprintf_s(portString, "%s%d", "tcp://localhost:", port);

    zmq::context_t context(2);
    zmq::socket_t repSocket(context, zmq::socket_type::rep);
    repSocket.connect(portString);

    Event init;
    init.type = "Client_Closed";
    std::string message = "Game Over";
    Event::variant messageVariant;
    messageVariant.m_Type = Event::variant::TYPE_STRING;
    messageVariant.m_asString = message.data();
    init.parameters.insert({ "message", messageVariant });

    Event::variant socketVariant;
    socketVariant.m_Type = Event::variant::TYPE_SOCKET;
    socketVariant.m_asSocket = &repSocket;
    init.parameters.insert({ "socket", socketVariant });

    zmq::message_t update;
    zmq::recv_result_t received(repSocket.recv(update, zmq::recv_flags::none));

    init.time = GAME_LENGTH - manager->getTimeline()->getGlobalTime(); //Time differential
    std::string rtnString = init.toString();


    zmq::message_t reply(rtnString.size() + 1);
    memcpy(reply.data(), rtnString.data(), rtnString.size() + 1);

    repSocket.send(reply, zmq::send_flags::none);

    int64_t tic = time->getTime();
    int64_t currentTic = tic;
    int64_t heartbeatTic = 0;
    float ticLength;
    bool connected = true;
    while (connected) {

        ticLength = time->getRealTicLength();
        currentTic = time->getTime();
        while (currentTic > tic) {
            currentTic = time->getTime();
            //Receive message from client
            zmq::message_t update;
            zmq::recv_result_t received;
            {
                std::lock_guard<std::mutex> lock(*mutex);
                received = repSocket.recv(update, zmq::recv_flags::dontwait);
            }
            if ((received.has_value() && (EAGAIN != received.value()))) {
                std::string updateString((char*)update.data());
                int score = stoi(updateString);

                //If it is a client that is disconnecting, remove them from the map
                if (score <= 0) {
                    connected = false;
                }
                if (score > *highScore) {
                    std::lock_guard<std::mutex> lock(*mutex);
                    *highScore = score;
                }
                //Send a response with the character's new ID

                std::stringstream stream;
                stream << "Connected";
                std::string rtnString;
                std::getline(stream, rtnString);

                zmq::message_t reply(rtnString.size() + 1);
                memcpy(reply.data(), rtnString.data(), rtnString.size() + 1);
                repSocket.send(reply, zmq::send_flags::none);
                tic = currentTic;
            }
            //Disconnect client if we haven't heard from them in 100 tics.
            else if (currentTic - tic >= 100) {
                std::lock_guard<std::mutex> lock(*mutex);
                tic = currentTic;
                connected = false;
            }
        }
    }
}

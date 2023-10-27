#include "PubThread.h"


PubThread::PubThread(Timeline *time, std::list<MovingPlatform*> *movings, std::map<int, std::shared_ptr<GameObject>>* characters, std::mutex* m) {
    this->time = time;
    this->movings = movings;
    this->characters = characters;
    this->mutex = m;
}

bool PubThread::isBusy() {
}

void PubThread::run() {

    zmq::context_t context(2);
    zmq::socket_t pubSocket(context, zmq::socket_type::pub);
    pubSocket.bind("tcp://localhost:5555");

    int64_t tic = 0;
    int64_t currentTic = 0;
    float ticLength;
    while (true) {
        ticLength = time->getRealTicLength();
        currentTic = time->getTime();

        if (currentTic > tic) {
            //Construct return string
            std::string rtnString;
            std::stringstream stream;
            for (MovingPlatform* i : *movings) {
                stream << rtnString << ',';
            }
            {
                std::lock_guard<std::mutex> lock(*mutex);
                for (auto iter = characters->begin(); iter != characters->end(); ++iter) {
                    stream << iter->second->toString() << ',';
                }
            }
            getline(stream, rtnString);
            zmq::message_t rtnMessage(rtnString.size() + 1);
            memcpy(rtnMessage.data(), rtnString.data(), rtnString.size() + 1);
            pubSocket.send(rtnMessage, zmq::send_flags::none);
        }
        tic = currentTic;
    }
}
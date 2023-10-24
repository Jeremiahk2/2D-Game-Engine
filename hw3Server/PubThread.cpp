#include "PubThread.h"


PubThread::PubThread(Timeline *time, std::list<MovingPlatform*> *movings, std::map<int, CharStruct>* characters) {
    this->time = time;
    this->movings = movings;
    this->characters = characters;
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

            //TODO: Cmbine these strings?

            //Construct platform position string
            char platRtnString[MESSAGE_LIMIT] = "";
            for (MovingPlatform* i : *movings) {
                char platString[MESSAGE_LIMIT];
                sprintf_s(platString, "%f %f ", i->getPosition().x, i->getPosition().y);
                strcat_s(platRtnString, platString);
            }
            //Send platform information
            zmq::message_t sendPlatforms(strlen(platRtnString) + 1);
            memcpy(sendPlatforms.data(), platRtnString, strlen(platRtnString) + 1);
            pubSocket.send(sendPlatforms, zmq::send_flags::none);

            //Construct character position string
            char charRtnString[MESSAGE_LIMIT] = "";
            for (auto iter = characters->begin(); iter != characters->end(); ++iter) {
                char charString[MESSAGE_LIMIT];
                sprintf_s(charString, "%d %f %f ", iter->second.id, iter->second.x, iter->second.y);
                strcat_s(charRtnString, charString);
            }
            //Send character information
            zmq::message_t sendCharacters(strlen(charRtnString) + 1);
            memcpy(sendCharacters.data(), charRtnString, strlen(charRtnString) + 1);
            pubSocket.send(sendCharacters, zmq::send_flags::none);
        }
    }
}
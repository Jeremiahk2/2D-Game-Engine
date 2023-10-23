#include "PubThread.h"


PubThread::PubThread() {
}

bool PubThread::isBusy() {
}

void PubThread::run() {
    //Done processing character updates, return platforms and characters
    if (numClients > 0) {

        //TODO: Cmbine these strings?

        //Construct platform position string
        char platRtnString[MESSAGE_LIMIT] = "";
        for (MovingPlatform* i : movings) {
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
        for (int i = 0; i < numCharacters; i++) {
            char charString[MESSAGE_LIMIT];
            sprintf_s(charString, "%d %f %f ", characters[i].id, characters[i].x, characters[i].y);
            strcat_s(charRtnString, charString);
        }
        //Send character information
        zmq::message_t sendCharacters(strlen(charRtnString) + 1);
        memcpy(sendCharacters.data(), charRtnString, strlen(charRtnString) + 1);
        pubSocket.send(sendCharacters, zmq::send_flags::none);
    }
}
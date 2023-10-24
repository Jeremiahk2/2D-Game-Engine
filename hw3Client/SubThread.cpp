#include "SubThread.h"

SubThread::SubThread(bool* stopped, GameWindow* window, CThread* other, bool* busy, std::condition_variable* rscv,
    Timeline* timeline, std::mutex *m) {
    this->stopped = stopped;
    this->rswindow = window;
    this->other = other;
    this->rsbusy = busy;
    this->rscv = rscv;
    this->rstime = timeline;
    this->mutex = m;

}

void SubThread::run() {
    //  Prepare our context and socket
    zmq::context_t context(2);
    zmq::socket_t subSocket(context, zmq::socket_type::sub);

    Character* character = rswindow->getCharacter();

    list<MovingPlatform*>* movings = rswindow->getMovings();

    int64_t tic = 0;
    int64_t currentTic = 0;
    float ticLength;
    subSocket.connect("tcp://localhost:5555");
    subSocket.set(zmq::sockopt::subscribe, "");
    while (*stopped == false) {
        ticLength = rstime->getRealTicLength();
        currentTic = rstime->getTime();
        if (currentTic > tic) {

            //Receive updated platforms
            zmq::message_t newPlatforms;
            subSocket.recv(newPlatforms, zmq::recv_flags::none);
            char* platformsString = (char*)newPlatforms.data();
            int pos = 0;
            {
                std::lock_guard<std::mutex> lock(*mutex);
                for (MovingPlatform* i : *movings) {
                    float x = 0;
                    float y = 0;
                    int matches = sscanf_s(platformsString + pos, "%f %f %n", &x, &y, &pos);

                    i->move(x - i->getPosition().x, y - i->getPosition().y);
                }
            }
            *rsbusy = true;

            //Receive updated characters
            zmq::message_t newCharacters;
            subSocket.recv(newCharacters, zmq::recv_flags::none);
            char* newCharString = (char*)newCharacters.data();
            //Update the characters in the window with new ones
            rswindow->updateCharacters(newCharString);
        }
        tic = currentTic;
    }
}

bool SubThread::isBusy() {
    return *rsbusy;
}


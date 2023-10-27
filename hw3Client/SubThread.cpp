#include "SubThread.h"

SubThread::SubThread(bool* stopped, GameWindow* window, CThread* other, bool* busy, std::condition_variable* rscv,
    Timeline* timeline, std::mutex *m ) {
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

    Character* character = (Character *)rswindow->getPlayableObject();

    int64_t tic = 0;
    int64_t currentTic = 0;
    float ticLength;
    subSocket.connect("tcp://localhost:5555");
    subSocket.set(zmq::sockopt::subscribe, "");
    while (*stopped == false) {
        ticLength = rstime->getRealTicLength();
        currentTic = rstime->getTime();
        if (currentTic > tic) {

            //Receive updates to nonstatic objects. Should be comma separated string.
            zmq::message_t newPlatforms;
            subSocket.recv(newPlatforms, zmq::recv_flags::none);
            std::string objectsString((char *)newPlatforms.data());

            //Update window with new objects.
            rswindow->updateNonStatic(objectsString);
        }
        tic = currentTic;
    }
}

bool SubThread::isBusy() {
    return *rsbusy;
}


#include "ReqThread.h"

ReqThread::ReqThread(bool *stopped, GameWindow *window, CThread *other, bool *busy, std::condition_variable *rscv,
                           Timeline *timeline, std::mutex *m) {
    this->stopped = stopped;
    this->rswindow = window;
    this->other = other;
    this->rsbusy = busy;
    this->rscv = rscv;
    this->rstime = timeline;
    this->mutex = m;

}

void ReqThread::run() {
    //  Prepare our context and socket
    zmq::context_t context(2);
    zmq::socket_t reqSocket(context, zmq::socket_type::req);

    Character* character = (Character*)rswindow->getPlayableObject();

    //Connect and get your own port.
    reqSocket.connect("tcp://localhost:5556");

    //Send the request to the server.
    char initRecvString[MESSAGE_LIMIT];
    sprintf_s(initRecvString, "Connect");
    zmq::message_t initRequest(strlen(initRecvString) + 1);
    memcpy(initRequest.data(), initRecvString, strlen(initRecvString) + 1);
    reqSocket.send(initRequest, zmq::send_flags::none);
    //Receive the reply from the server, should contain our port and ID
    zmq::message_t initReply;
    reqSocket.recv(initReply, zmq::recv_flags::none);;
    char* initReplyString = (char*)initReply.data();
    int initId = -1;
    int port = -1;
    int matches = sscanf_s(initReplyString, "%d %d", &initId, &port);

    //Exit if we didn't get a proper reply
    if (matches != 2) {
        exit(2);
    }
    //Set our character's ID and configure port string
    character->setID(initId);
    char portString[21];
    sprintf_s(portString, "%s%d", "tcp://localhost:", port);

    //Disconnect from main server process.
    reqSocket.disconnect("tcp://localhost:5556");
    //Connect to your unique port provided by the server.
    reqSocket.connect(portString);

    int64_t tic = 0;
    int64_t currentTic = 0;
    float ticLength;
    while (*stopped == false) {
        ticLength = rstime->getRealTicLength();
        currentTic = rstime->getTime();

        if (currentTic > tic) {
            //Send updated character information to server
            zmq::message_t reply;
            {
                std::lock_guard<std::mutex> lock(*mutex);
                std::string charString = character->toString();

                zmq::message_t request(charString.data() + 1);
                memcpy(request.data(), charString.data(), charString.size() + 1);
                reqSocket.send(request, zmq::send_flags::none);
            }

            //Receive confirmation
            reqSocket.recv(reply, zmq::recv_flags::none);
            char* replyString = (char*)reply.data();
            int newID;
            int matches = sscanf_s(replyString, "%d", &newID);
            character->setID(newID);
        }
        tic = currentTic;
    }

    //Disconnect
    std:string charString;
    character->setConnecting(false);
    charString = character->toString();


    zmq::message_t request(charString.size() + 1);
    memcpy(request.data(), charString.data(), charString.size() + 1);
    reqSocket.send(request, zmq::send_flags::none);

    //Receive confirmation
    zmq::message_t reply;
    reqSocket.recv(reply, zmq::recv_flags::none);
}

bool ReqThread::isBusy() {
    return *rsbusy;
}


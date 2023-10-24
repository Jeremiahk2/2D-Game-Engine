#include "ReqSubThread.h"

ReqSubThread::ReqSubThread(bool *stopped, GameWindow *window, CThread *other, bool *busy, std::condition_variable *rscv,
                           Timeline *timeline) {
    this->stopped = stopped;
    this->rswindow = window;
    this->other = other;
    this->rsbusy = busy;
    this->rscv = rscv;
    this->rstime = timeline;

}

void ReqSubThread::run() {
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t reqSocket(context, zmq::socket_type::req);
    zmq::socket_t subSocket(context, zmq::socket_type::sub);

    Character* character = rswindow->getCharacter();

    list<MovingPlatform*>* movings = rswindow->getMovings();

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
    reqSocket.recv(initReply, zmq::recv_flags::none);
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

    //Connect to general subsocket.
    subSocket.connect("tcp://localhost:5555");
    subSocket.set(zmq::sockopt::subscribe, "");

    int64_t tic = 0;
    int64_t currentTic = 0;
    float ticLength;
    while (*stopped = false) {
        ticLength = rstime->getRealTicLength();
        currentTic = rstime->getTime();

        if (currentTic > tic) {
            //Send updated character information to server
            char characterString[MESSAGE_LIMIT];
            sprintf_s(characterString, "%d %f %f %c", character->getID(), character->getPosition().x, character->getPosition().y, 'c');
            zmq::message_t request(strlen(characterString) + 1);
            memcpy(request.data(), &characterString, strlen(characterString) + 1);
            reqSocket.send(request, zmq::send_flags::none);

            //Receive confirmation
            zmq::message_t reply;
            reqSocket.recv(reply, zmq::recv_flags::none);
            char* replyString = (char*)reply.data();
            int newID;
            int matches = sscanf_s(replyString, "%d", &newID);
            character->setID(newID);
            if (newID > 9) {
                exit(1);
            }

            //Receive updated platforms
            zmq::message_t newPlatforms;
            subSocket.recv(newPlatforms, zmq::recv_flags::none);

            char* platformsString = (char*)newPlatforms.data();
            int pos = 0;
            for (MovingPlatform* i : *movings) {
                float x = 0;
                float y = 0;
                int matches = sscanf_s(platformsString + pos, "%f %f %n", &x, &y, &pos);

                i->move(x - i->getPosition().x, y - i->getPosition().y);
            }
            while (other->isBusy())
            {
                rscv->notify_all();
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

    //Disconnect
    char characterString[MESSAGE_LIMIT];
    sprintf_s(characterString, "%d %f %f %c", character->getID(), character->getPosition().x, character->getPosition().y, 'd');
    zmq::message_t request(strlen(characterString) + 1);
    memcpy(request.data(), &characterString, strlen(characterString) + 1);
    reqSocket.send(request, zmq::send_flags::none);

    //Receive confirmation
    zmq::message_t reply;
    reqSocket.recv(reply, zmq::recv_flags::none);
}

bool ReqSubThread::isBusy() {
    return *rsbusy;
}


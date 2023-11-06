#include "CThread.h"


bool CThread::isBusy()
{
    std::lock_guard<std::mutex> lock(*mutex);  // this locks the mutuex until the variable goes out of scope (i.e., when the function returns in this case)
    return *busy;
}

CThread::CThread(bool* upPressed, GameWindow* window, Timeline* timeline, bool* stopped,
    std::mutex* m, std::condition_variable* cv, bool* busy, EventManager *em)
{
    this->mutex = m;
    this->cv = cv;
    this->stop = stopped;
    this->line = timeline;
    this->window = window;
    this->upPressed = upPressed;
    this->busy = busy;
    this->em = em;
    std::string type("collision");
    std::list<std::string> types;
    types.push_back(type);
    em->registerEvent(types, new CollisionHandler);

    type = "movement";
    types.clear();
    types.push_back(type);
    em->registerEvent(types, new MovementHandler);

    type = "gravity";
    types.clear();
    types.push_back(type);
    em->registerEvent(types, new GravityHandler);
}

void CThread::run() {
    int64_t tic = 0;
    int64_t currentTic;
    float ticLength;
    window->setActive(true);

    Character *character = (Character *)window->getPlayableObject();

    //  Prepare our context and socket
    zmq::context_t context(2);
    zmq::socket_t subSocket(context, zmq::socket_type::sub);

    zmq::socket_t reqSocket(context, zmq::socket_type::req);

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
    auto r = reqSocket.recv(initReply, zmq::recv_flags::none);;
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
    //Bind to your unique port provided by the server.
    reqSocket.bind(portString);
    //Conflate messages to avoid getting behind.
    subSocket.set(zmq::sockopt::conflate, "");
    subSocket.connect("tcp://localhost:5555");
    subSocket.set(zmq::sockopt::subscribe, "");

    int moves = 0;
    bool direction = false;
    float jumpTime = JUMP_TIME;
    //For movement on top of horizontal platforms.
    float nonScalableTicLength = line->getNonScalableTicLength();

    while (!(*stop)) {
        currentTic = line->getTime();
        if (currentTic > tic) {
            //Get information from server

            //Receive updates to nonstatic objects. Should be comma separated string.
            zmq::message_t newPlatforms;
            r = subSocket.recv(newPlatforms, zmq::recv_flags::none);
            std::string updates((char*)newPlatforms.data());

            int pos = 0;
            int newPos = 0;
            char* currentObject = (char*)malloc(updates.size() + 1);

            std::stringstream stream;

            //Scan through each object looking for just characters
            while (sscanf_s(updates.data() + pos, "%[^,]%n", currentObject, (unsigned int)(updates.size() + 1), &newPos) == 1) {
                //Get the type of the current object.
                int type;
                int matches = sscanf_s(currentObject, "%d", &type);
                if (matches != 1) {
                    throw std::invalid_argument("Failed to read string. Type must be the first value.");
                }
                if (type == Character::objectType) {
                    stream << currentObject << ",";
                    //update position and skip past comma
                    pos += newPos + 1;
                }
                else {
                    break;
                }
            }
            std::string charStrings;
            std::getline(stream, charStrings);
            //Update window with new characters
            window->updateNonStatic(charStrings);

            //Sync with visuals
            {
                std::lock_guard<std::mutex> lock(*mutex);
                window->update();
                //Update the rest
                std::string otherStuff(updates.data() + pos);
                window->updateNonStatic(otherStuff);

                //Set up physics variables
                ticLength = line->getRealTicLength();
                *upPressed = false;
                float gravity = character->getGravity() * ticLength * (currentTic - tic);
                float oneHalfTicGrav = (character->getGravity() * ticLength) / 2;
                GameObject* collision = nullptr;
                bool doGravity = true;
                //Set up physics events.
                //Set up collision event.
                Event c;
                c.type = std::string("collision");
                Event::variant upPressedVariant;
                upPressedVariant.m_Type = Event::variant::TYPE_BOOLP;
                upPressedVariant.m_asBoolP = upPressed;
                Event::variant doGravityVariant;
                doGravityVariant.m_Type = Event::variant::TYPE_BOOLP;
                doGravityVariant.m_asBoolP = &doGravity;
                Event::variant characterVariant;
                characterVariant.m_Type = Event::variant::TYPE_GAMEOBJECT;
                characterVariant.m_asGameObject = character;
                Event::variant ticLengthVariant;
                ticLengthVariant.m_Type = Event::variant::TYPE_FLOAT;
                ticLengthVariant.m_asFloat = ticLength;
                Event::variant differentialVariant;
                differentialVariant.m_Type = Event::variant::TYPE_INT;
                differentialVariant.m_asInt = currentTic - tic;
                c.parameters.insert({ { "upPressed", upPressedVariant }, { "doGravity", doGravityVariant } });
                c.parameters.insert({ "character", characterVariant });
                c.parameters.insert({ "ticLength", ticLengthVariant });
                c.parameters.insert({ "differential", differentialVariant });
                //Set up gravity event.
                Event g = c;
                g.type = std::string("gravity");
                g.order = 1;
                Event::variant nonScalableTicLengthVariant;
                nonScalableTicLengthVariant.m_Type = Event::variant::TYPE_FLOAT;
                nonScalableTicLengthVariant.m_asFloat = nonScalableTicLength;
                Event::variant windowVariant;
                windowVariant.m_Type = Event::variant::TYPE_GAMEWINDOW;
                windowVariant.m_asGameWindow = window;
                g.parameters.insert({ "nonScalableTicLength", nonScalableTicLengthVariant });
                g.parameters.insert({ "window", windowVariant });

                //Check for collisions, add it to event manager if there is one.
                if (window->checkCollisions(&collision)) {
                    //Add collision event to queue.
                    Event::variant collisionVariant;
                    collisionVariant.m_Type = Event::variant::TYPE_GAMEOBJECT;
                    collisionVariant.m_asGameObject = collision;
                    c.parameters.insert({ "collision", collisionVariant });
                    em->raise(c);
                }
                //Handle gravity as well.
                em->raise(g);
                
                //Handle all events that have come up.
                bool erase = false;
                for (const auto& [time, orderMap] : em->raised_events) {
                    if (time <= currentTic) {
                        for (const auto& [order, e] : orderMap) {
                            for (EventHandler* currentHandler : em->handlers.at(e.type)) {
                                currentHandler->onEvent(e);
                            }
                        }
                        if (erase) {
                            em->raised_events.erase(em->raised_events.begin());
                        }
                        erase = true;
                    }
                    else {
                        break;
                    }
                }
                if (erase) {
                    em->raised_events.erase(em->raised_events.begin());
                }


                //Handle jumping.
                float frameJump = JUMP_SPEED * (float)ticLength * (float)(currentTic - tic);

                if (character->isJumping()) {
                    character->move(0, -1 * frameJump);
                    jumpTime -= (float)ticLength * (float)(currentTic - tic);
                    bool jumpCollides = window->checkCollisions(&collision);
                    if (jumpCollides && collision->getObjectType() == SideBound::objectType) {
                        //Don't stop the jump if it's just a side bound.
                        jumpCollides = false;
                    }
                    //Exit jumping if there are no more jump frames or if we collided with something.
                    if (jumpTime <= 0 || jumpCollides) {
                        if (jumpCollides) {
                            character->move(0, frameJump);
                        }
                        character->setJumping(false);
                        jumpTime = JUMP_TIME;
                    }
                }
            }
            
            //Send updated character information to server
            {
                std::lock_guard<std::mutex> lock(*mutex);
                std::string charString = character->toString();
                zmq::message_t request(charString.size() + 1);
                memcpy(request.data(), charString.data(), charString.size() + 1);
                reqSocket.send(request, zmq::send_flags::none);
            }
            //Receive confirmation
            zmq::message_t reply;
            r = reqSocket.recv(reply, zmq::recv_flags::none);
            char* replyString = (char*)reply.data();
            int newID;
            int matches = sscanf_s(replyString, "%d", &newID);
            character->setID(newID);
            tic = currentTic;
        }
    }
    //Tell the server we are disconnecting
    character->setConnecting(false);
    std::string charString = character->toString();

    zmq::message_t request(charString.size() + 1);
    memcpy(request.data(), charString.data(), charString.size() + 1);
    reqSocket.send(request, zmq::send_flags::none);

    //Receive confirmation
    zmq::message_t reply;
    r = reqSocket.recv(reply, zmq::recv_flags::none);
    window->setActive(false);
}
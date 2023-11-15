#include "PubThread.h"


PubThread::PubThread(Timeline *timeline, std::list<MovingPlatform*> *movings, std::map<int, std::shared_ptr<GameObject>>* characters, std::mutex* m, EventManager *manager) {
    this->timeline = timeline;
    this->movings = movings;
    this->characters = characters;
    this->mutex = m;
    this->manager = manager;
}

void PubThread::run() {

    zmq::context_t context(2);
    zmq::socket_t pubSocket(context, zmq::socket_type::pub);
    pubSocket.bind("tcp://localhost:5555");

    int64_t tic = 0;
    int64_t currentTic = 0;
    float ticLength;
    int moves = 0;
    while (true) {
        ticLength = timeline->getRealTicLength();
        currentTic = timeline->getTime();

        if (currentTic > tic) {
            //Construct return string
            std::string rtnString;
            std::stringstream stream;
            {
                //Add characters to stream
                std::lock_guard<std::mutex> lock(*mutex);
                for (auto iter = characters->begin(); iter != characters->end(); ++iter) {
                    stream << iter->second->toString() << ',';
                }
            }
            //Add moving platforms to string
            for (MovingPlatform* i : *movings) {
                stream << i->toString() << ',';
            }

            std::getline(stream, rtnString);
            //Send the string to the players
            zmq::message_t rtnMessage(rtnString.size() + 1);
            memcpy(rtnMessage.data(), rtnString.data(), rtnString.size() + 1);
            pubSocket.send(rtnMessage, zmq::send_flags::none);


            for (MovingPlatform* i : *movings) {

                sf::Vector2i bounds = i->getBounds();
                //boundx is left or bottom bound
                //If the movement is horizontal
                if ((i->getMovementType())) {
                    //If we are moving to the right
                    if (i->getSpeedValue() > 0) {
                        //Then we only care about our right bound.
                        if ((int)i->getGlobalBounds().left > bounds.y) {
                            i->setSpeed(sf::Vector2f(i->getSpeedValue() * -1.f, 0));
                        }
                    }
                    //Else if we are moving left, check the left bound.
                    else if ((int)i->getGlobalBounds().left < bounds.x) {
                        i->setSpeed(sf::Vector2f(i->getSpeedValue() * -1.f, 0));
                    }
                }
                //If movement is vertical
                else {
                    //If we are going up
                    if (i->getSpeedValue() < 0) {
                        //Then we only care about or top bound.
                        if ((int)i->getGlobalBounds().top < bounds.y) {
                            i->setSpeed(sf::Vector2f(0, i->getSpeedValue() * -1.f));
                        }
                    }
                    //else if we are going down, we only care about our bottom bound.
                    else if ((int)i->getGlobalBounds().top > bounds.x) {
                        i->setSpeed(sf::Vector2f(0, i->getSpeedValue() * -1.f));
                    }
                }
                float platSpeed = (float)i->getSpeedValue() * (float)ticLength * (float)(currentTic - tic);
                //TODO: I swapped the order. Check back later

                //If the platform is moving horizontally
                if ((i->getMovementType())) {
                    i->move(platSpeed, 0);
                }
                else {
                    i->move(0, platSpeed);
                }

                bool erase = false;
                {
                    std::lock_guard<std::mutex> lock(*mutex);
                    for (const auto& [time, orderMap] : manager->raised_events) {
                        if (time <= timeline->convertGlobal(currentTic)) {
                            for (const auto& [order, e] : orderMap) {
                                for (EventHandler* currentHandler : manager->handlers.at(e.type)) {
                                    currentHandler->onEvent(e);
                                }
                            }
                            if (erase) {
                                manager->raised_events.erase(manager->raised_events.begin());
                            }
                            erase = true;
                        }
                        else {
                            break;
                        }
                    }
                    if (erase) {
                        manager->raised_events.erase(manager->raised_events.begin());
                    }
                }
            }
            tic = currentTic;
        }
    }
}
#include "EventThread.h"


GameWindow* ewindow;

Timeline* eglobal;

bool* estopped;

std::condition_variable* ecv;



EventThread::EventThread(GameWindow *window, Timeline *global, bool *stopped, std::condition_variable* ecv)
{
    this->ewindow = window;
    this->eglobal = global;
    this->estopped = stopped;
    this->ecv = ecv;

}

void EventThread::run() {
    
    double scale = 1.0;
    while (ewindow->isOpen()) {
        sf::Event event;
        while (ewindow->pollEvent(event)) {
            
            if (event.type == sf::Event::Closed) {
                *estopped = true;
                //Need to notify all so they can stop
                ecv->notify_all();
                /*ewindow->close();*/
            }
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Q)) {
                ewindow->changeScaling();
            }
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::P)) {
                if (eglobal->isPaused()) {
                    eglobal->unpause();
                }
                else {
                    eglobal->pause();
                }
            }
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Left)) {
                if (scale != .5) {
                    scale *= .5;
                    eglobal->changeScale(scale);
                }
            }
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Right)) {
                if (scale != 2.0) {
                    scale *= 2.0;
                    eglobal->changeScale(scale);
                }
            }
            if (event.type == sf::Event::Resized)
            {
                ewindow->handleResize(event);
            }
        }
    }
}
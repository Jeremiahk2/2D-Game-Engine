#include "EventThread.h"


GameWindow ewindow;

Timeline* eglobal;

bool* estopped;

std::condition_variable* ecv;



EventThread::EventThread(Timeline* global, bool* stopped, std::condition_variable* ecv) : ewindow()
{
    this->eglobal = global;
    this->estopped = stopped;
    this->ecv = ecv;

}

GameWindow *EventThread::getWindowPointer() {
    return &ewindow;
}

void EventThread::run() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    ewindow.create(sf::VideoMode(800, 600, desktop.bitsPerPixel), "Window", sf::Style::Default);
    
    double scale = 1.0;
    while (ewindow.isOpen()) {
        sf::Event event;
        while (ewindow.pollEvent(event)) {
            
            if (event.type == sf::Event::Closed) {
                *estopped = true;
                //Need to notify all so they can stop
                ecv->notify_all();
                /*ewindow->close();*/
            }
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Q)) {
                ewindow.changeScaling();
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
                ewindow.handleResize(event);
            }
        }
    }
}
#ifndef TIMELINE_H
#define TIMELINE_H

#include <chrono>
#include <mutex>
#include <SFML/Graphics.hpp>
using namespace std::chrono;

//Default for lowest-level timeline (real time).
#define DEFAULT_TIC 1

class Timeline {
private:
    /**
    * The start time of the timeline
    * For anchored timelines, this will probably be 0, but if it was started later it will return the number of tics since the anchor started.
    * For anchor timelines, this will be the time since last epoc in milliseconds
    */
    int64_t start_time;

    /**
    * the TOTAL elapsed time spent paused
    */
    int64_t elapsed_paused_time;

    /**
    * The last time we paused, in terms of tics of this timeline.
    */
    int64_t last_paused_time;

    /**
    * Number of tics on the anchor timeline before one tic passes here.
    */
    int64_t tic;

    /**
    * are we paused?
    */
    bool paused;

    /**
    * Anchor timeline. 
    */
    Timeline *anchor;

    /**
    * Mutex for lock-guarding.
    */
    std::recursive_mutex mutex;

    /**
    * The scale to which the timeline should alter itself to.
    */
    float scale;

public:

    /**
    * Constructor for non-base timeline.
    */
    Timeline(Timeline *anchor, int64_t tic);

    /**
    * Base timeline constructor. Sets anchor to NULL
    * Ticrate is set to 1; 1 millisecond per tic.
    * This kind of timeline should probably never be used save for pausing timelines connected to it.
    */
    Timeline();

    /**
    * Return the number of tics that have passed since starting.
    */
    int64_t getTime();

    /**
    * Pause this timeline (and all timelines anchored to this one)
    */
    void pause();

    /**
    * Unpause this timeline (and all timelines anchored to this one)
    */
    void unpause();

    /**
    * Change the tic rate
    * WARNING: This is not really supported yet. Changing the tic rate arbitrarily may cause issues. Particularly with collision.
    */
    void changeTic(int tic);

    /**
    * Are we paused?
    */
    bool isPaused();

    /**
    * Return the amount of time that is passed per-tick of THIS timeline in real time.
    * Useful for if your objects are in terms of real time (pixels/second).
    * If you are changing scale arbitrarily, you will need to update this every iteration
    */
    float getRealTicLength();
    
    /**
    * Return the tic length without scale applied. 
    */
    float getNonScalableTicLength();

    /**
    * Change the scale of the timeline. 
    * What this actually does is multiply getRealTicLength() by scale.
    * So if you put it at 2, each tic counts for double the time.
    */
    void changeScale(float scale);
};
#endif
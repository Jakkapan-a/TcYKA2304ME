#ifndef Tc_YKA2304ME_h
#define Tc_YKA2304ME_h

#include "Arduino.h"
class TcYKA2304ME
{
  protected:
    int pin_pu = 0;
    int pin_dr = 0;
    int pin_mf = 0;
    bool _state_start = false;
    bool _state_end = false;
    // Encoder
    unsigned long _position;
    unsigned long _previousPosition;
    unsigned long minPosition = 0;
    unsigned long maxPosition = 5000;
    // Debounce
    unsigned long _lastDebounceTime = 0;
	  int _speed = 50;
    int _speedLearning = 100;
    // Direction
    bool _direction = true; // true: forward, false: backward
    bool isLearning = false;
    bool isPulse = false;
    // Learning
    void setDefaultLearning();
    bool learning();

    void (*OnLearned)(unsigned long position, unsigned long minPosition, unsigned long maxPosition);
    void (*OnStart)(unsigned long position, unsigned long minPosition, unsigned long maxPosition);
    void (*OnEnd)(unsigned long position, unsigned long minPosition, unsigned long maxPosition);
    void (*OnUpdate)(unsigned long position, unsigned long minPosition, unsigned long maxPosition);
    void (*OnUpdateLearning)(unsigned long position, unsigned long minPosition, unsigned long maxPosition);
    void (*OnUpdateSpeed)(int speed);
    void (*OnUpdateSpeedLearning)(int speed);
    void (*OnUpdateStateStart)(bool state);
    void (*OnUpdateStateEnd)(bool state);
    void (*OnUpdateDirection)(bool direction);
    void (*OnUpdatePosition)(unsigned long position);
  public:
    TcYKA2304ME(int pin_pu, int pin_dr, int pin_mf);
    void begin();
    void setSpeed(int speed);
    int getSpeed();
    void setSpeedLearning(int speed);
    int getSpeedLearning();
    void setStateStart(bool state);
    void setStateEnd(bool state);
    void update();
    void setPosition(unsigned long position);
    void ForwardToEnd();
    void BackwardToStart();
    
    // Add event listener
    void setOnLearned(void (*function)(unsigned long position, unsigned long minPosition, unsigned long maxPosition));
    void setOnStart(void (*function)(unsigned long position, unsigned long minPosition, unsigned long maxPosition));
    void setOnEnd(void (*function)(unsigned long position, unsigned long minPosition, unsigned long maxPosition));
    void setOnUpdate(void (*function)(unsigned long position, unsigned long minPosition, unsigned long maxPosition));
    void setOnUpdateLearning(void (*function)(unsigned long position, unsigned long minPosition, unsigned long maxPosition));
    void setOnUpdateSpeed(void (*function)(int speed));
    void setOnUpdateSpeedLearning(void (*function)(int speed));
    void setOnUpdateStateStart(void (*function)(bool state));
    void setOnUpdateStateEnd(void (*function)(bool state));
    void setOnUpdateDirection(void (*function)(bool direction));
    void setOnUpdatePosition(void (*function)(unsigned long position));

};
#endif
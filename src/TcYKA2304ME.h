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
	  int _speed = 20;
    int _speedLearning = 200;
    // Pulse
    // unsigned long currentPos = 0;
    // unsigned long targetPos = 0;
    unsigned long totalPulse = 0;
    unsigned long countRun = 0;
    unsigned long speedStep[4] = {20,20,50,100};
    unsigned long increase = 3000;
    unsigned long decrease = 3000;
    // Direction
    bool _direction = true; // true: forward, false: backward
    bool _oldDirection = true;
    bool isLearning = false;
    bool isPulse = false;
    // Learning
    void setDefaultLearning();
    bool learning();

    // Speed
    void setDefaultSpeed();
    long getSpeedPulse();
    unsigned long getSpeedMicros();

    // Event
    void (*OnLearned)(unsigned long position, unsigned long minPosition, unsigned long maxPosition);
    void (*OnStart)(unsigned long position, unsigned long minPosition, unsigned long maxPosition);
    void (*OnEnd)(unsigned long position, unsigned long minPosition, unsigned long maxPosition);
    void (*OnUpdate)(unsigned long position, unsigned long minPosition, unsigned long maxPosition);
    void (*OnUpdated)(unsigned long position, unsigned long minPosition, unsigned long maxPosition);


    void (*OnUpdateLearning)(unsigned long position, unsigned long minPosition, unsigned long maxPosition);
    void (*OnUpdateSpeed)(unsigned long speed); // for debug only
    void (*OnUpdateSpeedLearning)(int speed); // for debug only
    void (*OnUpdateDirection)(bool direction);
    void (*OnUpdatePosition)(unsigned long position);
    void (*OnError)(int code, String message);
    
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
    void setOnUpdated(void (*function)(unsigned long position, unsigned long minPosition, unsigned long maxPosition));
    void setOnUpdateLearning(void (*function)(unsigned long position, unsigned long minPosition, unsigned long maxPosition));
    void setOnUpdateSpeed(void (*function)(unsigned long speed));
    void setOnUpdateSpeedLearning(void (*function)(int speed));
    void setOnUpdateDirection(void (*function)(bool direction));
    void setOnUpdatePosition(void (*function)(unsigned long position));
    void setOnError(void (*function)(int code, String message));
    int MOTOR_OFFSET = 10000;
    // ERROR CODE
    enum ERROR_CODE {
      ERROR_CODE_NONE = 0,
      ERROR_CODE_POSITION = 1,
      ERROR_CODE_SPEED = 2,
      ERROR_CODE_DIRECTION = 3,
      ERROR_CODE_LEARNING = 4,
      ERROR_CODE_START = 5,
      ERROR_CODE_END = 6,
      ERROR_CODE_UNKNOWN = 7
    };
};
#endif
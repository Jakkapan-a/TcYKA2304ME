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

  // Direction
  bool _direction = true; // true: forward, false: backward
  bool _oldDirection = true;
  // Invert direction
  bool _invertDirection = false;

  // Invert MF
  bool _invertMF = false;
  void setMF(bool state);

  bool isLearning = false;
  bool isLearn = false;
  bool isPulse = false;
  // Learning
  void setDefaultLearning();


  // Speed
  void setDefaultSpeed();
  long getSpeedPulse();

  unsigned long getSpeedMicros();
  int countRelay = 0;
  bool isRelayState = false;
  bool isRelay();
  void setRelay(int sec);
  int countSleep = 0;
  int countSuccess = 0;
  // Event
  void (*OnLearned)(unsigned long position, unsigned long minPosition, unsigned long maxPosition);
  void (*OnStart)(unsigned long position, unsigned long minPosition, unsigned long maxPosition);
  void (*OnEnd)(unsigned long position, unsigned long minPosition, unsigned long maxPosition);
  void (*OnUpdate)(unsigned long position, unsigned long minPosition, unsigned long maxPosition);
  void (*OnUpdated)(unsigned long position, unsigned long minPosition, unsigned long maxPosition);

  void (*OnUpdateLearning)(unsigned long position, unsigned long minPosition, unsigned long maxPosition);
  void (*OnUpdateSpeed)(unsigned long speed); // for debug only
  void (*OnUpdateSpeedLearning)(int speed);   // for debug only
  void (*OnUpdateDirection)(bool direction);
  void (*OnUpdatePosition)(unsigned long position);
  void (*OnError)(int code, String message);
  void (*OnWorking)(bool state);

public:
  // Encoder
  unsigned long _position = 0;
  unsigned long _previousPosition = 0;
  unsigned long minPosition = 0;
  unsigned long maxPosition = 20000;
  // Debounce
  unsigned long _lastDebounceTime = 0;
  unsigned int _speed = 20;
  unsigned int _speedLearning = 500;
  unsigned long _lastDebounceTimeMillisCountDown = 0;
  // Offset point
  unsigned int _offsetPoint = 0;
  // Pulse
  // unsigned long currentPos = 0;
  // unsigned long targetPos = 0;
  unsigned long totalPulse = 0;
  unsigned long countRun = 0;
  // unsigned int speedStep[4] = {20,20,100,200};
  unsigned int increase = 500;      // Distance to increase speed
  unsigned int slowStartSpeed = 700; // Speed to start Acceleration
  unsigned int decrease = 500;      // Distance to decrease speed
  unsigned int slowDownSpeed = 700;  // Speed to slow down Deceleration
  unsigned int MOTOR_OFFSET = 20000;


  TcYKA2304ME(int pin_pu, int pin_dr, int pin_mf);
  void begin();
    bool learning();
  int getSpeed();
  int getSpeedLearning();
  void setStateStart(bool state);
  void setStateEnd(bool state);
  void update();
  void ForwardToEnd();
  void BackwardToStart();

  void setSpeed(unsigned int speed);
  void setSpeedLearning(unsigned int speed);
  void setPosition(unsigned long position);
  void setSlowStart(unsigned int speed);
  void setSlowEnd(unsigned int speed);
  void setIncrease(unsigned int distance);
  void setDecrease(unsigned int distance);
  void setOffsetPoint(unsigned int offsetPoint);
  void setToOffsetPoint();
  void setLearning(bool state);

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
  void setInvertDirection(bool invertDirection);
  void setOnWorking(void (*function)(bool state));
  void setInvertMF(bool invertMF);

  // ERROR CODE
  enum ERROR_CODE
  {
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
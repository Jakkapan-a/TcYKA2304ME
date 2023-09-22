#include "TcYKA2304ME.h"
TcYKA2304ME::TcYKA2304ME(int _pin_pu, int _pin_dr, int _pin_mf)
{
    this->pin_pu = _pin_pu;
    this->pin_dr = _pin_dr;
    this->pin_mf = _pin_mf;

    this->begin();
}

void TcYKA2304ME::begin()
{
     // Set pin mode for motor
    pinMode(this->pin_pu, OUTPUT);
    pinMode(this->pin_dr, OUTPUT);
    pinMode(this->pin_mf, OUTPUT);
    // Set pin mode for encoder
    digitalWrite(this->pin_pu, HIGH);
    this->_position = 0;
    this->_previousPosition = 0;
    this->setDefaultLearning();

    // this->update();
}
void TcYKA2304ME::setSpeed(int _speed)
{
    this->_speed = _speed;
}

void TcYKA2304ME::setDefaultLearning()
{
    this->_direction = false; // backward to start position
    this->isLearning = true;
}
bool TcYKA2304ME::learning()
{
    if(!this->isLearning){
        return false;
    }
    unsigned long currentMicros = micros();
    // if(currentMicros - this->_lastDebounceTime > this->_speedLearning)
    if(currentMicros - this->_lastDebounceTime > static_cast<unsigned long>(this->_speedLearning))
    {
        this->_lastDebounceTime = currentMicros;
        if(!this->_direction){
            // Backward to start position
            if(this->_state_start){
                this->_direction = true;
                this->isPulse = false;
                this->_position = 0;
                this->_previousPosition = 0;
                this->_lastDebounceTime = this->_lastDebounceTime*1000; // delay 1s    
            }
        }else{
             // Count position
            this->_position++;
            // Forward to end position and check end is position
            if(this->_state_end){
                // Finish learning
                this->isLearning = false;
                this->isPulse = false;
                // Set position
                this->minPosition = 0;
                this->maxPosition = this->_position;
                this->_previousPosition = this->_position;
                // Call event
                if(this->OnLearned != NULL){
                    this->OnLearned(this->_position, this->minPosition, this->maxPosition);
                }
                // Set motor to start position
                this->_position = this->minPosition;
                this->_lastDebounceTime = this->_lastDebounceTime*1000; // delay 1s
            }
        }
        // Pulse to move backward
        this->isPulse = !this->isPulse;
        digitalWrite(this->pin_pu, this->isPulse); // Pulse to move backward to start position
        digitalWrite(this->pin_dr, this->_direction);  // Direction of motor

    }
    return true;
}

void TcYKA2304ME::update()
{
    if(this->isLearning){
        this->learning();
        return;
    }

    unsigned long currentMicros = micros();
    if(currentMicros - this->_lastDebounceTime >static_cast<unsigned long>(this->_speed))
    {
        if(this->_position != this->_previousPosition)
        {
            // Check direction
            if(this->_position > this->_previousPosition){
                this->_direction = true; // forward
                this->_previousPosition++;
                  if(this->_state_end){
                if(this->_position == this->maxPosition){
                    // Call event
                    if(this->OnEnd != NULL){
                        this->OnEnd(this->_position, this->minPosition, this->maxPosition);
                    }
                }
                // Set position to end
                this->_position = this->maxPosition;
                this->_previousPosition = this->maxPosition;
             }

            }else{
                this->_direction = false; // backward
                this->_previousPosition--;
                // Check start and end position
                if(this->_state_start){
                    if(this->_position == this->minPosition){
                        // Call event
                        if(this->OnStart != NULL){
                            this->OnStart(this->_position, this->minPosition, this->maxPosition);
                        }
                    }
                    // Set position to start
                    this->_position = this->minPosition;
                    this->_previousPosition = this->minPosition;
                }
            }
            // Check position
            if(this->_position > this->maxPosition){
                this->_position = this->maxPosition;
            }else if(this->_position < this->minPosition){
                this->_position = this->minPosition;
            }
          
            // Set motor
            digitalWrite(this->pin_dr, this->_direction);  // Direction of motor
            isPulse = !isPulse;
            digitalWrite(this->pin_pu, isPulse); // Pulse to move motor
        }
        this->_lastDebounceTime = currentMicros;
    }
}
void TcYKA2304ME::setPosition(unsigned long position)
{
    if(position > this->maxPosition){
         this->_position = this->maxPosition;
    }else if(position < this->minPosition){
         this->_position = this->minPosition;
    }else{
        this->_position = position;
    }
}

void TcYKA2304ME::ForwardToEnd()
{
    this->_position = this->maxPosition;
}

void TcYKA2304ME::BackwardToStart()
{
    this->_position = this->minPosition;
}

int TcYKA2304ME::getSpeed()
{
    return this->_speed;
}

void TcYKA2304ME::setSpeedLearning(int speed)
{
    this->_speedLearning = speed;
}

int TcYKA2304ME::getSpeedLearning()
{
    return this->_speedLearning;
}

void TcYKA2304ME::setStateStart(bool state)
{
    this->_state_start = state;
}

void TcYKA2304ME::setStateEnd(bool state)
{
    this->_state_end = state;
}

void TcYKA2304ME::setOnLearned(void (*function)(unsigned long position, unsigned long minPosition, unsigned long maxPosition))
{
    this->OnLearned = function;
}
// Path: src/TcYKA2304ME.h
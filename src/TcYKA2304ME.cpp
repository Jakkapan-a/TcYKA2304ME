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
    // digitalWrite(this->pin_dr, HIGH);
    // digitalWrite(this->pin_mf, HIGH);

    if (this->_invertMF)
    {
        digitalWrite(this->pin_mf, LOW);
    }
    else
    {
        digitalWrite(this->pin_mf, HIGH);
    }
    this->_position = 0;
    this->_previousPosition = 0;
    this->setDefaultLearning();

    // this->update();
}
void TcYKA2304ME::setSpeed(unsigned int _speed)
{
    this->_speed = _speed;
}

void TcYKA2304ME::setDefaultLearning()
{
    this->_direction = false; // backward to start position
    this->isLearning = true;
    this->isLearn = true;
}

bool TcYKA2304ME::learning()
{
    if (!this->isLearning)
    {
        return false;
    }
    unsigned long currentMicros = micros();
    // this->pauseEndTime = 0;

    if (this->OnWorking != NULL)
    {
        this->OnWorking(true);
    }
    // this->setMF(true);
    if (this->isLearn)
    {
        this->setMF(true);
        // this->_lastDebounceTime = currentMicros + 1000000; // 1s
        this->setRelay(300);
        this->isLearn = false;
    }
    // if (currentMicros < pauseEndTime)
    // {
    //     return true; // still waiting
    // }

    if (currentMicros - this->_lastDebounceTime > static_cast<unsigned long>(this->_speedLearning))
    {
        this->_lastDebounceTime = currentMicros;
        if (!this->_direction)
        {
            // Backward to start position
            if (this->_state_start)
            {
                this->_direction = !this->_direction;
                this->isPulse = false;
                this->_position = 0;
                this->_previousPosition = 0;
                this->setRelay(300);
            }
        }
        else
        {
            // Count position
            this->_position++;
            // Forward to end position and check end is position
            if (this->_state_end)
            {
                // Finish learning
                this->isLearning = false;
                this->isPulse = false;
                // Set position
                this->minPosition = 0;
                this->maxPosition = this->_position;
                this->_previousPosition = this->_position;
                // Set motor to start position
                this->_position = 0;
                this->setPosition((unsigned long)this->_offsetPoint);

                // Call event
                if (this->OnLearned != NULL)
                {
                    this->OnLearned(this->_position, this->minPosition, this->maxPosition);
                }
            }
        }
        // Pulse to move backward
        this->isPulse = !this->isPulse;
        digitalWrite(this->pin_pu, this->isPulse); // Pulse to move backward to start position

        bool stateDir = this->_direction; // Direction of motor
        if (this->_invertDirection)
        {
            digitalWrite(this->pin_dr, !stateDir); // Direction of motor
        }
        else
        {
            digitalWrite(this->pin_dr, stateDir); // Direction of motor
        }
    }
    else if (currentMicros < 100)
    {
        this->_lastDebounceTime = currentMicros; // reset debounce time for overflow
    }
    return true;
}

void TcYKA2304ME::update()
{
    if (isRelay())
    {
        return;
    }
    if (this->isLearning)
    {
        this->learning();
        return;
    }
    static unsigned long _oldSpeed = 0;
    if (this->_position != this->_previousPosition)
    {
        unsigned long currentMicros = micros();
        unsigned long currentSpeed = this->getSpeedMicros();
        if (currentMicros - this->_lastDebounceTime > static_cast<unsigned long>(currentSpeed))
        {
            if (this->OnWorking != NULL)
            {
                this->OnWorking(true);
            }
            // this->setMF(true);
            // Check direction
            if (this->_position > this->_previousPosition)
            {                            // Forward
                this->_direction = true; // forward
                this->_previousPosition++;
                if (this->_state_end)
                {
                    if (this->_position == this->maxPosition)
                    {
                        // Call event
                        if (this->OnEnd != NULL)
                        {
                            this->OnEnd(this->_position, this->minPosition, this->maxPosition);
                        }
                    }
                    // Set position to end
                    this->_position = this->maxPosition;
                    this->_previousPosition = this->maxPosition;
                }
            }
            else
            {                             // Backward
                this->_direction = false; // backward
                this->_previousPosition--;
                // Check start and end position
                if (this->_state_start)
                {
                    if (this->_position == this->minPosition)
                    {
                        // Call event
                        if (this->OnStart != NULL)
                        {
                            this->OnStart(this->_position, this->minPosition, this->maxPosition);
                        }
                    }
                    // Set position to start
                    this->_position = this->minPosition;
                    this->_previousPosition = this->minPosition;
                }
            }

            // Check position
            if (this->_position > this->maxPosition)
            {
                this->_position = this->maxPosition;
            }
            else if (this->_position < this->minPosition)
            {
                this->_position = this->minPosition;
            }

            if (this->_position == this->_previousPosition && this->OnUpdate != NULL)
            {
                // Call event
                this->OnUpdate(this->_position, this->minPosition, this->maxPosition);
            }
            if (this->_position == this->_previousPosition && this->OnUpdated != NULL)
            {
                // Call event
                this->OnUpdated(this->_position, this->minPosition, this->maxPosition);
            }

            if (this->_position == this->_previousPosition)
            {
                // Count success update position
                this->countSleep = 10000; // 10s
                this->countSuccess = 40;  // 40ms
            }
            if (this->OnUpdatePosition != NULL)
            {
                this->OnUpdatePosition(this->_previousPosition);
            }

            if (this->_oldDirection != this->_direction && this->OnUpdateDirection != NULL)
            {
                // Call event
                this->OnUpdateDirection(this->_direction);
            }

            if (_oldSpeed != currentSpeed && this->OnUpdateSpeed != NULL)
            {
                this->OnUpdateSpeed(currentSpeed);
            }

            if (this->_previousPosition == this->minPosition && !this->_state_start && this->OnError != NULL)
            {
                this->OnError(ERROR_CODE_START, "Start position is not set");
            }

            if (this->_previousPosition == this->maxPosition && !this->_state_end && this->OnError != NULL)
            {
                this->OnError(ERROR_CODE_END, "End position is not set");
            }

            bool stateDir = this->_direction; // Direction of motor
            if (this->_invertDirection)
            {
                digitalWrite(this->pin_dr, !stateDir); // Direction of motor
            }
            else
            {
                digitalWrite(this->pin_dr, stateDir); // Direction of motor
            }
            isPulse = !isPulse;
            digitalWrite(this->pin_pu, isPulse); // Pulse to move motor
            this->_oldDirection = this->_direction;
            _oldSpeed = currentSpeed;
            this->_lastDebounceTime = currentMicros;
        }
        else if (currentMicros < 100)
        {
            this->_lastDebounceTime = currentMicros; // reset debounce time for overflow
        }
    }
    else
    {
        if (this->OnWorking != NULL)
        {
            this->OnWorking(false);
        }
    }
}

void TcYKA2304ME::setDefaultSpeed()
{
    if (this->_position > this->_previousPosition)
    {
        this->totalPulse = this->_position - this->_previousPosition;
    }
    else
    {
        this->totalPulse = this->_previousPosition - this->_position;
    }
    this->countRun = 0;
}

unsigned long TcYKA2304ME::getSpeedMicros()
{
    this->countRun++;
    if (this->countRun <= this->increase)
    {
        if (this->countRun > this->totalPulse - this->decrease)
        {
            return map(this->countRun, this->totalPulse - this->decrease, this->totalPulse, this->_speed, this->slowDownSpeed);
        }
        // increase
        return map(this->countRun, 0, this->increase, this->slowStartSpeed, this->_speed);
    }

    else if (this->countRun >= this->totalPulse - this->decrease)
    {
        // decrease
        return map(this->countRun, this->totalPulse - this->decrease, this->totalPulse, this->_speed, this->slowDownSpeed);
    }
    return this->_speed; // default speed (could be adjusted as needed)
}

void TcYKA2304ME::setPosition(unsigned long position)
{
    unsigned long input = map(position, 0, this->MOTOR_OFFSET, this->minPosition, this->maxPosition);
    if (input > this->maxPosition)
    {
        this->_position = this->maxPosition;
    }
    else if (input < this->minPosition)
    {
        this->_position = this->minPosition;
    }
    else
    {
        this->_position = input;
    }
    this->setDefaultSpeed();
    this->setMF(true);
    this->countSleep = 0;
    setRelay(400);
    this->_lastDebounceTime = micros() + 1000000; // 1s

    // Call event
    if (this->_position == this->_previousPosition)
    {
        countSuccess = 50; // 50ms
    }
}

void TcYKA2304ME::ForwardToEnd()
{
    this->_position = this->maxPosition;
    this->setDefaultSpeed();
}

void TcYKA2304ME::BackwardToStart()
{
    this->_position = this->minPosition;
    this->setDefaultSpeed();
}

int TcYKA2304ME::getSpeed()
{
    return this->_speed;
}

void TcYKA2304ME::setSpeedLearning(unsigned int speed)
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

void TcYKA2304ME::setOnUpdateSpeed(void (*function)(unsigned long speed))
{
    this->OnUpdateSpeed = function;
}

void TcYKA2304ME::setOnUpdateSpeedLearning(void (*function)(int speed))
{
    this->OnUpdateSpeedLearning = function;
}

void TcYKA2304ME::setOnUpdated(void (*function)(unsigned long position, unsigned long minPosition, unsigned long maxPosition))
{
    this->OnUpdated = function;
}

void TcYKA2304ME::setOnUpdate(void (*function)(unsigned long position, unsigned long minPosition, unsigned long maxPosition))
{
    this->OnUpdate = function;
}

void TcYKA2304ME::setOnStart(void (*function)(unsigned long position, unsigned long minPosition, unsigned long maxPosition))
{
    this->OnStart = function;
}

void TcYKA2304ME::setOnEnd(void (*function)(unsigned long position, unsigned long minPosition, unsigned long maxPosition))
{
    this->OnEnd = function;
}

void TcYKA2304ME::setOnUpdateLearning(void (*function)(unsigned long position, unsigned long minPosition, unsigned long maxPosition))
{
    this->OnUpdateLearning = function;
}

void TcYKA2304ME::setOnUpdateDirection(void (*function)(bool direction))
{
    this->OnUpdateDirection = function;
}

void TcYKA2304ME::setOnUpdatePosition(void (*function)(unsigned long position))
{
    this->OnUpdatePosition = function;
}

void TcYKA2304ME::setOnError(void (*function)(int code, String message))
{
    this->OnError = function;
}

void TcYKA2304ME::setInvertDirection(bool invertDirection)
{
    this->_invertDirection = invertDirection;
}

void TcYKA2304ME::setOnWorking(void (*function)(bool state))
{
    this->OnWorking = function;
}

void TcYKA2304ME::setInvertMF(bool invertMF)
{
    this->_invertMF = invertMF;
}

void TcYKA2304ME::setMF(bool _state)
{
    if (this->_invertMF)
    {
        digitalWrite(this->pin_mf, !_state);
    }
    else
    {
        digitalWrite(this->pin_mf, _state);
    }
}

void TcYKA2304ME::setSlowStart(unsigned int speed)
{
    this->slowStartSpeed = speed;
}

void TcYKA2304ME::setSlowEnd(unsigned int speed)
{
    this->slowDownSpeed = speed;
}

void TcYKA2304ME::setIncrease(unsigned int distance)
{
    this->increase = distance;
}

void TcYKA2304ME::setDecrease(unsigned int distance)
{
    this->decrease = distance;
}

void TcYKA2304ME::setOffsetPoint(unsigned int offsetPoint)
{
    this->_offsetPoint = offsetPoint;
}

void TcYKA2304ME::setToOffsetPoint()
{
    this->setPosition((unsigned long)this->_offsetPoint);
}

void TcYKA2304ME::setLearning(bool state)
{
    this->isLearning = state;
    this->isLearn = state;
}

bool TcYKA2304ME::isRelay()
{
    if (millis() - _lastDebounceTimeMillisCountDown > 1) // 1ms
    {
        if (countRelay > 0)
        {
            countRelay--;
            if (countRelay <= 0)
            {
                countRelay = 0;
                isRelayState = false;
            }
        }

        if (countSleep > 0)
        {
            countSleep--;
            if (countSleep <= 0)
            {
                countSleep = 0;
                this->setMF(false);
            }
        }

        if (countSuccess > 0)
        {
            countSuccess--;
            if (countSuccess <= 0)
            {
                countSuccess = 0;
                if (this->OnUpdated != NULL)
                {
                    // Call event
                    this->OnUpdated(this->_position, this->minPosition, this->maxPosition);
                }
            }
        }
        _lastDebounceTimeMillisCountDown = millis();
    }
    else if (millis() < 100)
    {
        _lastDebounceTimeMillisCountDown = millis();
    }
    return isRelayState;
}

void TcYKA2304ME::setRelay(int count)
{
    countRelay = count;
    isRelayState = true;
}
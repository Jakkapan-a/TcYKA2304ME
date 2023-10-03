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
    this->speedStep[0] = _speed;
}

void TcYKA2304ME::setDefaultLearning()
{
    this->_direction = false; // backward to start position
    this->isLearning = true;
}

bool TcYKA2304ME::learning()
{
    if (!this->isLearning)
    {
        return false;
    }
    unsigned long currentMicros = micros();

    static unsigned long pauseEndTime = 0;

    if (currentMicros < pauseEndTime)
    {
        return true; // still waiting
    }

    // if(currentMicros - this->_lastDebounceTime > this->_speedLearning)
    if (currentMicros - this->_lastDebounceTime > static_cast<unsigned long>(this->_speedLearning))
    {
        this->_lastDebounceTime = currentMicros;
        if (!this->_direction)
        {
            // Backward to start position
            if (this->_state_start)
            {
                this->_direction = true;
                this->isPulse = false;
                this->_position = 0;
                this->_previousPosition = 0;
                pauseEndTime = currentMicros + 2000000; // 2s
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
                this->_position = this->minPosition;
                pauseEndTime = currentMicros + 2000000; // 2s
                // Call event
                if (this->OnLearned != NULL)
                {
                    this->OnLearned(this->_position, this->minPosition, this->maxPosition);
                }
            }
        }
        // Pulse to move backward
        this->isPulse = !this->isPulse;
        digitalWrite(this->pin_pu, this->isPulse);    // Pulse to move backward to start position
        digitalWrite(this->pin_dr, this->_direction); // Direction of motor
    }
    return true;
}

void TcYKA2304ME::update()
{
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

            // Set motor
            digitalWrite(this->pin_dr, this->_direction); // Direction of motor
            isPulse = !isPulse;
            digitalWrite(this->pin_pu, isPulse); // Pulse to move motor
            this->_oldDirection = this->_direction;
            _oldSpeed = currentSpeed;
            this->_lastDebounceTime = currentMicros;
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
    if (this->totalPulse < this->increase && this->totalPulse < this->decrease)
    {
        if (this->countRun <= this->increase / 2)
        {
            int speed = map(this->countRun, 0, this->increase / 2, 1, 3);
            return this->speedStep[speed]; // increase
        }
        else if (this->countRun >= this->totalPulse - (this->decrease / 2))
        {
            int speed = map(this->countRun, this->totalPulse - this->decrease / 2, this->totalPulse, 3, 1);
            return this->speedStep[speed]; // decrease
        }
        return this->speedStep[2]; // slightly faster during learning
    }
    else if (this->countRun <= this->increase)
    {
        int speed = map(this->countRun, 0, this->increase, 3, 0);
        return this->speedStep[speed]; // increase
    }
    else if (this->countRun >= this->totalPulse - this->decrease)
    {
        int speed = map(this->countRun, this->totalPulse - this->decrease, this->totalPulse, 0, 3);
        return this->speedStep[speed]; // decrease
    }
    return this->speedStep[0]; // default speed (could be adjusted as needed)
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
    this->_lastDebounceTime = micros();

    // 
    if (this->_position == this->_previousPosition && this->OnUpdated != NULL)
    {
        // Call event
        this->OnUpdated(this->_position, this->minPosition, this->maxPosition);
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
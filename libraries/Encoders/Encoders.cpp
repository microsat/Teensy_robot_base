


#include <Encoders.h>
#include <FastGPIO.h>
#include <avr/interrupt.h>
#include <Arduino.h>

#define LEFT_XOR   IO_D1		// pin 6 int 1
#define LEFT_B     8	//
#define RIGHT_XOR  IO_D0	// pin 5 int 0
#define RIGHT_B    IO_D7	// pin 12 a9 pwm

static volatile bool lastLeftA;
static volatile bool lastLeftB;
static volatile bool lastRightA;
static volatile bool lastRightB;

static volatile bool errorLeft;
static volatile bool errorRight;

// These count variables are uint16_t instead of int16_t because
// signed integer overflow is undefined behavior in C++.
static volatile uint16_t countLeft;
static volatile uint16_t countRight;



static void leftISR()
{
    bool newLeftB = FastGPIO::Pin<LEFT_B>::isInputHigh();
    bool newLeftA = FastGPIO::Pin<LEFT_XOR>::isInputHigh() ^ newLeftB;

    countLeft += (newLeftA ^ lastLeftB) - (lastLeftA ^ newLeftB);

    if((lastLeftA ^ newLeftA) & (lastLeftB ^ newLeftB))
    {
        errorLeft = true;
    }

    lastLeftA = newLeftA;
    lastLeftB = newLeftB;
}

static void rightISR()
{
    bool newRightB = FastGPIO::Pin<RIGHT_B>::isInputHigh();
    bool newRightA = FastGPIO::Pin<RIGHT_XOR>::isInputHigh() ^ newRightB;

    countRight += (newRightA ^ lastRightB) - (lastRightA ^ newRightB);

    if((lastRightA ^ newRightA) & (lastRightB ^ newRightB))
    {
        errorLeft = true;
    }

    lastRightA = newRightA;
    lastRightB = newRightB;
}

void Encoders::init2()
{
    // Set the pins as pulled-up inputs.
    FastGPIO::Pin<LEFT_XOR>::setInputPulledUp();
    FastGPIO::Pin<LEFT_B>::setInputPulledUp();
    FastGPIO::Pin<RIGHT_XOR>::setInputPulledUp();
    FastGPIO::Pin<RIGHT_B>::setInputPulledUp();
    
    
    attachInterrupt(5, rightISR, CHANGE);
  	attachInterrupt(6, leftISR, CHANGE);


    // Initialize the variables.  It's good to do this after enabling the
    // interrupts in case the interrupts fired by accident as we were enabling
    // them.
    lastLeftB = FastGPIO::Pin<LEFT_B>::isInputHigh();
    lastLeftA = FastGPIO::Pin<LEFT_XOR>::isInputHigh() ^ lastLeftB;
    countLeft = 0;
    errorLeft = 0;

    lastRightB = FastGPIO::Pin<RIGHT_B>::isInputHigh();
    lastRightA = FastGPIO::Pin<RIGHT_XOR>::isInputHigh() ^ lastRightB;
    countRight = 0;
    errorRight = 0;
}

int16_t Encoders::getCountsLeft()
{
    init();

    cli();
    int16_t counts = countLeft;
    sei();
    return counts;
}

int16_t Encoders::getCountsRight()
{
    init();

    cli();
    int16_t counts = countRight;
    sei();
    return counts;
}

int16_t Encoders::getCountsAndResetLeft()
{
    init();

    cli();
    int16_t counts = countLeft;
    countLeft = 0;
    sei();
    return counts;
}

int16_t Encoders::getCountsAndResetRight()
{
    init();

    cli();
    int16_t counts = countRight;
    countRight = 0;
    sei();
    return counts;
}

bool Encoders::checkErrorLeft()
{
    init();

    bool error = errorLeft;
    errorLeft = 0;
    return error;
}

bool Encoders::checkErrorRight()
{
    init();

    bool error = errorRight;
    errorRight = 0;
    return error;
}

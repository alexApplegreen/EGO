#include <modm/architecture/interface/clock.hpp>
#include <modm/board.hpp>
#include <modm/board/board.hpp>
#include <modm/debug/logger/logger.hpp>
#include <modm/io/iostream.hpp>
#include <modm/platform/core/delay_impl.hpp>

#include <stddef.h>
#include <chrono>

#define ARR_LEN 5 // iterations to intergate when measuring
#define BOX_WIDTH 150 // Raiser Dimension in centimeters

float measurements[ARR_LEN];
size_t counter = 0;
bool offTimerActivated = false;

float calculateDistance(float time)
{
    float result = (time * 0.0343) / 2; // distance in m
    return result;
}

float avg(float* samples)
{
    unsigned long sum = 0;
    for (int i = 0; i < ARR_LEN; i++)
    {
        sum += samples[i];
    }
    return sum / float(ARR_LEN);
}

float pulseIn()
{
    modm::Clock::time_point start = modm::Clock::now();
    modm::Clock::time_point end;

    while(true)
    {
        bool res = A1::read();
        if (!res)
        {
            // falling edge
            end = modm::Clock::now();
            break;
        }
    }
    float result = (end - start).count();
    return result;
}

/**
 * Measures the length of the TTL pulse from HC-SR04 Echo Pin
 */
float measureDistance()
{
    // TODO are these actually being set?
    A0::set(false);
    modm::delay(2us);
    A0::set(true);
    modm::delay(10us);
    A0::set(false);

    float duration = pulseIn();
    return calculateDistance(duration);
}

void setup()
{
    LedD13::setOutput();

    // ------------------------------------------------------------------------
    // HC SR04
    // ------------------------------------------------------------------------
    A0::setOutput();
    A1::setInput();

    // ------------------------------------------------------------------------
    // MOSFETS
    // ------------------------------------------------------------------------
    A3::setOutput(); // RED
    A4::setOutput(); // GREEN
    A5::setOutput(); // BLUE

    A3::set(false);
    A4::set(false);
    A5::set(false);
}

void turnOnLed()
{
    LedD13::set(true);
    A3::set(true);
}

void turnOffLed()
{
    LedD13::set(false);
    A3::set(false);
    A4::set(false);
    A5::set(false);
}

int main()
{
    Board::initialize();
    setup();

    float distanceAvg;

    while (true)
    {
        measurements[counter] = measureDistance();
        counter++;

        if (counter == ARR_LEN)
        {
            distanceAvg = avg(measurements);

            MODM_LOG_INFO << "Distance: " << distanceAvg << " cm" << modm::endl;

            if (distanceAvg < BOX_WIDTH)
            {
                turnOnLed();
            }
            else
            {
                turnOffLed();
            }
            counter = 0;
        }

        modm::delay(61us);
    }
}

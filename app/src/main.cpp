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
    return (time * 0.0343) / 2; // distance in m
}

float avg(float* samples)
{
    unsigned long sum = 0;
    for (int i = 0; i < 5; i++)
    {
        sum += samples[i];
    }
    return sum / 5;
}

/**
 * Measures the length of the TTL pulse from HC-SR04 Echo Pin
 */
float measureDistance()
{
    A0::set(0);
    modm::delay(2us);
    A0::set(1);
    modm::delay(10us);
    A0::set(0);

    // TODO implement PulseIn function
    // float duration = pulseIn(PIN_A1, HIGH);
    float duration = 0;
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

    A3::set(0);
    A4::set(0);
    A5::set(0);
}

void turnOnLed()
{
    LedD13::set(1);
    A3::set(1);
}

void turnOffLed()
{
    LedD13::set(0);
    A3::set(0);
    A4::set(0);
    A5::set(0);
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
            // Arduino
            distanceAvg = avg(measurements);

            // modm
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

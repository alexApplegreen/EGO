#include <modm/board.hpp>

#include <stddef.h>
#include <chrono>

#define ARR_LEN 5
#define BOX_WIDTH 150 // Raiser Dimension in centimeters

using namespace std::chrono_literals;

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
    digitalWrite(PIN_A0, LOW);
    modm::delay(2us);
    // delayMicroseconds(2);
    digitalWrite(PIN_A0, HIGH);
    modm::delay(10us);
    // delayMicroseconds(10);
    digitalWrite(PIN_A0, LOW);

    float duration = pulseIn(PIN_A1, HIGH);
    return calculateDistance(duration);
}

void blink(uint64_t delayMs)
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(delayMs);
    digitalWrite(LED_BUILTIN, LOW);
    delay(delayMs);
}

void setup()
{
    // ------------------------------------------------------------------------
    // HC SR04
    // ------------------------------------------------------------------------
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN_A0, OUTPUT);
    pinMode(PIN_A1, INPUT);

    // ------------------------------------------------------------------------
    // MOSFETS
    // ------------------------------------------------------------------------
    pinMode(PIN_A3, OUTPUT); // RED
    pinMode(PIN_A4, OUTPUT); // GREEN
    pinMode(PIN_A5, OUTPUT); // BLUE

    digitalWrite(PIN_A3, LOW);
    digitalWrite(PIN_A4, LOW);
    digitalWrite(PIN_A5, LOW);

    // Serial USB
    Serial.begin(9600);
}

void turnOnLed()
{
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(PIN_A3, HIGH);
    // digitalWrite(PIN_A4, HIGH);
    // digitalWrite(PIN_A5, HIGH);
}

void turnOffLed()
{
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(PIN_A3, LOW);
    digitalWrite(PIN_A4, LOW);
    digitalWrite(PIN_A5, LOW);
}

void loop()
{
    float distanceAvg;

    measurements[counter] = measureDistance();
    counter++;

    if (counter == ARR_LEN)
    {
        distanceAvg = avg(measurements);
        Serial.print("Distance: ");
        Serial.print(distanceAvg);
        Serial.println(" cm");

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

    delay(61);
}

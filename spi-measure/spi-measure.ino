#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#define MISO_PIN 12
#define MOSI_PIN 13
#define SCK_PIN  14
#define CS_PIN   15

#define ACCELEROMETER_SENSOR_ID 1234

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN, ACCELEROMETER_SENSOR_ID);

void setup() {
    Serial.begin(230400);

    pinMode(CS_PIN, OUTPUT);

    Serial.print("*** Connecting accelerometer: ");
    while(! accel.begin()) {
        Serial.print(".");
        digitalWrite(CS_PIN,! digitalRead(CS_PIN));
        delay(10);
        digitalWrite(CS_PIN, ! digitalRead(CS_PIN));
        delay(40);
    }
    Serial.println("connected!");

    // Set range and rate.
    //
    accel.setRange(ADXL345_RANGE_16_G);
    accel.setDataRate(ADXL345_DATARATE_800_HZ);
}

void loop() {
    static sensors_event_t event;

    accel.getEvent(&event);

    Serial.print(event.acceleration.x);
    Serial.print(" ");
    Serial.print(event.acceleration.y);
    Serial.print(" ");    
    Serial.println(event.acceleration.z);
}


#include <Wire.h>
#include <ADXL345.h>

// Configurables
//
#define LED_PIN    0
#define CS_PIN     2

#define SSID "ethera"
#define PASS "fooart1234123"

// Calibration for the sensor.  Used to get semi-sensible values.
//
struct Calibration {
    float xoff;
    float yoff;
    float zoff;

    float xgain;
    float ygain;
    float zgain;
};

// Measured calibration values for trap sensor.
//
static Calibration cal = {0.313813f, -0.666852f, 0.509946f, 10.552234f, 10.454167f, 10.022675f};

static ADXL345 acc;

// Apply calibration in-place
//
void apply_calibration(Calibration *c, Vector *v) {
    v->XAxis = (v->XAxis - c->xoff) / c->xgain;
    v->YAxis = (v->YAxis - c->yoff) / c->ygain;
    v->ZAxis = (v->ZAxis - c->zoff) / c->zgain;
}

void setup(){
    Serial.begin(250000);
    
    // Enables use of printf() on the ESP8266
    Serial.setDebugOutput(true);


    // Set the chip select high
    pinMode(CS_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH);

    // Make the LED pin output
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    while (! acc.begin()) {
        Serial.println("ADXL345 not ready...");

        // I have no idea if blipping the CS pin actually would get it
        // out of a hung state, but hey.
        digitalWrite(CS_PIN, LOW); // 
        delay(20);
        digitalWrite(CS_PIN, HIGH);
        delay(50);
    }

    acc.setRange(ADXL345_RANGE_16G);
    acc.setDataRate(ADXL345_DATARATE_800HZ);
}

void loop() {
    Vector v;

    unsigned long start = millis();
    for (int i = 0; i < 1000; i++) {
        v = acc.readNormalize();

        if (0x0002 & i) {
            yield();
        }
    }
    unsigned long end = millis() - start;

    Serial.print("Millis per thousand samples");
    Serial.println(end);
    
    apply_calibration(&cal, &v);

    Serial.print(v.XAxis, 2);
    Serial.print(" ");
    Serial.print(v.YAxis, 2);
    Serial.print(" ");
    Serial.print(v.ZAxis, 2);
    Serial.println();

}

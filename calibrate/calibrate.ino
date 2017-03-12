#include <Wire.h>
#include <ADXL345.h>

#include<stdlib.h>

#define LED_PIN    0
#define CS_PIN    16
#define INTR1_PIN 12
#define INTR2_PIN 13

struct Limits {
    float xmin;
    float xmax;
    float ymin;
    float ymax;
    float zmin;
    float zmax;
};

struct Calibration {
    float xoff;
    float yoff;
    float zoff;

    float xgain;
    float ygain;
    float zgain;
};

static Calibration calibration;
static ADXL345 acc;

void setup() {
    Serial.begin(115200);
    
    // Enables use of printf() on the ESP8266
    Serial.setDebugOutput(true);


    // Set the chip select high
    pinMode(CS_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH);

    // Make the LED pin output
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Make interrupt pins inputs
    pinMode(INTR1_PIN, INPUT);
    pinMode(INTR2_PIN, INPUT);XF

    while (! acc.begin()) {
        Serial.println("ADXL345 not ready...");
        digitalWrite(CS_PIN, LOW);
        delay(20);
        digitalWrite(CS_PIN, HIGH);
        delay(250);
    }

    acc.setRange(ADXL345_RANGE_16G);
    acc.setDataRate(ADXL345_DATARATE_400HZ);

    calibrate_sensor(&calibration);
}

void toggle_led() {
    digitalWrite(LED_PIN, ! digitalRead(LED_PIN));
}

void calibrate_sensor(Calibration *cal) {
    Limits lim = {0.0,0.0,0.0,0.0,0.0, 0.0};
    int count = 0;

    Serial.println("\n\n*** Calibration mode ***");
    
    while (Serial.available() == 0) {
        Vector v = acc.readNormalize();
        
        lim.xmin = (v.XAxis < lim.xmin) ? v.XAxis : lim.xmin;
        lim.ymin = (v.YAxis < lim.ymin) ? v.YAxis : lim.ymin;
        lim.zmin = (v.ZAxis < lim.zmin) ? v.ZAxis : lim.zmin;
        
        lim.xmax = (v.XAxis > lim.xmax) ? v.XAxis : lim.xmax;
        lim.ymax = (v.YAxis > lim.ymax) ? v.YAxis : lim.ymax;
        lim.zmax = (v.ZAxis > lim.zmax) ? v.ZAxis : lim.zmax;
        
        if (++count == 1000) {
            count = 0;
            
            print_limits(&lim);
        }
    }
    
    // Calculate calibration offset
    cal->xoff = (lim.xmax + lim.xmin) / 2.0f;
    cal->yoff = (lim.ymax + lim.ymin) / 2.0f;
    cal->zoff = (lim.zmax + lim.zmin) / 2.0f;

    // Note that gain is for calibration performed against 1g
    cal->xgain = (lim.xmax - lim.ymin) / 2.0f;
    cal->ygain = (lim.ymax - lim.ymin) / 2.0f;
    cal->zgain = (lim.zmax - lim.zmin) / 2.0f;

    
    Serial.print("Calibration cal = {");
    Serial.print(cal->xoff, 6); Serial.print("f, ");
    Serial.print(cal->yoff, 6); Serial.print("f, ");
    Serial.print(cal->zoff, 6); Serial.print("f, ");

    Serial.print(cal->xgain, 6); Serial.print("f, ");
    Serial.print(cal->ygain, 6); Serial.print("f, ");
    Serial.print(cal->zgain, 6); Serial.println("f};");
    Serial.println();

    // Empty buffer
    while(Serial.available() > 0) {
        Serial.read();
    }
}

// Apply calibration in-place
//
void apply_calibration(Calibration *c, Vector *v) {
    v->XAxis = (v->XAxis - c->xoff) / c->xgain;
    v->YAxis = (v->YAxis - c->yoff) / c->ygain;
    v->ZAxis = (v->ZAxis - c->zoff) / c->zgain;
}

// Just print limits for help when we are calibrating.
//
void print_limits(Limits *limits) {

    Serial.println();    
    
    Serial.print("  xmin = "); Serial.println(limits->xmin, 5);
    Serial.print("  xmax = "); Serial.println(limits->xmax, 5);
    
    Serial.print("  ymin = "); Serial.println(limits->ymin, 5);
    Serial.print("  ymax = "); Serial.println(limits->ymax, 5);    

    Serial.print("  zmin = "); Serial.println(limits->zmin, 5);
    Serial.print("  zmax = "); Serial.println(limits->zmax, 5);

    Serial.println();
}

void loop() {
   
    Vector v = acc.readNormalize();
    apply_calibration(&calibration,&v);

    Serial.print(v.XAxis);
    Serial.print(" ");
    Serial.print(v.YAxis);
    Serial.print(" ");
    Serial.print(v.ZAxis);
    Serial.print(" ");
    Serial.print(digitalRead(INTR1_PIN));
    Serial.print(" ");                 
    Serial.print(digitalRead(INTR1_PIN));                 
    Serial.print("\n");

    toggle_led();
    delay(500);
}

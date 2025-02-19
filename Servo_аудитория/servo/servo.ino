#include <Servo.h>
#define SERVO 2
Servo servo;

void setup() {
    Serial.begin(9600);
    servo.attach(SERVO);
}

void loop() {
    int c = 1;

    // Часы:
    while (true) {
        if (c == 60)
            c = 1;

        servo.write(c*3);
        delay(1000);
        c++;
    }

}
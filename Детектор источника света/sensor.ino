#define PHOTO_SENSOR_PIN_RIGHT  A0
#define PHOTO_SENSOR_PIN_CENTER  A1
#define PHOTO_SENSOR_PIN_LEFT  A2
#define LED_PIN_LEFT 9
#define LED_PIN_RIGHT 8
#define UPDATE_INTERVAL 500
#define CENTER_CALIBRATION 2
#define RIGHT_CALIBRATION 1.2


long int work_start;

void setup() {
    Serial.begin(9600);
    pinMode(LED_PIN_LEFT, OUTPUT);
    digitalWrite(LED_PIN_LEFT, HIGH);
    digitalWrite(LED_PIN_RIGHT, HIGH);
    work_start = millis();
}

void loop() {
    
    if (millis() >= work_start + UPDATE_INTERVAL) {
        work_start = millis();
    }
    else {
        return;
    }

    int photo_val_right = analogRead(PHOTO_SENSOR_PIN_RIGHT);
    int photo_val_center = analogRead(PHOTO_SENSOR_PIN_CENTER);
    int photo_val_left = analogRead(PHOTO_SENSOR_PIN_LEFT);

    Serial.print("Photo val left = ");
    Serial.println(photo_val_left);
    Serial.println();

    Serial.print("Photo val center = ");
    Serial.println(photo_val_center);
    Serial.println();

    Serial.print("Photo val right = ");
    Serial.println(photo_val_right);
    Serial.println();
    

    if ((photo_val_center * CENTER_CALIBRATION) >= photo_val_right and (photo_val_center * CENTER_CALIBRATION) >= photo_val_left) {
        digitalWrite(LED_PIN_LEFT, LOW);
        digitalWrite(LED_PIN_RIGHT, LOW);
    }
    else if (photo_val_left >= photo_val_right * RIGHT_CALIBRATION) {
        digitalWrite(LED_PIN_LEFT, HIGH);
        digitalWrite(LED_PIN_RIGHT, LOW);
    }
    else if (photo_val_left <= (photo_val_right * RIGHT_CALIBRATION)) {
        digitalWrite(LED_PIN_RIGHT, HIGH);
        digitalWrite(LED_PIN_LEFT, LOW);
    }

}


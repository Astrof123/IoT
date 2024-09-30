
#define LED_PIN_GREEN 13
#define LED_PIN_RED 12
#define LED_PIN_YELLOW 11

#define SET_LIGHTS_AUTO 'l'
#define SET_LIGHTS_HAND 'h'

#define SET_LED_GREEN_ON 'g'
#define SET_LED_RED_ON 'r'


#define STATE_IDLE 0
#define STATE_LED_ON 1
#define STATE_LED_OFF 2
#define STATE_LED_BLINK 3

#define BLINK_INTERVAL 500
#define BLINK_DURATION 2000
#define YELLOW_DURATION 1000
#define LIGHTS_GREEN_DURATION 10000
#define LIGHTS_RED_DURATION 7000
#define AFK_DURATION 60000

bool is_now_lights_auto;
bool is_now_lights_hand;
int stage;
int state;

int current_led_pin;
int previous_led_pin;
int current_light_duration;

long int blink_start;
long int light_start;
long int afk_start = 0;

bool led_status = LOW;

void update_state() {
    if (Serial.available() > 0) {
        char cmd = Serial.read();
        
        if (cmd == SET_LIGHTS_AUTO) {
            digitalWrite(current_led_pin, LOW);
            is_now_lights_auto = true;
            is_now_lights_hand = false;
            stage = 1;
            current_led_pin = LED_PIN_GREEN;
            previous_led_pin = LED_PIN_RED;
        }
        else if (cmd == SET_LIGHTS_HAND) {
            digitalWrite(current_led_pin, LOW);
            is_now_lights_hand = true;
            is_now_lights_auto = false;
        }
        else if (cmd == SET_LED_GREEN_ON and is_now_lights_hand) {
            digitalWrite(current_led_pin, LOW);
            if (current_led_pin == LED_PIN_RED) {
                current_led_pin = LED_PIN_YELLOW;
                state = STATE_LED_ON;
                light_start = millis();
                previous_led_pin = LED_PIN_RED;
            }
            else {
                state = STATE_LED_ON;
                current_led_pin = LED_PIN_GREEN;
                afk_start = millis();
            }

        }
        else if (cmd == SET_LED_RED_ON and is_now_lights_hand) {
            digitalWrite(current_led_pin, LOW);
            if (current_led_pin == LED_PIN_GREEN) {
                current_led_pin = LED_PIN_YELLOW;
                state = STATE_LED_ON;
                light_start = millis();
                previous_led_pin = LED_PIN_GREEN;
            }
            else {
                state = STATE_LED_ON;
                current_led_pin = LED_PIN_RED;
                afk_start = millis();
            }

        }
        else {
            Serial.println("Unknown command");
        }
    }
}

void traffic_light_stages() {
  if (is_now_lights_auto == true) {
      if (stage == 1) {
          current_led_pin = previous_led_pin == LED_PIN_GREEN ? LED_PIN_RED : LED_PIN_GREEN;
          current_light_duration = current_led_pin == LED_PIN_GREEN ? LIGHTS_GREEN_DURATION : LIGHTS_RED_DURATION;
          state = STATE_LED_ON;
          light_start = millis();
          stage += 1;
      }
      else if (stage == 2 and millis() >= light_start + current_light_duration) {
        if (current_led_pin == LED_PIN_RED) {
            stage += 1;
        }
        else {
            state = STATE_LED_BLINK;
            stage += 1;
            blink_start = millis();
        }
      }
      else if (stage == 3 and millis() >= light_start + current_light_duration + BLINK_DURATION) {
          state = STATE_LED_OFF;
          stage += 1;
      }
      else if (stage == 4) {
          light_start = millis();
          current_led_pin = LED_PIN_YELLOW;
          state = STATE_LED_ON;
          stage += 1;
      }
      else if (stage == 5 and millis() >= light_start + YELLOW_DURATION) {
          light_start = millis();
          state = STATE_LED_OFF;
          previous_led_pin = previous_led_pin == LED_PIN_GREEN ? LED_PIN_RED : LED_PIN_GREEN;
          stage = 1;
      }
  }
}

void process_state() {
    if (is_now_lights_hand and millis() >= afk_start + AFK_DURATION) {
        digitalWrite(current_led_pin, LOW);
        is_now_lights_auto = true;
        is_now_lights_hand = false;
        stage = 1;
        current_led_pin = LED_PIN_GREEN;
        previous_led_pin = LED_PIN_RED;
    }
    if (state == STATE_LED_ON) {
        if (is_now_lights_hand and current_led_pin == LED_PIN_YELLOW) {
            if (millis() >= light_start + YELLOW_DURATION * 3) {
                digitalWrite(current_led_pin, LOW);
                current_led_pin = previous_led_pin == LED_PIN_GREEN ? LED_PIN_RED : LED_PIN_GREEN;
                state = STATE_LED_ON;
                afk_start = millis();
            }
        }
        digitalWrite(current_led_pin, HIGH);
    }
    else if (state == STATE_LED_OFF) {
        digitalWrite(current_led_pin, LOW);
    }
    else if (state == STATE_LED_BLINK) {
        if (millis() <= blink_start + BLINK_INTERVAL and led_status == LOW) {
            digitalWrite(current_led_pin, HIGH);
            led_status = HIGH;
        }
        if (millis() >= blink_start + BLINK_INTERVAL and millis() <= blink_start + 2 * BLINK_INTERVAL and led_status == HIGH) {
            digitalWrite(current_led_pin, LOW);
            led_status = LOW;
        }
        if (millis() >= blink_start + 2 * BLINK_INTERVAL) {
            blink_start = millis();
        }
    }
}


void setup() {
    Serial.begin(9600);

    pinMode(LED_PIN_GREEN, OUTPUT);
    pinMode(LED_PIN_RED, OUTPUT);
    pinMode(LED_PIN_YELLOW, OUTPUT);
}

void loop() {
    update_state();
    traffic_light_stages();
    process_state();
}
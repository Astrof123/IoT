#define BUTTON_PIN 2 
#define BOUNCE_TIME 50
#define PRESSED HIGH
volatile long int press_time = 0;
volatile bool pressed_candidate = false;
volatile int press_count = 0;
volatile bool current_state = false;
volatile bool is_button_pressed = false;

void setup() {
    pinMode(BUTTON_PIN, INPUT);
    Serial.begin(9600);
    attachInterrupt(0, rising_button_click, RISING);
    attachInterrupt(1, falling_button_click, FALLING);
}
void loop() {
  long int buffer = millis();
  if (is_button_pressed == false && pressed_candidate && (buffer - press_time - BOUNCE_TIME) > 0) {
    if (current_state) {
        press_count += 1;
        is_button_pressed = true; 
    }
    else {
        press_time = 0;
        is_button_pressed = false;
        pressed_candidate = false;
    }
  }
}
void rising_button_click() {
  if (pressed_candidate == false) {
  	  pressed_candidate = true;
      press_time = millis();
  }
  current_state = true;
}

void falling_button_click() {
  current_state = false;
  if (is_button_pressed) {
      is_button_pressed = false;
      pressed_candidate = false;
    
      Serial.print("Press count ");
      Serial.println(press_count);
      Serial.print("Hold time ");
      Serial.println(millis() - press_time);

      press_time = 0;
  }
    
}
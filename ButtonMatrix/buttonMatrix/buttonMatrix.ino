#define ROW1 2
#define ROW2 3
#define ROW3 4

#define COL1 5
#define COL2 6
#define COL3 7

#define NROWS 3
#define NCOLS 3

const byte row_pins[NROWS] = {ROW1, ROW2, ROW3};
const byte col_pins[NCOLS] = {COL1, COL2, COL3};

bool button_states[NCOLS * NROWS] = {false};

unsigned long button_press_start_time[NCOLS * NROWS] = {0};
unsigned long button_press_duration[NCOLS * NROWS] = {0};

bool previous_button_states[NCOLS * NROWS] = {false};

const int TIMER_INTERVAL_MS = 5;
volatile int current_row = 0;


void setup() {
  for (int i = 0; i < NROWS; i++) {
    pinMode(row_pins[i], OUTPUT);
  }
  for (int i = 0; i < NCOLS; i++) {
    pinMode(col_pins[i], INPUT_PULLUP);
  }

  Serial.begin(9600);

  cli();
  TCCR1A = 0; 
  TCCR1B = 0;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS11) | (1 << CS10);
  OCR1A = 1249;
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

void loop() {

}

ISR(TIMER1_COMPA_vect) {
  scan_row(current_row);

  current_row++;
  if (current_row >= NROWS) {
    current_row = 0;
  }
}

void scan_row(int row_number) {
  PORTD &= ~(1 << row_pins[row_number]);
  for (int icol = 0; icol < NCOLS; icol++) {
    int button_index = row_number * NCOLS + icol;
    bool current_state = !(PIND & (1 << col_pins[icol]));

    button_states[button_index] = current_state;

    if (current_state != previous_button_states[button_index]) {
      if (current_state) {
        button_press_start_time[button_index] = millis();
        print_pressed_buttons();
      } else {
        button_press_duration[button_index] = millis() - button_press_start_time[button_index];
        print_pressed_buttons();
        Serial.print("Button press duration ");
        Serial.print(button_index + 1);
        Serial.print(" - ");
        Serial.print(button_press_duration[button_index]);
        Serial.print(" ms, start at ");
        Serial.print(button_press_start_time[button_index]);
        Serial.println(" ms");
      }

      previous_button_states[button_index] = current_state;
    }
  }

  PORTD |= (1 << row_pins[row_number]);
}

void print_pressed_buttons() {
  bool any_pressed = false;
  Serial.print("Buttons pressed: ");
  for (int i = 0; i < NROWS * NCOLS; i++) {
    if (button_states[i]) {
      Serial.print(i + 1);
      Serial.print(", ");
      any_pressed = true;
    }
  }
  if (any_pressed) {
    Serial.println();
  } else {
    Serial.println("no");
  }
}
const int fan_control_pin1 = 9; //Blue Wire on Fan (about 25kHz PWM)
const int fan_control_pin2 = 10;
int count1 = 0;
int count2 = 0;
long previousMillis = 0;
int rpm1;
int rpm2;
int speed = 100;

void setup() {
  TCCR1A = 0; // undo the configuration done by...
  TCCR1B = 0; // ...the Arduino core library
  TCNT1 = 0; // reset timer
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11); //Undo default timers
  TCCR1B = _BV(WGM13) | _BV(CS10); //for pins 9 & 10
  ICR1 = 320; // PWM will be 0 to 320 instead of 0 to 255
  pinMode(fan_control_pin1, OUTPUT);
  pinMode(fan_control_pin2, OUTPUT);
  OCR1A = 0;  //set pwm to 0 out of 320 on pin 9
  OCR1B = 0;  //set pwm to 0 out of 320 on pin 10
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), counter1, RISING);
  attachInterrupt(digitalPinToInterrupt(3), counter2, RISING);
}
void loop() {
  OCR1A = speed;
  OCR1B = speed;
  //  Serial.print(count1);
  //  Serial.print(" ");
  //  Serial.println(count2);

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > 1000) {
    previousMillis = currentMillis;
    rpm1 = count1 * 30;
    rpm2 = count2 * 30;
    Serial.print(rpm1);
    Serial.print(" ");
    Serial.println(rpm2);
    count1 = 0;
    count2 = 0;
  }


}
void counter1() {
  count1++;
}
void counter2() {
  count2++;
}

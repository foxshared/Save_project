unsigned int count1 = 0;
unsigned int count2 = 0;

int speed;
int level;
long previousMillis = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  // attachInterrupt(digitalPinToInterrupt(2), counter1, RISING);
  // attachInterrupt(digitalPinToInterrupt(3), counter2, RISING);

  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(9, speed);
  analogWrite(10, speed);
  counter1;
  counter2;

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > 5000) {
    previousMillis = currentMillis;
    speed = speed + 10;
    level = level + 1;
    if (speed > 255) {
      speed = 0;
      level = 0;
    }
  }
  Serial.print(0);  // To freeze the lower limit
  Serial.print(" ");
  Serial.print(25);  // To freeze the upper limit
  Serial.print(" ");

  Serial.print(level);
  Serial.print(" ");
  Serial.print(digitalRead(2));
  Serial.print(" ");
  Serial.println(digitalRead(3));
}
void counter1() {
  count1++;
}
void counter2() {
  count2++;
}
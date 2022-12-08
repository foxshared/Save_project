//Problem - reading two fan speed is inaccurate due to some noise
//Idea to fix - Reading the fan speed one by one by full speed one of the fan that not need to read speed every 20 second (use switching method) and 19 second reading

// ARGB control
#include <FastLED.h>

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN 11
//#define CLK_PIN   4
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 20
CRGB leds[NUM_LEDS];

#define BRIGHTNESS 100
#define FRAMES_PER_SECOND 120

//Fan control def
const byte OC1A_PIN = 9;
const byte OC1B_PIN = 10;
const word PWM_FREQ_HZ = 25000;  //Adjust this value to adjust the frequency (Frequency in HZ!) (Set currently to 25kHZ)
const word TCNT1_TOP = 16000000 / (2 * PWM_FREQ_HZ);

//Fan read RPM
const int fan_in1 = 2;
const int fan_in2 = 3;
unsigned long count1 = 0;
unsigned long count2 = 0;
unsigned long previousMillis = 0;
unsigned long rpm1;
unsigned long rpm2;

int start_update = 20;
int update_read = 19;  // trigger every 10 second
int speed = 0;
int trigger_read;
int step;
int first_read = 4;
int max_time = start_update + update_read;

unsigned long filter_rpm1;
unsigned long filter_rpm2;

unsigned long result_rpm1;
unsigned long result_rpm2;

void setup() {
  //ARG Setup
  delay(3000);  // 3 second delay for recovery
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  //Intial setup fan control
  pinMode(OC1A_PIN, OUTPUT);
  pinMode(OC1B_PIN, OUTPUT);

  // Clear Timer1 control and count registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // Set Timer1 configuration
  // COM1A(1:0) = 0b10   (Output A clear rising/set falling)
  // COM1B(1:0) = 0b00   (Output B normal operation)
  // WGM(13:10) = 0b1010 (Phase correct PWM)
  // ICNC1      = 0b0    (Input capture noise canceler disabled)
  // ICES1      = 0b0    (Input capture edge select disabled)
  // CS(12:10)  = 0b001  (Input clock select = clock/1)

  TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
  TCCR1B |= (1 << WGM13) | (1 << CS10);

  ICR1 = TCNT1_TOP;

  OCR1A = 0;
  OCR1B = 0;

  //Setup for tach sensor
  pinMode(fan_in1, INPUT_PULLUP);
  pinMode(fan_in2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(fan_in1), counter1, RISING);
  attachInterrupt(digitalPinToInterrupt(fan_in2), counter2, RISING);

  Serial.begin(9600);
}

uint8_t gHue = 0;  // rotating "base color" used by many of the patterns

void loop() {
  // delay(10);
  //ARG loop
  // Call the current pattern function once, updating the 'leds' array
  rainbow();
  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);
  // do some periodic updates
  EVERY_N_MILLISECONDS(20) {
    gHue++;
  }  // slowly cycle the "base color" through the rainbow

  //loop fan control
  speed = 15;



  //loop read speed
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > 1000) {
    previousMillis = currentMillis;
    rpm1 = count1 * 30;
    rpm2 = count2 * 30;
    count1 = 0;
    count2 = 0;
    trigger_read++;
    if (trigger_read > max_time) {
      trigger_read = 0;
    }
  }

  if (trigger_read >= start_update && trigger_read < start_update + first_read) {
    setPwmDuty(speed, 0);
    step = 1;
  } else if (trigger_read >= start_update + first_read && trigger_read <= max_time) {
    setPwmDuty(speed, 1);
    step = 2;
  } else if (trigger_read < start_update) {
    setPwmDuty(speed, 2);
    step = 3;
  }

  if (step == 1) {
    filter_rpm2 = rpm2;
  } else if (step == 2) {
    filter_rpm1 = rpm1;
  } else if (step == 3) {
    result_rpm1 = filter_rpm1;
    result_rpm2 = filter_rpm2;
  }

  //Serial output
  Serial.print(0);
  Serial.print(" ");
  Serial.print(2500);
  Serial.print(" ");
  Serial.print(step);
  Serial.print(" ");
  Serial.print(trigger_read);
  Serial.print(" ");
  // Serial.print(count1);
  // Serial.print(" ");
  // Serial.print(count2);
  // Serial.print(" ");
  Serial.print(rpm1);
  Serial.print(" ");
  Serial.print(rpm2);
  Serial.print(" ");
  Serial.print(filter_rpm1);
  Serial.print(" ");
  Serial.print(filter_rpm2);
  Serial.print(" ");
  Serial.print(result_rpm1);
  Serial.print(" ");
  Serial.println(result_rpm2);
}
void rainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, NUM_LEDS, gHue, 7);
}
void counter1() {
  ++count1;
}
void counter2() {
  ++count2;
}

void setPwmDuty(byte duty, int flag) {
  if (flag == 0) {  //
    OCR1A = (word)(duty * TCNT1_TOP) / 100;
    // OCR1B = 0; //if want to stop one fan
    OCR1B = (word)(5 * TCNT1_TOP) / 100;
  } else if (flag == 1) {
    OCR1A = (word)(5 * TCNT1_TOP) / 100;
    // OCR1A = 0; //if want to stop one fan
    OCR1B = (word)(duty * TCNT1_TOP) / 100;
  } else if (flag == 2) {
    OCR1A = (word)(duty * TCNT1_TOP) / 100;
    OCR1B = (word)(duty * TCNT1_TOP) / 100;
  }
}
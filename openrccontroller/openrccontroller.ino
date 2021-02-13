// https://forum.arduino.cc/index.php?topic=421081

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
// PWM Lirary for PCA9685
#include <Adafruit_PWMServoDriver.h>


#define CE_PIN   9
#define CSN_PIN  8
#define RADIO_CHANNEL 108

#define SERVO_FREQ 50
#define SERVOMIN  270 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  380 // This is the 'maximum' pulse length count (out of 4096)

// ESC
#define ESC_LOW_DEFAULT 1150
#define ESC_HIGH_DEFAULT 1330

// Radio Related Variables
const byte thisSlaveAddress[5] = {'R','x','A','A','A'};
RF24 radio(CE_PIN, CSN_PIN);
char dataReceived[10]; // this must match dataToSend in the TX
int joyData[3] = {0,0,0};
bool newData = false;

//Servo related variables
uint8_t servonum = 0;
uint8_t ecspin = 4;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Starting Radio");
  radio.begin();
  radio.setChannel(RADIO_CHANNEL);
  radio.setRetries(3,5); // delay, count
  Serial.println((String) "Channel " + RADIO_CHANNEL);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, thisSlaveAddress);
  radio.startListening();
  Serial.println("Starting Radio [OK]");

  Serial.println("Initialising PWM");
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  delay(10);
  uint16_t servoCenter =  SERVOMIN + (SERVOMAX - SERVOMIN) / 2;
  Serial.println((String)"Servo Center: " + servoCenter); 
  pwm.setPWM(servonum, 0, servoCenter);
  delay(1000);
  Serial.println("Initialising PWM [OK]");
  Serial.println("Initialise ESC");
  pwm.setPWM(ecspin, 0, 0);
  delay(1000);
  pwm.setPWM(ecspin, 0, ESC_HIGH_DEFAULT);
  Serial.println((String)"Initialise ESC High [OK]");
  delay(1000);
  pwm.setPWM(ecspin, 0, ESC_LOW_DEFAULT);
  Serial.println((String)"Initialise ESC Low [OK]");
  Serial.println("Initialise ESC [OK]");
}

void loop() {
  // put your main code here, to run repeatedly:
//  Serial.println(servonum);
//  for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) {
//    pwm.setPWM(servonum, 0, pulselen);
//    Serial.println(pulselen);
//    delay(10);
//  }
//  delay(500);
//  for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) {
//    pwm.setPWM(servonum, 0, pulselen);
//    Serial.println(pulselen);
//    delay(10);
//  }
//  delay(500);

  getData();
  showData();

  pwm.setPWM(ecspin, 0, 1150);
  Serial.println(1150);
  delay(2000);
  pwm.setPWM(ecspin, 0, 1140);
  delay(2000);
  
}


void getData() {
    if ( radio.available() ) {
        radio.read( &joyData, sizeof(joyData));
        newData = true;
    }
}

void showData() {
    if (newData == true) {
        Serial.print("Data received ");
        //Serial.println(dataReceived);
        Serial.print(joyData[0]);
        Serial.print(",");
        Serial.print(joyData[1]);
        Serial.print(",");
        Serial.print(joyData[2]);
        Serial.println("");
        newData = false;
    }
}

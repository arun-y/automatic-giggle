

// SimpleTx - the master or the transmitter

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


#define CE_PIN   9
#define CSN_PIN  8

const byte slaveAddress[5] = {'R','x','A','A','A'};


RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

char dataToSend[10] = "Message 0";
int joyData[3] = {0,0,0};
char txNum = '0';


unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 100; // send once per second

int VRx = A5;
int VRy = A6;
int SW = A7;

int xPosition = 0;
int yPosition = 0;
int SW_state = 0;
int mapX = 0;
int mapY = 0;

void setup() {
    Serial.begin(9600);
    Serial.println("SimpleTx Starting");

    radio.begin();
    radio.setDataRate(RF24_250KBPS);
    radio.setChannel(108);
    radio.setRetries(3,5); // delay, count
    radio.openWritingPipe(slaveAddress);

    // Joystick pins
    pinMode(VRx, INPUT);
    pinMode(VRy, INPUT);
    pinMode(SW, INPUT_PULLUP);
}

//====================

void loop() {
    currentMillis = millis();
    xPosition = analogRead(VRx);
    yPosition = analogRead(VRy);
    SW_state = digitalRead(SW);
    mapX = map(xPosition, 0, 1023, -512, 512);
    mapY = map(yPosition, 0, 1023, -512, 512);
    joyData[0] = SW_state;
    joyData[1] = mapX;
    joyData[2] = mapY;
    if (currentMillis - prevMillis >= txIntervalMillis) {
        send();
        prevMillis = millis();
    }
}

//====================

void send() {
    bool rslt;
    rslt = radio.write( &joyData, sizeof(joyData) );
        // Always use sizeof() as it gives the size as the number of bytes.
        // For example if dataToSend was an int sizeof() would correctly return 2

    Serial.print("Data Sent ");
    Serial.print(dataToSend);
    Serial.print("<=>");
    Serial.print(joyData[0]);
    Serial.print(",");
    Serial.print(joyData[1]);
    Serial.print(",");
    Serial.print(joyData[2]);
    if (rslt) {
        Serial.println("  Acknowledge received");
        updateMessage();
    }
    else {
        Serial.println("  Tx failed");
    }
}

//================

void updateMessage() {
        // so you can see that new data is being sent
    txNum += 1;
    if (txNum > '9') {
        txNum = '0';
    }
    dataToSend[8] = txNum;
}

#include <Arduino.h>

//#define SERIAL_DEBUG_DISABLED

#define USE_LIB_WEBSOCKET true
#define ESP32_CAN_TX_PIN GPIO_NUM_19
#define ESP32_CAN_RX_PIN GPIO_NUM_18

#define DATAEN_PIN GPIO_NUM_4
#define RF_RX  GPIO_NUM_16
#define RF_TX  GPIO_NUM_17


#include <NMEA2000_esp32.h>
#include <NMEA2000_CAN.h> 
#include <N2kMessages.h>


#include <jdy40.h>
#include <windsensor.h>
#include <windconfig.h>

#define BLUETOOTHCLASSIC 1
#ifdef BLUETOOTHCLASSIC
#include <BlueToothSerial.h>
BluetoothSerial SerialBT;
#define SerialIO SerialBT
#else
#define SerialIO Serial
#endif


const unsigned long TransmitMessages[] PROGMEM={130306L,0};



Jdy40 jdy40(DATAEN_PIN);
WindSensor windSensor;
WindConfig windConfig(&windSensor,&SerialIO);

#define INPUT_BUFFER_SIZE 1024
char inputBuffer[INPUT_BUFFER_SIZE];


void setup() {

  
#ifdef BLUETOOTHCLASSIC                               
  SerialBT.begin("WindSensorBase"); 
#else 
  Serial.begin(115200);
#endif

  SerialIO.println("Confgiure JDY40");

  Serial1.begin(9600,SERIAL_8N1, RF_RX, RF_TX);
  
  jdy40.setInputBuffer(inputBuffer, INPUT_BUFFER_SIZE);
  jdy40.setDebug(&SerialIO);
  jdy40.begin(&Serial1, 9600);
  jdy40.startConfig();
  jdy40.init();
  jdy40.setRFID(1021);
  jdy40.setDeviceID(1123);
  jdy40.setChannel(1);

  windConfig.begin();

  // Setup NMEA2000
  // Set Product information
  NMEA2000.SetProductInformation("00000002", // Manufacturer's Model serial code
                                 100, // Manufacturer's product code
                                 "Simple wind monitor",  // Manufacturer's Model ID
                                 "1.1.0.22 (2016-12-31)",  // Manufacturer's Software version code
                                 "1.1.0.0 (2016-12-31)" // Manufacturer's Model version
                                 );
  // Set device information
  NMEA2000.SetDeviceInformation(1, // Unique number. Use e.g. Serial number.
                                130, // Device function=Atmospheric. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                85, // Device class=External Environment. See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2046 // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf                               
                               );

  // debugging with no chips connected.
  NMEA2000.SetForwardStream(&SerialIO);
  NMEA2000.SetDebugMode(tNMEA2000::dm_ClearText);
  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); 
  NMEA2000.SetDebugMode(tNMEA2000::dm_ClearText); // Uncomment this, so you can test code without CAN bus chips on Arduino Mega

  // this is a node, we are not that interested in other traffic on the bus.
  NMEA2000.SetMode(tNMEA2000::N2km_NodeOnly,23);
  NMEA2000.EnableForward(false);
  NMEA2000.ExtendTransmitMessages(TransmitMessages);
  NMEA2000.Open();

  

  SerialIO.println("Starting");
  
}

// todo, configuration.

#define WindUpdatePeriod 1000

void SendN2kWind() {
  static unsigned long WindUpdated=millis();
  tN2kMsg N2kMsg;

  if ( WindUpdated+WindUpdatePeriod<millis() ) {
    SetN2kWindSpeed(N2kMsg, 1, windSensor.getWindSpeed(), windSensor.getWindAngle() ,N2kWind_Apprent);
    WindUpdated=millis();
    NMEA2000.SendMsg(N2kMsg);
  }
}



void loop() {
  // read the serial data and process.
  windConfig.process();
  windSensor.processData(jdy40.readLine());
  if ( windConfig.isOutputEnabled() ) {
    NMEA2000.SetForwardStream(&SerialIO);
  } else {
    NMEA2000.SetForwardStream(0);
  }
  // Send to N2K Bus
  SendN2kWind();
  NMEA2000.ParseMessages();
}
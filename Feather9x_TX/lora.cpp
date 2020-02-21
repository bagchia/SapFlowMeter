#include "lora.h"
// Feather9x_TX
// Designed to work with  Feather9x_RX

#include <RHReliableDatagram.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <ArduinoJson.h>
#include <ftoa.h> // for float to string conversion

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Class to manage message delivery and receipt, using the driver declared above
static RHReliableDatagram manager(rf95, CLIENT_ADDRESS);

void lora_init(void) 
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);


  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!manager.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

static int16_t packetnum = 0;  // packet counter, we increment per xmission

static char radiopacket[RH_RF95_MAX_MESSAGE_LEN];
static uint8_t packet_len; // This makes me nervous - how long IS the max message length?

void build_msg(float flow, float weight, float temp, char * time)
{
  const int capacity=JSON_OBJECT_SIZE(5);
  StaticJsonDocument<capacity>doc;
  char str1[15];
  char str2[15];
  char str3[15];
  ftoa(flow, str1);
  ftoa(weight, str2);
  ftoa(temp, str3);
  doc["flow"].set(str1);
  doc["weight"].set(str2);
  doc["temp"].set(str3);
  doc["time"].set(time);
  doc["id"].set("1");
  packet_len = serializeJson(doc,radiopacket);
  Serial.print("Sending "); Serial.println(radiopacket);
  radiopacket[packet_len] = 0;
}


void send_msg( void )
{
  Serial.println("Transmitting..."); // Send a message to rf95_server
  // Send a message to manager_server
  if (manager.sendtoWait((uint8_t *)radiopacket, packet_len, SERVER_ADDRESS))
  {
    // Now wait for a reply from the server
    uint8_t from;   
    if (manager.recvfromAckTimeout((uint8_t *)radiopacket, &packet_len, 2000, &from))
    {
      Serial.print("got reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println(radiopacket);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
    }
    else
    {
      Serial.println("No reply, is server running?");
    }
  }
  else
    Serial.println("sendtoWait failed");

}
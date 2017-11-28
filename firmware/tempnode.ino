/*
 * Author: Joar Rusten
 * Date: 2017-10-27
 *
 * This example is designed for the following hardware setup:
 *
 * - Arduino Uno
 * - Seed studion X-Bee shield, http://wiki.seeed.cc/XBee_Shield_V2.0/
 * - LoRabee RN2483 module, https://shop.sodaq.com/en/lorabee-rn2483.html
 *
 * Configure the X-Bee shield as follows:
 * 
 * XB_TX = 10
 * XB_RX = 11
 */

#include <rn2xx3.h>
#include <SoftwareSerial.h>
#include "secrets.h"

#define XB_RX   11
#define XB_TX   10

#define M_INFO  "        "
#define M_ERROR "ERROR : "

static SoftwareSerial lora_serial(XB_TX, XB_RX);
static rn2xx3 lora(lora_serial);

/** @brief Sleep and save power
 */
static void m_sleep(void) {
  // TODO: implement power down
  // 10 minutes between packets
  delay(600000);
}

/** @brief Configure serial port for serial terminal
 */
static void m_serial_init(void) {
  Serial.begin(9600);
}

/** @brief Print welcome message to serial terminal
 */
static void m_welcome(void){
  Serial.println("");
  Serial.print(M_INFO);
  Serial.println("Welcome to the Elsys RN2483 example");
  Serial.println("");
}

/** @brief Initiate LoRa module
 */
static void m_lora_init(void) {
  // Initiate LoRa serial interface    
  lora_serial.begin(9600);
  lora_serial.flush();
  lora.autobaud();
  
  // Check lora serial connection by reading Device EUI from LoRa module
  String device_eui = lora.hweui();
  
  if(device_eui.length() != 16) {
    Serial.print(M_ERROR);
    Serial.println("Unable to communication with LoRa module.");

    while(true){
      Serial.print(M_INFO);
      Serial.println("Fix hardware setup and power cycle board!");
      m_sleep();
    }
  }

  Serial.print(M_INFO);
  Serial.print("Device EUI = ");
  Serial.println(device_eui);
}

/** @brief Connect to TTN
 * 
 * This function takes care of connecting to TTN
 * 
 * @param p_app_eui    Pointer to application EUI
 * @param p_app_key    Pointer to application key
 */
static void m_lora_connect(const char * p_app_eui, const char * p_app_key) {
  bool joined = false;
    
  Serial.print(M_INFO);
  Serial.println("Sending OTAA join request to TTN");
  joined = lora.initOTAA(p_app_eui, p_app_key);

  // Force SF7
  lora.setDR(5);

  while(joined == false) {
    Serial.print(M_ERROR);
    Serial.println("Unable to join TTN using current EUIs");
    m_sleep();
    joined = lora.init();
  }
  
  Serial.print(M_INFO);
  Serial.println("Successfully joined TTN");
}

/** @brief Read temperature from temperature sensor
 */
static int m_read_temperature(void) {
  static int temp = 0;

  // TODO: read temperature from sensor
  temp++;   

  return temp;
}

/** @brief Send temperature to TTN
 * 
 * @param temp  Temperature to send
 */
static void m_send_temperature(int temp) {
  char ta[2];
  
  ta[0] = (char)(temp >> 8);
  ta[1] = (char)temp;
  
  lora.txBytes(&ta[0],2);
}

// Main functions

void setup(void) {
  m_serial_init();
  m_welcome();
  m_lora_init();
  m_lora_connect(appEui, appKey);
}

void loop(void){
  Serial.print(M_INFO);
  Serial.print("Temperature = ");

  // Read temperature sensor  
  int temp = m_read_temperature();
  // Send temperature to TTN
  m_send_temperature(temp);

  Serial.println(temp, DEC);

  m_sleep();
}
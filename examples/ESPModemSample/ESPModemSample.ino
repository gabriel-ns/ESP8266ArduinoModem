#include <SoftwareSerial.h>
#include <ESP8266Modem.h>

#define ESP8266_TX     2  // Pin used on SoftwareSerial TX
#define HARD_RESET_PIN 4  // Pin connected to the ESP8266 reset

/* Set the RX pin in a not used pin. The ideia here is to
   not use the software serial RX pin due to it's low reliability
*/
SoftwareSerial espSerial(13, ESP8266_TX);

/*
   When instantiating an ESPModem object we need to define
   the UART TX port, the UART RX port, and optionally the debug port.
   ESP8266Modem ESP(txUart, rxUart, debugUart);

   We are using a SoftwareSerial port to handle the TX
   and the HardwareSerial port to handle the RX.

   The hardware must handle the coexistence with the USB RX and ESP RX.
*/
ESP8266Modem ESP(&espSerial, &Serial, &Serial);

const char* ssid = "network_name";
const char* pw   = "network_password";

void setup() {
  /* Starting the UART in the ESP default speed */
  Serial.begin(115200);
  Serial.setTimeout(10); // Setting timeout for readString function
  espSerial.begin(115200);

  Serial.println("Starting firmware...");

  /* You can reset the ESP8266 on starting if you don't know its previous state */
  //ESP.swReset();
  //delay(1000);  //Time for ESP reboot

  //ESP.enableDebug();  //enable the debug if you want
  
  /* Wait until ESP is ready to operate */
  while (!ESP.isPresent())
  {
    delay(100);
  }

  ESP.connectWifi(ssid, pw);
}

void loop() {

  /** Connecting to the server */
  String response = ESP.connectToServer("TCP","api.pushingbox.com",80);
  Serial.println(response);

  /* Preparing the GET request */
  String req = "GET /pushingbox?devid=DEVICE_ID";         // define the page to access
  req += "&value=" + String(millis());                    // set the value to be sent
  req += " HTTP/1.1\r\nHost: api.pushingbox.com\r\n";     // sending HTTP data
  req += "Connection: close\r\n\r\n";                     // close connection after sending

  /* Sending the request to the server */
  response = ESP.httpGet(req);
  
  Serial.println(response);

  delay(10000);
}

#include <SoftwareSerial.h>
#include <ESP8266Modem.h>

#define ESP8266_TX	2
#define HARD_RESET_PIN	4
#define LM35_PIN	A0
#define LED_PIN	5

SoftwareSerial espSerial(13, ESP8266_TX);
ESP8266Modem ESP(&espSerial, &Serial, &Serial);

const char* ssid = "network_name";
const char* pw  = "network_password";
const char* script = "/macros/s/YOUR_SCRIPT_ID_HERE/exec?";

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(10);
  espSerial.begin(115200);

  pinMode(A0, INPUT);
  pinMode(LED_PIN, OUTPUT);

  Serial.println("Starting firmware");

  while (!ESP.isPresent())
  {
    delay(100);
  }
}

void loop()
{
  String response = ESP.connectToServer("TCP", "appscripts.herokuapp.com", 80);
  Serial.println(response);
  String req = "GET " + String(script);
  req += "&value=" + String(millis());
  req += " HTTP/1.1\r\nHost: appscripts.herokuapp.com\r\n";
  req += " Conection: close\r\n\r\n";

  response = ESP.httpGet(req);
  Serial.println(response);

  if((response.indexOf("ref:") != -1) && (response.indexOf("$$$") != -1))
  {
    String refString = response.substring(response.indexOf("ref:") + 4, response.indexOf("$$$"));
    int16_t ref = refString.toInt();

    if(ref != -1)
    {
    	int16_t temp = (int16_t) (analogRead(A0) * 500.0 / 1024);
    	if(temp > ref)
    	{
    		digitalWrite(LED_PIN, HIGH);
    	}
    	else
    	{
    		digitalWrite(LED_PIN, LOW);
    	}
    }
  }

  delay(15000);
}

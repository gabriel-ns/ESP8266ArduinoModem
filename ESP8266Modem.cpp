#include <stdint.h>
#include <WString.h>
#include "ESP8266Modem.h"

ESP8266Modem::ESP8266Modem(Stream *ptxSerial, Stream *prxSerial, Stream *pdebugSerial)
{
  this->_txSerial = ptxSerial;
  this->_rxSerial = prxSerial;
  this->_debugSerial = pdebugSerial;
}

bool ESP8266Modem::isPresent(uint16_t timeout, uint8_t retries)
{
	String response;
	do{
		tx_print(F("AT\r\n"));
		uint32_t currentTime = millis();
		while(millis() < (currentTime + timeout))
		{
			while(_rxSerial->available()) response += (char) _rxSerial->read();
			if(response.indexOf(F("OK")) != -1)
			{
				debug_print(response);
				return true;
			}
		}
		debug_print(response);
		response = "";
	}while(retries-- > 1);

	return false;
}

String ESP8266Modem::connectWifi(const char * ssid, const char * password, uint16_t timeout)
{
	String cmd = "AT+CWJAP=\"" + String(ssid) + "\",\"" + String(password) + "\"\r\n";
	debug_print(sendModemCommand("AT+CWMODE=3\r\n", 2500));
	return sendModemCommand(cmd.c_str(), timeout);

}

String ESP8266Modem::disconnectWifi()
{
	return sendModemCommand(F("AT+CWQAP\r\n"), 500);
}

String ESP8266Modem::showNetworkStatus()
{
	return sendModemCommand(F("AT+CIFSR\r\n"), 1000);
}

String ESP8266Modem::swReset()
{
	String rsp = sendModemCommand(F("AT+RST\r\n"), 1000);
	delay(500);
	return rsp;
}

String ESP8266Modem::connectToServer(String type, String server, uint16_t port)
{
	String cmd = "AT+CIPSTART=\"" + type + "\",\"" + server + "\"," + String(port) + "\r\n";
	return sendModemCommand(cmd, 10000);
}

String ESP8266Modem::httpGet(String data, uint16_t timeout_ms)
{
	debug_print("Send request");
	String response = sendModemCommand("AT+CIPSEND=" + String(data.length()) + "\r\n", 3000);
	if(response.indexOf("ERROR") != -1) return response;

	bool success = false;
	uint32_t currentTime = millis();

	while(millis() < (currentTime + 1000))
	{
		while(_rxSerial->available()) response += (char) _rxSerial->read();
		/* Quebra o loop se o OK for encontrado */
		if(response.indexOf('>') != -1)
		{
			debug_print("Sending authorized.");
			success = true;
			break;
		}
	}
	if(success) tx_print(data);
	else
	{
		debug_print("Authorization timed out");
		return response;
	}

	success = false;
	currentTime = millis();
	debug_print("Waiting for +IPD");
	uint16_t rcvdBytes = 0;
	uint16_t IPDSize = 0;

	while(millis() < (currentTime + timeout_ms))
	{
		while(_rxSerial->available()) response += (char) _rxSerial->read();

		if(response.indexOf("+IPD,") != -1 && response.indexOf(":") != -1)
		{
			debug_print("Got IPD. Reading...");

			String sizeString = response.substring(response.indexOf("+IPD,") + 5, response.indexOf(":"));
			
			IPDSize = sizeString.toInt();
			debug_print("Total response size: str(" + sizeString + ") | int(" + String(IPDSize) + ")");

			while(rcvdBytes < IPDSize) 
			{
				if(_rxSerial->available() > 0)
				{
					response += String((char) _rxSerial->read());
					rcvdBytes++;
				}

				if(millis() > (currentTime + timeout_ms))
				{
					debug_print("Receiving timed out...");
					break;
				}
			}

			success = true;
			break;
		}
	}

	if(!success) 
	{
		debug_print("Error, timed out without response");
		response += "\nTimed out.";
	}
	else
	{
		debug_print("Received " + String(rcvdBytes) + " from " + String(IPDSize));
	}

	
	debug_print("Done.\n");

	return response;
}

template <typename Output>
void ESP8266Modem::debug_print(Output text)
{
	if(_debugSerial != NULL && _isDebugEnabled)
	{
		_debugSerial->print(F(" * ESPModemDEBUG: "));
		_debugSerial->println(text);
	}
}

template <typename Output>
void ESP8266Modem::tx_print(Output text)
{
	if(_txSerial != NULL)
	{
		_txSerial->print(text);
	}
}
String ESP8266Modem::sendModemCommand(String cmd, uint16_t timeout)
{
	String response = "";
	debug_print("Sending: \n" + String(cmd));
	tx_print(cmd.c_str());
	bool success = false;

	uint32_t currentTime = millis();
	while(millis() < (currentTime + timeout))
	{
		while(_rxSerial->available()) response += (char) _rxSerial->read();
		/* Quebra o loop se o OK for encontrado */
		if(response.indexOf(F("OK")) != -1)
		{
			success = true;
			break;
		}
	}
	if(success)debug_print("Success! RSP: \n_\n" + response + "\n_");
	else debug_print("Failed :\n_\n " + response + "\n_");
	return response;
}

void ESP8266Modem::setHwResetPin(uint8_t pin)
{
	_rstPin = pin;
	digitalWrite(_rstPin, HIGH);
	pinMode(_rstPin, OUTPUT);
}
void ESP8266Modem::hwReset()
{
	digitalWrite(_rstPin, LOW);
	delay(10);
	digitalWrite(_rstPin, HIGH);
	delay(500);
}

void ESP8266Modem::enableDebug()
{
	_isDebugEnabled = true;
}

void ESP8266Modem::disableDebug()
{
	_isDebugEnabled = false;
}

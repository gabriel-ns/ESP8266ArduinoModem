#include <Arduino.h>
#include <stdint.h>
#include <stddef.h>

class ESP8266Modem
{
public:
	ESP8266Modem(Stream *ptxSerial, Stream *prxSerial, Stream *pdebugSerial=NULL);

	bool isPresent(uint16_t timeout=500, uint8_t retries=2);

	String connectWifi(const char * ssid, const char * password, uint16_t timeout=10000);
	String disconnectWifi();
	String showNetworkStatus();
	String swReset();

	void setHwResetPin(uint8_t pin);
	void hwReset();

	String connectToServer(String type, String server, uint16_t port);
	String httpGet(String data);

	String sendModemCommand(String cmd, uint16_t timeout=500);

	void enableDebug();
	void disableDebug();

private:
	bool _isDebugEnabled = false;

	Stream *_txSerial = NULL;
	Stream *_rxSerial = NULL;
	Stream *_debugSerial = NULL;
	uint8_t _rstPin = 99;

	template <typename Output>
	void debug_print(Output text);

	template <typename Output>
	void tx_print(Output text);

};

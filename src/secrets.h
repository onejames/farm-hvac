/*
  HVAC Monitor - AWS IoT & Wi-Fi Secrets

  This file contains the sensitive information required to connect to your
  Wi-Fi network and the AWS IoT Core service.

  **Instructions:**
  1. Fill in your Wi-Fi SSID and Password.
  2. In the AWS IoT Core console, create a "Thing" for your ESP32.
  3. Download the certificates for your Thing.
  4. Paste your unique AWS IoT Endpoint URL.
  5. Paste the contents of the certificate files into the corresponding
     variables below, including the "-----BEGIN..." and "-----END..." lines.
*/

#include <pgmspace.h>

// A unique name for your device
#define THINGNAME "ESP32_HVAC_MONITOR"

// Your Wi-Fi network credentials
const char WIFI_SSID[] = "TheFarmIot";
const char WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD";

// Your unique AWS IoT Core endpoint
const char AWS_IOT_ENDPOINT[] = "your-iot-endpoint.iot.your-region.amazonaws.com";

// The MQTT topic to publish data to
const char AWS_IOT_TOPIC[] = "hvac/data";

// Amazon Root CA 1 certificate
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)EOF";

// Device certificate
static const char AWS_CERT_CRT[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)EOF";

// Device private key
static const char AWS_CERT_PRIVATE[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
-----END RSA PRIVATE KEY-----
)EOF";
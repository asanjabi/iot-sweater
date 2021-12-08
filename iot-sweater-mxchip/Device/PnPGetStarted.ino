// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.
// To get started please visit https://microsoft.github.io/azure-iot-developer-kit/docs/projects/connect-iot-hub?utm_source=ArduinoExtension&utm_medium=ReleaseNote&utm_campaign=VSCode
#include "AZ3166WiFi.h"
#include "AzureIotHub.h"
#include "DevKitMQTTClient.h"
#include "azure_prov_client/prov_security_factory.h"
#include "DevkitDPSClient.h"

#include "config.h"
#include "utility.h"
#include "SystemTickCounter.h"
#include "neopixel.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Secrets

// Fill these in from your device values in Azure IOT Central
static const char *deviceId = "<replace with your device Id>";
static char *sasPrimaryKey = "<replace with your Device SAS Primary Key>";
static const char *idScope = "<replace with your Id Scope>";
//////////////////////////////////////////////////////////////////////////////////////////////////////////
static const char *globaldDpsUri = "global.azure-devices-provisioning.net";

//////////////////////////////////////////////////////////////////////////////////////////////////////////
#define NUMBER_OF_LED 30 // This is the number of LEDs in your strip
PinName LED_PIN = PB_6; // this is the pin you plugged the data wire into

static NeoPixel *neopixel = NULL;

enum Patterns
{
  None = 0,
  Rainbow,
  Snake
};
Patterns pattern = None;

//////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool hasWifi = false;
int messageCount = 1;
int sentMessageCount = 0;
static bool messageSending = true;
static uint64_t send_interval_ms;
static const char model_id[] = "dtmi:com:contoso:iotSweater:devthehalls;1";
static const char sampleProperties[] = "{\"sample\": \"DevKit-PnP-GetStarted\" }";
static const char responseMessageFormat[] = "{\"message\": \"%s\" }";
static const char *successResponse = "Successfully invoke device method";
static const char *failResponse = "No method found";

static float temperature;
static float humidity;
static float pressure;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utilities
static void InitWifi()
{
  Screen.print(2, "Connecting...");

  if (WiFi.begin() == WL_CONNECTED)
  {
    IPAddress ip = WiFi.localIP();
    Screen.print(1, ip.get_address());
    hasWifi = true;
    Screen.print(2, "Running... \r\n");
  }
  else
  {
    hasWifi = false;
    Screen.print(1, "No Wi-Fi\r\n ");
  }
}

static void BuildCommandResponse(unsigned char **response, int *response_size, const char *message)
{
  int responseBuilderSize = 0;
  unsigned char *responseBuilder = NULL;
  int result = 200;

  if ((responseBuilderSize = snprintf(NULL, 0, responseMessageFormat, message)) < 0)
  {
    LogError("snprintf to determine string length for command response failed");
  }
  else if ((responseBuilder = (unsigned char *)calloc(1, responseBuilderSize + 1)) == NULL)
  {
    LogError("Unable to allocate %lu bytes", (unsigned long)(responseBuilderSize + 1));
  }
  else if ((responseBuilderSize = snprintf((char *)responseBuilder, responseBuilderSize + 1, responseMessageFormat, message)) < 0)
  {
    LogError("snprintf to output buffer for command response");
  }

  *response = responseBuilder;
  *response_size = responseBuilderSize;
  LogInfo("Response=<%s>", (const char *)responseBuilder);
}

static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result)
{
  if (result == IOTHUB_CLIENT_CONFIRMATION_OK)
  {
    blinkSendConfirmation();
    sentMessageCount++;
  }

  Screen.print(1, "> IoT Hub");
  char line1[20];
  sprintf(line1, "Count: %d/%d", sentMessageCount, messageCount);
  Screen.print(2, line1);

  char line2[20];
  sprintf(line2, "T:%.2f H:%.2f", temperature, humidity);
  Screen.print(3, line2);
  messageCount++;
}

static void MessageCallback(const char *payLoad, int size)
{
  blinkLED();
  Screen.print(1, payLoad, true);
}

static void DeviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int size)
{
  char *temp = (char *)malloc(size + 1);
  if (temp == NULL)
  {
    return;
  }
  memcpy(temp, payLoad, size);
  temp[size] = '\0';
  parseTwinMessage(updateState, temp);
  free(temp);
}

static int DeviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size)
{
  LogInfo("Try to invoke method %s", methodName);
  int result = 200;

if (strcmp(methodName, "TurnOff") == 0)
  {
    LogInfo("Turn off pattern");
    messageSending = false;

    pattern = None;
  }
  else if (strcmp(methodName, "ShowRainbow") == 0)
  {
    LogInfo("Start showRainbowCycle");
    messageSending = false;

    pattern = Rainbow;
  }
  else if (strcmp(methodName, "ShowSnake") == 0)
  {
    LogInfo("Start ShowSnake");
    messageSending = false;

    pattern = Snake;
  }
  else
  {
    LogInfo("No method %s found", methodName);
    BuildCommandResponse(response, response_size, failResponse);
    result = 404;
  }
  BuildCommandResponse(response, response_size, successResponse);
  return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Arduino sketch
void setup()
{
  Screen.init();
  Screen.print(0, "IoT DevKit");
  Screen.print(2, "Initializing...");

  Screen.print(3, " > Serial");
  Serial.begin(115200);

  // Initialize the WiFi module
  Screen.print(3, " > WiFi");
  hasWifi = false;
  InitWifi();
  if (!hasWifi)
  {
    return;
  }

  LogTrace("HappyPathSetup", NULL);

  Screen.print(3, " > Sensors");
  SensorInit();

  Screen.print(3, " > IoT Hub");

  DevkitDPSSetAuthType(DPS_AUTH_SYMMETRIC_KEY);
  prov_dev_set_symmetric_key_info(deviceId, sasPrimaryKey);
  DevkitDPSClientStart(globaldDpsUri, idScope, deviceId, sasPrimaryKey, NULL, 0);

  DevKitMQTTClient_SetOption(OPTION_MINI_SOLUTION_NAME, "DevKit-PnP-GetStarted");
  DevKitMQTTClient_Init(true, false, model_id);
  DevKitMQTTClient_SetSendConfirmationCallback(SendConfirmationCallback);
  DevKitMQTTClient_SetMessageCallback(MessageCallback);
  DevKitMQTTClient_SetDeviceTwinCallback(DeviceTwinCallback);
  DevKitMQTTClient_SetDeviceMethodCallback(DeviceMethodCallback);

  EVENT_INSTANCE *message = DevKitMQTTClient_Event_Generate(sampleProperties, STATE);
  DevKitMQTTClient_SendEventInstance(message);

  send_interval_ms = SystemTickCounterRead();

  neopixel = new NeoPixel(LED_PIN, NUMBER_OF_LED);
  neopixel->init();
}

void loop()
{
  if (hasWifi)
  {
    if (messageSending &&
        (int)(SystemTickCounterRead() - send_interval_ms) >= getInterval())
    {
      // Send teperature data
      char messagePayload[MESSAGE_MAX_LEN];

      readMessage(messageCount, messagePayload, &temperature, &humidity, &pressure);
      EVENT_INSTANCE *message = DevKitMQTTClient_Event_Generate(messagePayload, MESSAGE);
      DevKitMQTTClient_SendEventInstance(message);

      send_interval_ms = SystemTickCounterRead();
    }
    else
    {
      DevKitMQTTClient_Check();
    }

    switch (pattern)
    {
    case None:
    LogInfo("No Pattern");
      neopixel->showColor(0, 0, 0);
      break;

    case 1:
      LogInfo("Rainbow...");
      neopixel->showRainbowCycle(60, 25, 20, 200);
      neopixel->showColor(0, 0, 0);
      break;

    case 2:
      LogInfo("Theatre-style crawling lights...");
      neopixel->showTheaterChase(236, 19, 122, 500);
      neopixel->showColor(0, 0, 0);
      break;
    }
  }
  delay(1000);
}

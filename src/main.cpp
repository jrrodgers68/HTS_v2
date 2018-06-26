/*
 * Project HTS_v2
 * Description: Battery operated temperature sensor
 * Author: John Rodgers
 * Date: 5/19/18
 */

#include "application.h"
#include <Adafruit_DHT.h>
#include <MQTT.h>
#include <PowerShield.h>

#include "wifi_creds.h"

#define MQTT_BROKER         "pi3_2"
#define MQTT_PORT           1883
#define LOW_BATTERY_LEVEL   15.0
#define AWAKE_LIMIT         10000
#define DEEP_SLEEP_TIME     600


SYSTEM_MODE(MANUAL);

DHT dht(D4, DHT22);
PowerShield batteryMonitor;

bool gFlashLED = false;

void callback(char* topic, byte* payload, unsigned int length);
MQTT client(MQTT_BROKER, MQTT_PORT, callback);

void startWifi()
{
    WiFi.on();
    WiFi.clearCredentials();
    WiFi.setCredentials(WIFI_SSA, WIFI_PWD);
    waitUntil(WiFi.ready);
}

void publishTemp(float temp)
{
    client.publish("","");
}

void publishFailure()
{
    client.publish("", "");
}

void publishSoC(float soC)
{
    client.publish("", "");
}

void flashLED()
{
    gFlashLED = true;
}


void callback(char* topic, byte* payload, unsigned int length)
{
}


// setup() runs once, when the device is first turned on.
void setup()
{
    batteryMonitor.begin();
    batteryMonitor.quickStart();
    float stateOfCharge = batteryMonitor.getSoC();

    dht.begin();

    float temp = dht.getTempFarenheit();
    float humidity = dht.getHumidity();

    startWifi();
    if (isnan(humidity) || isnan(temp))
    {
        flashLED();
        publishFailure();
        if(stateOfCharge < LOW_BATTERY_LEVEL)
        {
            publishSoC(stateOfCharge);
        }
    }
    else
    {
        publishTemp(temp);
        if(stateOfCharge < LOW_BATTERY_LEVEL)
        {
            publishSoC(stateOfCharge);
        }
    }
}

bool gStayAwake = false;

// loop() runs over and over again, as quickly as it can execute.
void loop()
{
    if((millis() > AWAKE_LIMIT) && !gStayAwake)
    {
        System.sleep(SLEEP_MODE_DEEP, DEEP_SLEEP_TIME);
    }
    else
    {
        client.loop();
        if(gFlashLED)
        {
            // turn LED on/ sleep 500 millis / turn LED off / sleep 500 milis
        }
    }
}

#include <Arduino.h>

/**
 * MYSENSORS
 **/
/* Enable debug prints to serial monitor */
#define MY_DEBUG
/* Enable and select radio type attached (NRF24) */
#define MY_RADIO_NRF24
/* Enable repeater functionality for this node */
#define MY_REPEATER_FEATURE

/* do not use to high baud rate when not needed */
#define MY_BAUD_RATE 9600
/* CE/CS PIN for NRF24L01+ can be redefined (9 and 10 by default but use by IR to have PWM) */
#define MY_RF24_CE_PIN 5
#define MY_RF24_CS_PIN 6

/* node id used for gateway (must be uniq) */
/* when not set it leave the gateway to assign an id (do not work always) */
#define MY_NODE_ID 43

/* MySensors will override usual function of a sketch, it for a node it need a gateway to start */
#include <MySensors.h>

/* Sensor id to present and finally received */
#define SENSOR_ID_SPRINKLER_FRONT  (0)
#define SENSOR_ID_SPRINKLER_MIDDLE (1)
#define SENSOR_ID_SPRINKLER_BACK   (2)
#define SENSOR_ID_DRIP_BY_DRIP     (3)
#define SENSOR_ID_HUMIDITY         (4)

/* 10 minutes in milliseconds (10 * 60 * 1000) */
#define HUMIDITY_POOLING_TIME (600000)
#define HUMIDITY_POOLING_TIME (500)

/* PIN number of each relay */
#define PIN_ID_SPRINKLER_FRONT  (7)
#define PIN_ID_SPRINKLER_MIDDLE (8)
#define PIN_ID_SPRINKLER_BACK   (9)
#define PIN_ID_DRIP_BY_DRIP     (10)
/* PIN number of rain sensor */
#define PIN_ID_RAIN_SENSOR      (19)

/* relay state */
#define WATERING_CLOSED (HIGH)
#define WATERING_RUNNING (LOW)

/* declare message about humidity sensor (dry/wet) */
MyMessage HumidityMessage(SENSOR_ID_HUMIDITY, V_TRIPPED);

boolean IsWateringRunning = false;


void before()
{
  /* everything before MySensors execution */
  pinMode(PIN_ID_SPRINKLER_FRONT, OUTPUT);
  pinMode(PIN_ID_SPRINKLER_MIDDLE, OUTPUT);
  pinMode(PIN_ID_SPRINKLER_BACK, OUTPUT);
  pinMode(PIN_ID_DRIP_BY_DRIP, OUTPUT);

  /* make sure sprinkler and drip are closed */
  digitalWrite(PIN_ID_SPRINKLER_FRONT, WATERING_CLOSED);
  digitalWrite(PIN_ID_SPRINKLER_MIDDLE, WATERING_CLOSED);
  digitalWrite(PIN_ID_SPRINKLER_BACK, WATERING_CLOSED);
  digitalWrite(PIN_ID_DRIP_BY_DRIP, WATERING_CLOSED);

  pinMode(PIN_ID_RAIN_SENSOR, INPUT);
}

void setup()
{
  //This pipes to the serial monitor
  /* Serial.begin(9600); */
}

void presentation()
{
  /* Send the sketch version information to the gateway and Node */
  sendSketchInfo("Garden Watering System", "1.0");
  /* everything to present each sensors/actuators on this node to the gateway (so domotic box will register it) */
  present(SENSOR_ID_SPRINKLER_FRONT, S_BINARY);
  present(SENSOR_ID_SPRINKLER_MIDDLE, S_BINARY);
  present(SENSOR_ID_SPRINKLER_BACK, S_BINARY);
  present(SENSOR_ID_DRIP_BY_DRIP, S_BINARY);
  present(SENSOR_ID_HUMIDITY, S_BINARY);
}

void loop()
{
  boolean isWet = false;

  /* get sensor state (5V dry and GND for wet) */
  isWet = !digitalRead(PIN_ID_RAIN_SENSOR);

  send(HumidityMessage.set(isWet ? "1" : "0"));
  sleep(HUMIDITY_POOLING_TIME);

  if (IsWateringRunning)
  {
    IsWateringRunning = false;
  }
  else
  {
    digitalWrite(PIN_ID_SPRINKLER_FRONT, WATERING_CLOSED);
    digitalWrite(PIN_ID_SPRINKLER_MIDDLE, WATERING_CLOSED);
    digitalWrite(PIN_ID_SPRINKLER_BACK, WATERING_CLOSED);
    digitalWrite(PIN_ID_DRIP_BY_DRIP, WATERING_CLOSED);
  }
}

void receive(const MyMessage &message)
{
  Serial.println("Message received!");
  if (message.type == V_STATUS)
  {
    switch(message.sensor)
    {
      case SENSOR_ID_SPRINKLER_FRONT:
        digitalWrite(PIN_ID_SPRINKLER_FRONT, message.getBool() ? WATERING_RUNNING : WATERING_CLOSED);
        IsWateringRunning = true;
        break;

      case SENSOR_ID_SPRINKLER_MIDDLE:
        digitalWrite(PIN_ID_SPRINKLER_MIDDLE, message.getBool() ? WATERING_RUNNING : WATERING_CLOSED);
        IsWateringRunning = true;
        break;

      case SENSOR_ID_SPRINKLER_BACK:
        digitalWrite(PIN_ID_SPRINKLER_BACK, message.getBool() ? WATERING_RUNNING : WATERING_CLOSED);
        IsWateringRunning = true;
        break;

      case SENSOR_ID_DRIP_BY_DRIP:
        digitalWrite(PIN_ID_DRIP_BY_DRIP, message.getBool() ? WATERING_RUNNING : WATERING_CLOSED);
        IsWateringRunning = true;
        break;

      default:
        Serial.println("Message received with unknown sensor ID.");
        break;
    }
  }
  else
  {
    Serial.println("Message received with unknown type.");
  }
}

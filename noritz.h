#include "esphome.h"
#include <cmath>

// UART data
unsigned char data[6] = {};

class Noritz : public Component, public UARTDevice
{
public:
  // Constructor
  
  Sensor *power = new Sensor();
  Sensor *temperature = new Sensor();

  Noritz(UARTComponent *parent) : UARTDevice(parent) {};

	// void binarySensor(BinarySensor* sensor, bool value)
	// {
	// 	if (sensor->state != value)
	// 	{
	// 		sensor->publish_state(value);
	// 	}
	// }

  void sensor(Sensor* sensor, float value)
  {
    if (sensor->state != value)
    {
      sensor->publish_state(value);
    }
  }

  void setup() override
  {
    // Nothing to do here
  }
  // loop is called many times per second
  void loop() override
  {
      data[0] = 0xdf;
      data[1] = 0x00;
      if (available())
      {
        if (read() == 0xdf)
        {
          for (int i = 1; i < 6; i++)
          {
            data[i] = read();
          }
        }
      }
      flush();

    if (data[1] == 0xef && data[2] == 0xff)
    {
      int sum;
      bool power_state;
      int temperature_state=35;
      int temp_byte;
      sum = data[0] + data[1] + data[2] + data[3] + data[4] - 945;
      if (sum == data[5])
      {
        if ((data[3] & 0x01) == 0)
        {
          power_state = true;
        }
        else
        {
          power_state = false;
        }
        // int buffer = data[4];
        // ESP_LOGD("custom", "data4: %i", buffer);
        if ((data[4] >> 4) == 13)
        {
          ESP_LOGD("custom", "getting temp");
          temp_byte = data[4]&0x0f;
          if (temp_byte > 6)
          {
            temperature_state = 53 - temp_byte;
          }
          else
          {
            switch (temp_byte)
            {
            case 0:
              temperature_state = 47;
              break;
            case 1:
              temperature_state = 48;
              break;
            case 2:
              temperature_state = 35;
              break;
            case 3:
              temperature_state = 37;
              break;
            case 5:
              temperature_state = 60;
              break;
            case 6:
              temperature_state = 50;
              break;  
            }
          }
        }

        sensor(power, power_state);
        sensor(temperature, temperature_state);
      }
      else
      {
        ESP_LOGW("custom", "Checksum ERROR");
      }
    }
  }
};

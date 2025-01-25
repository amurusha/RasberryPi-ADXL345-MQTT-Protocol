# RaspberryPi-ADXL345-MQTT
Interfaced ADXL345 sensor with Raspberry Pi

1) Set up Paho on Raspberry Pi.
2) Connected ADXL345 with Raspberry Pi using I2C Bus.
3) Installed MQTT on RPi, LINUX VM.
4) Collected CPU Temperature, Time, Pitch and Roll values from sensor.
5) Published the values on MQTT server.
6) Created subscribers to subscribe to the topic to display values on subscriber side.
  6.1) Created a subscriber which lights up an LED when value of pitch increases a threshold value.
  6.2) Tested the application using QoS levels 0, 1 and 2.
7) Created a QT application to visualize the values wrt time on QT editor by subscribing to the topic.
8) Documented the entire project 

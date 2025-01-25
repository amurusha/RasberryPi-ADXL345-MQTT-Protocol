// Based on the Paho C code example from www.eclipse.org/paho/
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include "MQTTClient.h"
#include "ADXL345.h"
#define  CPU_TEMP "/sys/class/thermal/thermal_zone0/temp"
using namespace std;
using namespace exploringRPi;

//Please replace the following address with the address of your server
#define ADDRESS    "tcp://172.20.10.7:1883"
#define CLIENTID   "rpi1"
#define AUTHMETHOD "Spandan"
#define AUTHTOKEN  "spd"
#define TOPIC1      "ee513/TimeRoll"
#define TOPIC2      "ee513/TempPitch"
#define QOS        1
#define TIMEOUT    10000L

float getCPUTemperature() {        // get the CPU temperature
   int cpuTemp;                    // store as an int
   fstream fs;
   fs.open(CPU_TEMP, fstream::in); // read from the file
   fs >> cpuTemp;
   fs.close();
   return (((float)cpuTemp)/1000);
}

int main(int argc, char* argv[]) {


	//ADXL345 CODE TO GET PITCH & ROLL
	ADXL345 sensor(1,0x53);
	sensor.setResolution(ADXL345::NORMAL);
	sensor.setRange(ADXL345::PLUSMINUS_4_G);
	sensor.displayPitchAndRoll(25);
	float pitch = sensor.getPitch();
	float roll = sensor.getRoll();



	//TIME
	time_t now = time(NULL);
	struct tm* timeinfo = localtime(&now);
	char curr_time[20];
	strftime(curr_time, 20, "%Y-%m-%d %H:%M:%S", timeinfo);

	//CONSTRUCT THE MQTT MESSAGE FOR PAYLOAD 1
	char str_payload1[100];
	sprintf(str_payload1, "{\"d\":{\"Time\": \"%s\", \"Roll\": %f}}",curr_time,roll);

	//CONSTRUCT THE MQTT MESSAGE FOR PAYLOAD 2
	char str_payload2[100];
	sprintf(str_payload2, "{\"d\":{\"Pitch\": %f,\"CPU Temperature\": %f }}",pitch,getCPUTemperature());

	//PUBLISH THE MQTT MESSAGE
	MQTTClient client;
	MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;

	//PAYLOAD 1
	MQTTClient_message pubmsg1 = MQTTClient_message_initializer;
	//PAYLOAD 2
	MQTTClient_message pubmsg2 = MQTTClient_message_initializer;

	MQTTClient_deliveryToken token;
	MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	opts.keepAliveInterval = 20;
	opts.cleansession = 1;
	opts.username = AUTHMETHOD;
	opts.password = AUTHTOKEN;
	int rc;
	if ((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS) {
	    cout << "Failed to connect, return code " << rc << endl;
	    return -1;
	}

	//PAYLOAD 1
	pubmsg1.payload = str_payload1;
	pubmsg1.payloadlen = strlen(str_payload1);
	pubmsg1.qos = QOS;
	pubmsg1.retained = 0;
	MQTTClient_publishMessage(client, TOPIC1, &pubmsg1, &token);

	//PAYLOAD 2
	pubmsg2.payload = str_payload2;
	pubmsg2.payloadlen = strlen(str_payload2);
	pubmsg2.qos = QOS;
	pubmsg2.retained = 0;
	MQTTClient_publishMessage(client, TOPIC2, &pubmsg2, &token);


	cout << "Waiting for up to " << (int)(TIMEOUT/1000) <<
	    " seconds for publication of " << str_payload1 <<
	    " \non topic " << TOPIC1 << " for ClientID: " << CLIENTID << endl;

	cout << "Waiting for up to " << (int)(TIMEOUT/1000) <<
		    " seconds for publication of " << str_payload2 <<
		    " \non topic " << TOPIC2 << " for ClientID: " << CLIENTID << endl;

	rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
	cout << "Message with token " << (int)token << " delivered." << endl;
	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(&client);
	return rc;
}

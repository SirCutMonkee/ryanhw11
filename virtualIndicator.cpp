/* virtualIndicator.cpp
* HW 11 problem 3
* by Joshua Ryan
* modified from publish.cpp in ~/exploringBB/chp11/adafruit/
* To run this program use: virtualIndicator
* This program takes no arguments
*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <MQTTClient.h>
using namespace std;

//Please replace the username, feed and token with your values
#define ADDRESS    "tcp://io.adafruit.com"
#define CLIENTID   "Beagle1"
#define TOPIC      "JoshuaRyan99/feeds/gpio46.button"
#define AUTHMETHOD "JoshuaRyan99"
#define AUTHTOKEN  "aio_WIbT85FVjnhBCvaaYgb4G3TiWVSZ"
#define QOS        1
#define TIMEOUT    10000L
#define GPIO_PATH "/sys/class/gpio/gpio46/value"

string getButtonState() {
   ifstream fs;
   fs.open(GPIO_PATH);
   if(!fs.is_open()){
      perror("GPIO: read failed to open file\n");
   }
   string input;
   getline(fs,input);
   fs.close();
   return input;
}

int main(int argc, char* argv[]) {
   char str_payload[100];          // Set your max message size here
   MQTTClient client;
   cout << "Starting BeagleBoard Virtual Button Indicator" << endl;
   MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
   MQTTClient_message pubmsg = MQTTClient_message_initializer;
   MQTTClient_deliveryToken token;
   MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
   opts.keepAliveInterval = 0;
   opts.cleansession = 1;
   opts.username = AUTHMETHOD;
   opts.password = AUTHTOKEN;
   int rc;
   if ((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS) {
      cout << "Failed to connect, return code " << rc << endl;
      return -1;
   }
   cout << "Press Q or q <Enter> to quit" << endl;
   string state;
   string lastState = "0";
   while(1){
      state = getButtonState();
      if(state != lastState){
         lastState = state;
         strcpy(str_payload, state.c_str());
         pubmsg.payload = str_payload;
         pubmsg.payloadlen = strlen(str_payload);
         pubmsg.qos = QOS;
         pubmsg.retained = 0;
         MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
         cout << "Waiting for up to " << (int)(TIMEOUT/1000) <<
           " seconds for publication of " << str_payload <<
           " \non topic " << TOPIC << " for ClientID: " << CLIENTID << endl;
         rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
         if(rc == MQTTCLIENT_SUCCESS) {
           cout << "Message with token " << (int)token << " delivered." << endl;
         }
         else {
           cout << "Did not complete with error code: " << rc << endl;
           // MQTTCLIENT_SUCCESS 0           MQTTCLIENT_FAILURE -1
           // MQTTCLIENT_DISCONNECTED -3     MQTTCLIENT_MAX_MESSAGES_INFLIGHT -4
           // MQTTCLIENT_BAD_UTF8_STRING -5  MQTTCLIENT_NULL_PARAMETER -6
           // MQTTCLIENT_TOPICNAME_TRUNCATED -7   MQTTCLIENT_BAD_STRUCTURE -8
           // MQTTCLIENT_BAD_QOS   -9        MQTTCLIENT_SSL_NOT_SUPPORTED   -10
         }
      }
      else{
         int numRead;
         char buf[1];
         fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
         usleep(100);
         numRead = read(0, buf, 4);
         if(buf[0]=='Q' || buf[0]=='q'){
            break;
         }
      }
   }
   MQTTClient_disconnect(client, 10000);
   MQTTClient_destroy(&client);
   return rc;
}


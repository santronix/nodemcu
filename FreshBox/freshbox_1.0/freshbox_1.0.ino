#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "SANTRONIX";
const char* password = "XXXXXXX";
const char* mqtt_server = "test.mosquitto.org";
bool param = false;
const int UV_LIGHT_PIN = 16;

bool uv_started = false;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
long totalElapsed = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  StaticJsonBuffer<200> JSONBuffer;
  JsonObject& parsed = JSONBuffer.parseObject(payload);
  if(!parsed.success()){
    Serial.println("parsing failed");
  }else{
    String boxStatus = parsed["status"];
    Serial.println("Status : "+boxStatus);
    if(boxStatus == "00"){
      digitalWrite(UV_LIGHT_PIN, HIGH);
      uv_started = false;
      totalElapsed = 0;
    }else if(boxStatus =="01"){
      digitalWrite(UV_LIGHT_PIN, LOW);
      uv_started = true;
    }
  }
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("FB/VXDF323", "{\"deviceId\":\"VXDF323\",\"deviceImage\":\"RefreshBox.png\",\"deviceName\":\"Fresh Box\",\"deviceVersion\":\"2.1\",\"status\":\"01\"}");
      // ... and resubscribe
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["deviceId"] = "VXDF323";
      json["deviceImage"] = "RF";
      json["deviceName"] = "FB";
      json["deviceVersion"] = "2.1";
      json["status"] = "00";
      char payload[114];
      json.printTo(payload, sizeof(payload));
      Serial.println(payload);
      //client.publish("FB/VXDF323",payload);
  
      client.subscribe("FB/VXDF323");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(UV_LIGHT_PIN, OUTPUT);
  digitalWrite(UV_LIGHT_PIN, HIGH);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if(uv_started){
    totalElapsed += 1000;
    Serial.println("Total Elapsed : " + totalElapsed);
    long now = millis();
    if(now - lastMsg > 60000){
      lastMsg = now;
      //Send UV Light ON message every 1 second
      Serial.print("Send Heartbeat: ");
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["deviceId"] = "VXDF323";
      json["deviceImage"] = "RF";
      json["deviceName"] = "FB";
      json["deviceVersion"] = "2.1";
      json["status"] = "01";
      char payload[114];
      json.printTo(payload, sizeof(payload));
      Serial.println(payload);
      client.publish("FB/VXDF323",payload);
    }
    
    if(totalElapsed > 1800000){
      uv_started = false;
      digitalWrite(UV_LIGHT_PIN, HIGH);
      totalElapsed = 0;

      //Publish UV Light Off status
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["deviceId"] = "VXDF323";
      json["deviceImage"] = "RF";
      json["deviceName"] = "FB";
      json["deviceVersion"] = "2.1";
      json["status"] = "00";
      char payload[114];
      json.printTo(payload, sizeof(payload));
      Serial.println(payload);
      client.publish("FB/VXDF323",payload); 
    }
  }else{
    long now = millis();
    if(now - lastMsg > 60000){
      lastMsg = now;
      //Send UV Light ON message every 1 second
      Serial.print("Send Heartbeat: ");
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["deviceId"] = "VXDF323";
      json["deviceImage"] = "RF";
      json["deviceName"] = "FB";
      json["deviceVersion"] = "2.1";
      json["status"] = "00";
      char payload[114];
      json.printTo(payload, sizeof(payload));
      Serial.println(payload);
      client.publish("FB/VXDF323",payload);
    }
  }
  delay(1000);
}

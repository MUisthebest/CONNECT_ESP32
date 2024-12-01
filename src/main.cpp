#include <HardwareSerial.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Thông tin WiFi
const char* ssid = "Wokwi-GUEST";      // Tên mạng WiFi
const char* password = ""; // Mật khẩu WiFi

// Thông tin MQTT HiveMQ Cloud
const char* mqtt_server = "945d37355b7044d686d91d35ef4f0e67.s1.eu.hivemq.cloud";
const int mqtt_port = 8883; // Port TLS MQTT
const char* mqtt_user = "bangbangshooting";  // Tên người dùng HiveMQ
const char* mqtt_password = "Bangcuto24052004";  // Mật khẩu HiveMQ

// Sử dụng Serial2 (cổng UART2 trên ESP32)
HardwareSerial mySerial(1);
WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  
  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  mySerial.begin(9600, SERIAL_8N1, 4, 2);
  Serial.println("ESP32 Ready!");
  delay(1500);

  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  mySerial.println("Hello");

  // Cấu hình client TLS
  wifiClient.setInsecure(); // Sử dụng kết nối không xác thực (không cần chứng chỉ)

  // Cấu hình MQTT server
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback); // Đăng ký hàm xử lý khi nhận tin nhắn
}

void reconnect() {
  // Kết nối lại MQTT nếu bị ngắt
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("Connected");

      // Subscribe vào topic
      client.subscribe("test");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
    delay(100);
  }
}

void sendMQTTMessage(const char* topic, const char* payload) {
  if (client.publish(topic, payload)) {
    Serial.println("Message sent successfully");
  } else {
    Serial.println("Failed to send message");
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED)
  {
  if (!client.connected()) {
    reconnect();
  }
  sendMQTTMessage("test", "Hello from ESP32!");
  client.loop();
  mySerial.println("Hello Arduino, đây là ESP32!");

  // Nhận dữ liệu từ Arduino Mega
  if (mySerial.available()) {
    String received = mySerial.readStringUntil('\n');
    Serial.println("Received: " + received);
  }

  delay(1000); // Chờ 1 giây
  }

}

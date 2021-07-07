
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define relay 16
#define trigger  5
#define echo 4

//#define ssid "Dawi Wifi"
//#define password "@cozy190900"
#define ssid "cozyID"
#define password "cozy190900"
//const char* ssid = "Eka";
//const char* password = "22222222";
#define mqtt_server "192.168.43.51"
//#define mqtt_server "192.168.1.5"

bool mode_kondisi = false; // untuk state kondisi mode automatis atau tidak, jika true maka mode automatis
bool kondisi_keran = false; // untuk state keran hidup atau mati, jika true maka hidup, dan sebaliknya
int ketinggian_air_terbaru = 0; // ketinggian air terbaru dalam liter
int data_penggunaan = 0; // ketinggian air terbaru dalam liter untuk kenaikan topic

//void pub_ketinggian_air__untuk_curva(int ketinggian_liter);
void pub_data_pengguna(int data, int kondisi, int ketinggian_liter);

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  pinMode(relay, OUTPUT);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);

  if(kondisi_keran){
      digitalWrite(relay, HIGH);
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  if(strcmp(topic, "user/mode") == 0){
      String mesg_mode;
      for (int i = 0; i < length; i++) {
        mesg_mode += (char)payload[i];
      }
      if(mesg_mode == "aktifkan_mode_automatis"){
        mode_kondisi = true;
        Serial.println("Mode automatis dinyalakan");
      }else{
        mode_kondisi = false;
        Serial.println("Mode automatis dimatikan");
      }
  }else if(strcmp(topic, "user/saklar") == 0){
        String mesg_saklar;
        for (int i = 0; i < length; i++) {
          mesg_saklar += (char)payload[i];
        }
        if(mesg_saklar == "1"){
            digitalWrite(relay, HIGH);
            kondisi_keran = true;
            Serial.println("Pompa dinyalakan");
        }else{
            digitalWrite(relay, LOW);
            kondisi_keran = false;
            Serial.println("Pompa dimatikan");
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
    if (client.connect("ESP8266Client", "kev", "kev")) {
      Serial.println("connected");
      // ... resubscribe
      client.subscribe("user/mode");
      client.loop(); 
      client.subscribe("user/saklar");
      client.loop();
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
  // put your setup code here, to run once:
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1234);
  client.setCallback(callback);
}



void loop() {
  float durasi, jarak;
  
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  
  durasi = pulseIn(echo, HIGH);
  jarak = durasi / 29.1 / 2 ;
  Serial.println(jarak);
  // char nilai_ketinggian_air = "5";
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();
  client.subscribe("user/mode");
  client.loop();
  client.subscribe("user/saklar");
  client.loop();

//  disini kita akan menghitung beberapa jarak, saya sengaja menggunakan perkondisian memanggil meth
  if(jarak <= 28 && jarak > 27){
      pub_data_pengguna(28, 1, 1);
  }else if(jarak <= 27 && jarak > 26){
      pub_data_pengguna(27, 1, 2);
  }else if(jarak <= 26 && jarak > 25){
      pub_data_pengguna(26, 1, 3);
  }else if(jarak <= 25 && jarak > 24){
      pub_data_pengguna(25, 2, 6);
  }else if(jarak <= 24 && jarak > 23){
      pub_data_pengguna(24, 2, 7);
  }else if(jarak <= 23 && jarak > 22){
      pub_data_pengguna(23, 2, 10);
  }else if(jarak <= 22 && jarak > 21){
      pub_data_pengguna(22, 2, 15);
  }else if(jarak <= 21 && jarak > 20){
      pub_data_pengguna(21, 2, 17);
  }else if(jarak <= 20 && jarak > 19){
      pub_data_pengguna(20, 0, 18);
  }else if(jarak <= 19 && jarak > 18){
      pub_data_pengguna(19, 0, 19);
  }else if(jarak <= 18 && jarak > 17){
      pub_data_pengguna(18, 0, 16);
  }else if(jarak <= 17 && jarak > 16){
      pub_data_pengguna(17, 0, 18);
  }else if(jarak <= 16 && jarak > 15){
      pub_data_pengguna(16, 0, 19);
  }else if(jarak <= 15 && jarak > 14){
      pub_data_pengguna(15, 0, 19);
  }else if(jarak <= 14 && jarak > 13){
      pub_data_pengguna(14, 0, 19);
  }else if(jarak <= 13 && jarak > 12){
      pub_data_pengguna(13, 2, 19);
  }else if(jarak <= 12 && jarak > 11){
      pub_data_pengguna(12, 0, 19);
  }else if(jarak <= 11 && jarak > 10){
      pub_data_pengguna(11, 0, 19);
  }else if(jarak <= 10 && jarak > 9){
      pub_data_pengguna(10, 0, 19);
  }

//  Serial.println(perintah);
  delay(500);
}

void pub_data_pengguna(int data, int kondisi, int ketinggian_liter){
      Serial.println((String)"data : "+data);
      char msg_ketinggian[10];
      char msg_penggunaan[10];
      if(kondisi == 1){
          // mengecek apakah dalam keadan automatis atau tidak, jika true, maka nyalakan, karna air sudah didasar
          if(mode_kondisi){
              if(kondisi_keran == false){
                  digitalWrite(relay, HIGH); // menyalakan relay
                  kondisi_keran = true; // kondisi_keran kasih true / hidup
                  Serial.println("Menyalakan pompa"); 
              }else{
                  Serial.println("pompa sudah menyala"); 
              }
          }
          // kondisi dibawah ini berfungsi untuk data penggunaan air, yang disimpan di database
          // mengecek apakah keadaan keran menyala atau tidak, dan jika ketingian liter tidak sama dengan 0, jika menyala kita akan melakukan publish kenaikan air
          if(kondisi_keran){
            if(ketinggian_liter != 0){
                if(ketinggian_liter > data_penggunaan){
                    data_penggunaan = ketinggian_liter;
                    Serial.println("mengirim data penggunaan 1 liter");
                    sprintf(msg_penggunaan, "%d", 1);
                    client.publish("mc/data", msg_penggunaan);
                }else{
                    data_penggunaan = ketinggian_liter;
                } 
            }
          }
      }else if(kondisi == 0){
          // mengecek apakah dalam keadan automatis atau tidak, jika true, maka matikan, karna air sudah penuh
          if(mode_kondisi){
            if(kondisi_keran == true){
                digitalWrite(relay, LOW);
                kondisi_keran = false;
                Serial.println("Mematikan pompa"); 
            }else{
                Serial.println("pompa sudah mati"); 
            }
          }
          if(kondisi_keran){
            if(ketinggian_liter != 0){
                if(ketinggian_liter > data_penggunaan){
                    data_penggunaan = ketinggian_liter;
                    Serial.println("mengirim data penggunaan 1 liter");
                    sprintf(msg_penggunaan, "%d", 1);
                    client.publish("mc/data", msg_penggunaan);
                }else{
                    data_penggunaan = ketinggian_liter;
                } 
            }
          }
      }else if(kondisi == 2){
          if(kondisi_keran){
            if(ketinggian_liter != 0){
                if(ketinggian_liter > data_penggunaan){
                    data_penggunaan = ketinggian_liter;
                    Serial.println("mengirim data penggunaan 1 liter");
                    sprintf(msg_penggunaan, "%d", 1);
                    client.publish("mc/data", msg_penggunaan);
                }else{
                    data_penggunaan = ketinggian_liter;
                } 
            }
          }
      }
      
      // publish data ketinggian air untuk curva
      // cek dulu jika ketinggian liter 0 jangan dikirim
      if(ketinggian_liter != 0){
          sprintf(msg_ketinggian, "%d", ketinggian_liter);
          client.publish("mc/data/curva", msg_ketinggian);
          Serial.println(msg_ketinggian); 
      }
}

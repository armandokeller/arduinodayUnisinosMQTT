#include <WiFi.h>
#include <PubSubClient.h>


#define LED1 25
#define LED2 33
#define BT1 26
#define BT2 27

const char* ssid     = "coletividadeborg";
const char* password = "assimilado";

const char* host = "192.168.0.3";
WiFiClient wclient;
PubSubClient client(wclient);

bool estadoBTanterior1 = HIGH;
bool estadoBTatual1 = HIGH;
int t_debounce1 = 0;
int estadoLED1 = LOW;


bool estadoBTanterior2 = HIGH;
bool estadoBTatual2 = HIGH;
int t_debounce2 = 0;
int estadoLED2 = LOW;

void assinaTopicos()
{
  client.subscribe("arduinoday/led1");
  client.subscribe("arduinoday/led2");
}

void callback(char* topico, byte* payload, unsigned int length) {
  String mensagem = String();
  Serial.print("Mensagem recebida [");
  Serial.print(topico);
  Serial.print("] ");
  
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
    mensagem += (char)payload[i];
  }
  Serial.println();
  
  if(String(topico)=="arduinoday/led1"){
    if(mensagem == "on")
    {
      estadoLED1=HIGH;
      Serial.println("Liga led 1");
    }
    else if(mensagem == "off")
    {
      estadoLED1=LOW;
      Serial.println("Desliga led 1");
    }
  }
  else if(String(topico)=="arduinoday/led2"){
    if(mensagem == "on")
    {
      estadoLED2=HIGH;
      Serial.println("Liga led 2");
    }
    else if(mensagem == "off")
    {
      estadoLED2=LOW;
      Serial.println("Desliga led 2");
    }
  }
  digitalWrite(LED1,estadoLED1);
  digitalWrite(LED2,estadoLED2);
}

int testaBotao1()
{
   estadoBTanterior1=estadoBTatual1;
   estadoBTatual1=digitalRead(BT1);
   if(estadoBTatual1==LOW and estadoBTanterior1==HIGH)
   {
    t_debounce1=millis();
    while((millis()-t_debounce1)<50)
    {
      if(digitalRead(BT1)==HIGH) return(0);
    }
      return(1);
   }
   return(0);
}

int testaBotao2()
{
   estadoBTanterior2=estadoBTatual2;
   estadoBTatual2=digitalRead(BT2);
   if(estadoBTatual2==LOW and estadoBTanterior2==HIGH)
   {
    t_debounce2=millis();
    while((millis()-t_debounce2)<50)
    {
      if(digitalRead(BT2)==HIGH) return(0);
    }
      Serial.println("Botao2");
      return(1);
   }
   return(0);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando com broker MQTT\n");
    // Gerar um nome diferente para cada placa
    if (client.connect("arduinoday_156541351615")) {
      Serial.println("conectado");
      
      //client.publish("outTopi","conectado");
      
      //client.subscribe("armandokeller");
      assinaTopicos();
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}


void setup()
{
    Serial.begin(115200);
    delay(10);

    Serial.println();
    Serial.println();
    Serial.print("Conectando WiFi: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("");
    Serial.println("WiFi conectado");

    pinMode(LED1,OUTPUT);
    pinMode(LED2,OUTPUT);
    pinMode(BT1,INPUT_PULLUP);
    pinMode(BT2,INPUT_PULLUP);
    
    client.setServer(host, 1883);
    client.setCallback(callback);
}



void loop()
{
   if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if(testaBotao1())
  {
    if(estadoLED1==HIGH)
    {
      client.publish("arduinoday/led1","off");
    }
    else
    {
      client.publish("arduinoday/led1","on");
    }
  }
  if(testaBotao2())
  {
    if(estadoLED2==HIGH)
    {
      client.publish("arduinoday/led2","off");
    }
    else
    {
      client.publish("arduinoday/led2","on");
    }
  }
}


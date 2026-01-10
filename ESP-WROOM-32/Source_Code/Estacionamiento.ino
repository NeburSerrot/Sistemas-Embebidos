// ==================================================================================================
// Instituto Politécnico Nacional (IPN) - Escuela Superior de Cómputo (ESCOM)
// Proyecto: Sistema de Estacionamiento Inteligente IoT
// Unidad Académica:  Sistemas Embebidos
// Integrantes del Equipo:
// 1. Ayala Fuentes Sunem Gizeht
// 2. Michelle Anguiano Rodea
// 3. Torres Covarrubias Rubén
// ==================================================================================================

// ==================================================================================================
// Librerías
// ==================================================================================================

#include <WiFi.h>
#include <PubSubClient.h>

// ==================================================================================================
// Configuración de Red y Servidor
// ==================================================================================================
// Cambiar dependiendo si se trabajara usando Wowki y HiveMQ o únicamente de forma local:
// - Wokwi: ssid="Wokwi-GUEST", password="", mqtt_server="broker.hivemq.com"
// - Localmente: ssid="Nombre_del_Hotspot", password="Contraseña_del_Hotspot", mqtt_server="IP_que_el_Hotspot_asigno_a_la_Raspberry"

const char* ssid = "Redmi10";
const char* password = "escom1234";
const char* mqtt_server = "172.28.240.168";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// ==================================================================================================
// Asignación de Pines
// ==================================================================================================
// Matriz que guarda los pares {Trigger, Echo} para los 4 sensores HC-SR04.

const int sensorPins[4][2] = {
  {23, 22}, // Sensor 1: Trigger G23, Echo G22
  {21, 19}, // Sensor 2: Trigger G21, Echo G19
  {18, 5}, // Sensor 3: Trigger G18, Echo G5
  {17, 16}  // Sensor 4: Trigger G17, Echo G16
};

// ==================================================================================================
// Topics MQTT
// ==================================================================================================
// Rutas donde se publicaran los estados de cada uno de los espacios asigando a su respectivo sensor.

const char* topics[4] = {
  "escom/estacionamiento/espacio1",
  "escom/estacionamiento/espacio2",
  "escom/estacionamiento/espacio3",
  "escom/estacionamiento/espacio4"
};

// Array para recordar el último estado enviado
String lastState[4] = {"", "", "", ""};

// ==================================================================================================
// Funciones de Conexión
// ==================================================================================================

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a la Red WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Verificación de Conexión a la Red y Asignación de IP
  Serial.println("\nConectado a la Red: ");
  Serial.print(ssid);

  Serial.println("\nCon la Dirección IP: ");
  Serial.print(WiFi.localIP()); // Correción de wifi por WiFi
}

void reconnect() {
  while (!client.connected()) {
    // Cadena para verificar a que tipo de Broker se esta conectando
    String serverAddress = String(mqtt_server);
    
    Serial.println();
    Serial.print("Intentando conectar a: ");
    
    // Si la dirección contiene la palabra "hivemq", entonces es la Nube
    if (serverAddress.indexOf("hivemq") != -1) {
      Serial.print("HiveMQ Public");
    } 
    // Si no, es una IP Local
    else {
      Serial.print("Mosquitto MQTT ");
    }
    Serial.println(mqtt_server);

    // Generación de un ID aleatorio
    String clientId = "ESP32-Parking-";
    clientId += String(random(0xffff), HEX);

    // Comprobación de Conexión
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado al Broker");
    } else {
      Serial.print("Falló la conexión, código de error rc=");
      Serial.print(client.state());
      Serial.println("Reintentando en 5 segundos");
      delay(5000);
    }
  }
}

// Función para leer el sensor HC-SR04
long readUltrasonicDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);   // Comprobar que este apagado
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);  // Activar un pulso de 10us
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);   // Apagar pulso
  return pulseIn(echoPin, HIGH); // Medir tiempo de retorno
}

// ==================================================================================================
// Código Principal
// ==================================================================================================

void setup() {
  Serial.begin(115200);
  
  // Configuración de todos los Pines
  for(int i=0; i<4; i++){
    pinMode(sensorPins[i][0], OUTPUT); // Trigger es SALIDA
    pinMode(sensorPins[i][1], INPUT);  // Echo es ENTRADA
  }

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  Serial.println("Leyendo Sensores");
  
  // Recorriendo los 4 sensores uno por uno
  for(int i=0; i<4; i++) {
    long duration = readUltrasonicDistance(sensorPins[i][0], sensorPins[i][1]);
    // Cálculo de distancia en cm (Velocidad sonido: 0.034 cm/us)
    int distance = duration * 0.034 / 2;
    
    // Estado por defecto: LIBRE
    String currentState = "LIBRE";
    
    // Lógica de detección: Si está entre 1cm y 10cm, es un coche
    if (distance > 0 && distance < 10) { 
      currentState = "OCUPADO";
    }

    Serial.print("Espacio "); Serial.print(i+1);
    Serial.print(": "); Serial.print(distance); Serial.println("cm");

    // Esto sección evita enviar miles de mensajes "LIBRE, LIBRE, LIBRE" si no pasa nada.
    if (currentState != lastState[i]) {
      Serial.print("Cambio Detectado - Publicando: ");
      Serial.println(currentState);
      
      // Publicar al topic correspondiente del array
      client.publish(topics[i], currentState.c_str());
      
      // Actualizar el último estado conocido
      lastState[i] = currentState;
    }
    
    delay(50); // Pequeña pausa entre lecturas de sensores para estabilidad
  }
  Serial.println("------------------------------------------------------------------------");
  
  delay(1000); // Esperar 1 segundo antes de volver a medir los 4 sensores
}

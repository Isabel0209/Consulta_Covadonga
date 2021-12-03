#include <WiFi.h>
#include "FirebaseESP32.h"
#include "esp_adc_cal.h" //Libreria de calibración de adc
//Configuración LM35
#define LM35_Sensor1 35
int LM35_Raw_Sensor1 = 0;
float LM35_TempC_Sensor1 = 0.0;
float Voltage = 0.0;
//Subproceso para lectura de pin adc
uint32_t readADC_Cal(int ADC_Raw){
  esp_adc_cal_characteristics_t adc_chars; 
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  return(esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars));
}
//Indicando Conexión a wifi
#define WIFI_SSID "HACKATHON"
#define WIFI_PASSWORD "tectectec"
//Entrando a la base de datos
#define FIREBASE_HOST "https://covandonga-base-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "GEGvuRlNxMuZDGydNGWkJrKBebCSMvWIE1e53wPd"

FirebaseData firebaseData;

String nodo = "/Sensores";
bool iterar = true;
int altura=0;

const int Trigger = 4;   //Pin digital 2 para el Trigger del sensor
const int Echo = 5;   //Pin digital 3 para el Echo del sensor

void setup() {
  Serial.begin(115200);
  Serial.println();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectado al Wi-Fi");
  while (WiFi.status() !=WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  pinMode(Trigger, OUTPUT); //pin como salida
  pinMode(Echo, INPUT);  //pin como entrada
  digitalWrite(Trigger, LOW);//Inicializamos el pin con 0
  
}

void loop() { 
 long t; //timepo que demora en llegar el eco
  long d; //distancia en centimetros
  int Altura;
  int Peso;
  int Temperatura;
  int Oxigenacion;
  int presion;
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);
  
  t = pulseIn(Echo, HIGH); //obtenemos el ancho del pulso
  d = t/59;             //escalamos el tiempo a una distancia de cm
  if (d>200){
    d=200;
  }
  Altura = 200-d;  
  Serial.print("Distancia: ");
  Serial.print(d);      //Enviamos serialmente el valor de la distancia
  Serial.print("cm");
  Serial.println();
    // Leer pin de LM35
  LM35_Raw_Sensor1 = analogRead(LM35_Sensor1);  
  // Calibrando ADC y consiguiendo voltaje de sensor (en mV)
  Voltage = readADC_Cal(LM35_Raw_Sensor1);
  // TempC = Voltage(mV) / 10
  LM35_TempC_Sensor1 = Voltage / 10;
  Temperatura=LM35_TempC_Sensor1;
  
  //Imprimiendo la lectura de temperatura
  Serial.print("Temperatura = ");
  Serial.print(LM35_TempC_Sensor1);
  Serial.print(" °C , ");
  
  Firebase.setInt(firebaseData, nodo + "/prueba", Altura);
    Firebase.setInt(firebaseData, nodo + "/temperatura", Temperatura);
  iterar = false;

  delay(100);

}

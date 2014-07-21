/*
Adquisición del Inclinómetro
*/

// inslude the SPI library:
// Esta libreria esta en la web de arduino y permite
//controlar el inclinometro mediante el protocolo SPI
#include <SPI.h>


// Pin que irá conectado a la señal CS/SS para selección de esclavo
const int slaveSelectPin = 53;
const byte X_INCL = 0x0C;
const byte Y_INCL = 0x0E;
const byte ROT_INCL = 0x10;
int valor_x,valor_y, valor_rot;

void setup() {
  // set the slaveSelectPin as an output:
  
  // initialize SPI:
  SPI.begin(); 
  // este modo depende de phase y polarity 
  SPI.setDataMode(SPI_MODE3);
  // determina el bit más significativo en la comunicación
  SPI.setBitOrder(MSBFIRST);
  // determina el tamaño en bits de los ciclos de operaciones
  SPI.setClockDivider(SPI_CLOCK_DIV16); 
  // comienza la comunicación SPI a la velocidad indicada: 9600 es típica. 
  // Los valores recomendados están en la web
  Serial.begin(9600);
}

// operación en bucle que realizará el controlador arduino sobre el inclinometro
void loop() {

  
  //Serial.print("[ ");
  valor_x = leer_reg(X_INCL);
  //Serial.print(" , ");
  valor_y = leer_reg(Y_INCL);
  //Serial.print(" , ");
  valor_rot = leer_reg(ROT_INCL);
  //Serial.print(" ]: ");
  
  delay(100);
  
  // impresion de los valores
//  Serial.print("[ ");
//  Serial.print(valor_x);
//  Serial.print(" , ");
//  Serial.print(valor_y);
//  Serial.print(" , ");
//  Serial.print(valor_rot);
//  Serial.print(" ]\n");

    Serial.print(valor_x);
    Serial.print("\\");
    Serial.print(valor_y);
    Serial.print("\\");
    Serial.print(valor_rot);
    Serial.print("\n");
    


}

// Función que lee los registros 0x0C 0x0E 0x10
int leer_reg(byte dir){
  byte MSB, LSB; // valores locales de los Bytes del registro
  int valor; // valor que almacena la inclinacion como entero y que habrá que multiplicar por la sensibilidad 0.025 º
  
  //Comienzo la transferencia
  digitalWrite(slaveSelectPin,LOW); // Pone a 0 el CS para la selección del esclavo
  
  //pido los datos
  MSB = SPI.transfer(dir); // Copia el contenido de la dirección dir
  LSB = SPI.transfer(dir); // Copia el contenido de la dirección dir
  
  digitalWrite(slaveSelectPin,HIGH); // Resetea la comunicación
  
  delay(1); // Necesario para la siguiente lectura
  
  // El valor pedido no es el actual, sino el anterior, necesitamos
  // Volver a leer el registro para coger el dato adecuado
  //comienzo otro ciclo para leer los datos
    digitalWrite(slaveSelectPin,LOW); // Pone a 0 el CS para la selección del esclavo
  
  //almaceno los datos
  MSB = SPI.transfer(dir); // Copia el contenido de la dirección dir
  LSB = SPI.transfer(dir); // Copia el contenido de la dirección dir
  
  digitalWrite(slaveSelectPin,HIGH); // Resetea la comunicación
  
  // Procesado del valor como complemento a 2
  
  // deberia comprobar los flags...
  // 15 -- new_data
  
  //if((MSB & 0x80)> 0x00) Serial.print("OK");
  
  // 14 -- Error/Alarm
  
  //if((MSB & 0x40)> 0x00) Serial.print("ER");
  
  // Elimino los  FLAGS
  
  MSB = MSB & 0x3F; // MSB & 00111111
  valor = int(word(MSB,LSB)); // Compongo la palabra
  // 13 es el signo
  // Si es negativo, complemento y sumo 1 y le pongo el signo
  if (valor>8192) valor = (16384-valor+1)*(-1);
  
  // Este valor habrá que multiplicarlo por 0.025 para obtener el valor en Grados
  return valor; 
}



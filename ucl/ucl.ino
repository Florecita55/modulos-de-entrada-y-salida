#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Wire.h> 
#include <Keypad.h> //libreria para utilizar el teclado.


//configuracion del teclado
const byte filas = 4;
const byte columnas = 4;
int num;

char codigo[3];
int k;
long tiempoultimocambio;

char ncodigo[4];
int posicionActual = 0;
char teclas[filas][columnas] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

SoftwareSerial ucs(2, 3);   // se crea un objeto con parametros 2 y 3 que son los pines del arduino.


byte pinesFila[filas] = {A0,A1,A2,A3};
byte pinesCol[columnas] = {4,5,6,7};

Keypad keypad = Keypad(makeKeymap(teclas), pinesFila, pinesCol, filas, columnas); //se crea un objeto para controlar el teclado

LiquidCrystal lcd(13,12,11,10,9,8);

int estado;
/*
0 - Esperando codigo desde el otro arduino
1 - Esperando codigo de usuario
*/

void setup(){
  Serial.begin(9600);
  ucs.begin(9600);
  randomSeed(analogRead(0));
  num= random(100,999);
  pinMode(3,OUTPUT);
  pinMode(A4,INPUT);
  lcd.begin(16, 2);
  lcd.setCursor(1,0);  
  lcd.write("En espera");
  lcd.setCursor(3,1);
  k = 0;
}

void loop(){
  switch(estado){
    case 0: esperarArduino(); break;
    case 1: esperarCodigo(); break;
  }
}


void esperarArduino(){
  //obtine y guarda el codigo que le encia el otro arduino
  if(ucs.available()>0){
    if(k == 0){
      k++;
    }else{
    codigo[k-1] = ucs.read();
    k++;
    if(k == 4){
      for(int i = 0; i < 3; i++){
        Serial.print(codigo[i]);
      }
      Serial.println();
      lcd.begin(16, 2);
      lcd.setCursor(1,0);  
      lcd.write("Codigo:");
      lcd.setCursor(3,1);
      estado = 1;
      k = 0;
      tiempoultimocambio = millis();
    }
    }
  }
  
  if(Serial.available()>0){
    if(k == 1 || k == 0){
      k++;
    }else{
    codigo[k-2] = Serial.read();
    k++;
    if(k == 5){
      for(int i = 0; i < 3; i++){
        Serial.print(codigo[i]);
      }
      Serial.println();
      lcd.begin(16, 2);
      lcd.setCursor(1,0);  
      lcd.write("Codigo:");
      lcd.setCursor(3,1);
      estado = 1;
      k = 0;
      tiempoultimocambio = millis();
    }
    }
  }
  
}

void esperarCodigo(){
  
  if((millis() - tiempoultimocambio) > 20000){
    lcd.setCursor(1,0);
    lcd.write("Tiempo agotado   ");
    Serial.write('x');
    ucs.write('x');
    delay(3000);
    estado = 0;
    lcd.begin(16, 2);
    lcd.setCursor(1,0);  
    lcd.write("En espera");
    lcd.setCursor(3,1);
    posicionActual = 0;
  }
  //lee del teclado
  char key = keypad.getKey();
  if (int(key) != 0) {
    lcd.write(key);
    ncodigo[posicionActual]=key;
    posicionActual++;
      if (posicionActual == 3)
      {
        if(valida()==1){
          
          lcd.setCursor(1,0);
          lcd.write("Codigo Correcto :)");
          Serial.write('x');
          ucs.write('x');
          delay(3000);
          estado = 0;
          lcd.begin(16, 2);
          lcd.setCursor(1,0);  
          lcd.write("En espera");
          lcd.setCursor(3,1);
          posicionActual = 0;
        }
        else
        {
          clrs();
          lcd.setCursor(1,0);
          lcd.write("Codigo incorrecto  ");
          delay(3000);
          
          initt();
          posicionActual = 0;
        }
      }
    }  
}

void clrs(){
 lcd.setCursor(0,0);
 lcd.write("                ");
 lcd.setCursor(0,1);
 lcd.write("                ");
}
void initt(){
  clrs();
 lcd.setCursor(1,0);
 lcd.write("Codigo:");
 lcd.setCursor(3,1);
}
int valida(){
  int i;
  for (i=0;i<3;i++)
  {
    if (codigo[i]!=ncodigo[i])
    {return 0;}
  }
  return 1;
}

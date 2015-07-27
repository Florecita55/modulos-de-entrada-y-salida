#include <SoftwareSerial.h> //librería que se utiliza para comunicación serial
#include <LiquidCrystal.h>  //librería para usar el display lcd
#include <Wire.h>           // librería para los pines de comunicacion del lcd

LiquidCrystal lcd(8, 9, 10, 11 , 12, 13); //pines que utiliza el lcd
String destinos[3]={"Catedral","La Soledad","Santo Domingo"};  
int estado;                               
/*
0 espera de opcion
1 espera de notificacion de recepcion de codigo
2 bloqueo
*/
int destino;
/*
0 sin destino
5 catedral
6 soledad
7 Santo Domingo
*/
long tiempoultimocambio; // variable para el control de tiempos.
char codigo[]="000";     // variable para guardar el codigo que se envial a la unidad de llegada

SoftwareSerial ucl(2, 3);  // se crear un objeto con parametros 2 y 3 que son los pines del arduino.

void setup(){
  Serial.begin(9600);
  ucl.begin(9600);
  lcd.begin(16, 2);
  estado=0;
  destino=0;
  tiempoultimocambio=millis();
  codigo[0]='0';
  codigo[1]='0';
  codigo[2]='0';
  pinMode(5,INPUT);//Catedral
  pinMode(6,INPUT);//Soledad
  pinMode(7,INPUT);//Santo Domingo
  pinMode(4,INPUT);//Boton aceptar
  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(A2,OUTPUT);
  lcd.setCursor(0,0);
  lcd.print("Seleccione        ");
  lcd.setCursor(0,1);  
  lcd.print("Destino           ");
  delay(1000);
}

void loop(){
switch(estado)                    //Determinar el estado actual del sistema y para cada estado se ejecuta una funcion. 
{
  case 0:
  esperarOpcion();
  break;
  case 1:
  esperarNotificacion();
  break;
  case 2:
  esperarSenial();
  break;
  
}
}

void esperarOpcion()                            
{
   int ti=millis();
   int i;
   for (i=5;i<8;i++)      //recorre los pines en los que conectamos la serie de leds
   {
     if(digitalRead(i)==HIGH) // pregunta si uno de estos pines fue presionado
     {
       destino=i;             //le asigna ese pin a la variable destino.
       lcd.setCursor(0,1);    //coloca el cursor del display en la posicion 0,1
       lcd.print("Seleccionado          "); 
       lcd.setCursor(0,0);
       lcd.print(destinos[destino-5]); //escribe el nombre del destino 
       lcd.print("        ");
       tiempoultimocambio=millis();//establecer tiempo ultimo cambio
       
     }
   }
   //comparar  tiempo actual con ultimo cambio
   if((millis()-tiempoultimocambio)>20000)
   {
       tiempoultimocambio = millis();
       destino=0;
       //mostrar en display "seleccione destino";
       lcd.setCursor(0,0);
       lcd.print("Seleccione        ");
       lcd.setCursor(0,1);  
       lcd.print("Destino           ");
   }
   if(digitalRead(4)==HIGH&&(destino!=0)) //compara si el boton de aceptar se presiono y que haya un destino.
   {
   //generar codigo
   String str;
   randomSeed(tiempoultimocambio);
   int cod=random(100,999);
   str=String(cod);
   str.toCharArray(codigo,4); //convierte el string aun arreglo de caracyeres y lo guarda en la variable 'codigo'
   //mostrar codigo en display
   lcd.setCursor(0,0);
   lcd.print("Codigo:    ");
   lcd.print(codigo);
   lcd.setCursor(0,1);
   lcd.print("Guardelo      ");
     estado=1;
     tiempoultimocambio = millis();
    delay(500); // para que no entre al siguiente estado mientras sigue pulsado 
   }
  
}

void esperarNotificacion()
{
  if(digitalRead(4)==HIGH)
  {

    //enviar codigo al otro arduino
      Serial.write(codigo);
       ucl.write(codigo);
       lcd.setCursor(0,0);
       lcd.print("Ocupado          ");
       lcd.setCursor(0,1);
       lcd.print("               ");
       estado = 2;
   //encender circuito 
      switch(destino){
        case 5:
        analogWrite(A0, 255);
        break;
        case 6:
        analogWrite(A1, 255);
        break;
        case 7:
        analogWrite(A2, 255);
        break;
      }
  
  }
  
  if((millis() - tiempoultimocambio) > 20000){
    apagar();
  }

}

void esperarSenial()
{
  if(ucl.available()>0){ //compara si este arduino recibe algo
    char lectura = ucl.read(); 
    if(lectura == 'x'){   // la senial de notificacion del otro arduino que recibira sera una X
      apagar();
    }
  }
  
  if(Serial.available()>0){
    char lectura = Serial.read();
    if(lectura == 'x'){
      apagar();
    }
  }
  
}

void apagar(){
  switch(destino){
    case 5:
    analogWrite(A0, 0);
    break;
    case 6:
    analogWrite(A1, 0);
    break;
    case 7:
    analogWrite(A2, 0);
    break;
  }
  //regresa todo a como estaba.
  estado=0;
  destino=0;
  tiempoultimocambio=millis();
  codigo[0]='0';
  codigo[1]='0';
  codigo[2]='0';
  lcd.setCursor(0,0);
  lcd.print("Seleccione        ");
  lcd.setCursor(0,1);  
  lcd.print("Destino           ");
  delay(500);
}

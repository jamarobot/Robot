#include <DcMotor.h>
#include <Counter.h>
#include <SharpIR.h>

#define SENSOR_DERECHA A3
#define SENSOR_IZQUIERDA A1
#define SENSOR_FRENTE A0

//#define PULSOS_AVANCE 202
#define ENCODER 7
#define MOTOR_DER_DIR 4
#define MOTOR_DER_VEL 5
#define MOTOR_IZQ_DIR 12
#define MOTOR_IZQ_VEL 6
#define VEL_GIRO 50
#define VEL_AVANCE 50
#define model 20150
#define VELOCIDAD_MINIMA 40
#define VELOCIDAD_MAXIMA 70
#define DISTANCIA_MEDIA 40

#define DISTANCIA_MAXIMA 80
#define DELAY 10

#define DETECTA_FRENTE
//#define NAVEGAR
//#define CONTROL
#define ZOMBI
int KP = 1;
int KD = 30;
int 
//PULSOS_AVANCE= 235;
PULSOS_AVANCE= 222;
int PULSOS_GIRO =140;
Counter myCounter(ENCODER);
DcMotor IZQ;
DcMotor DER;

SharpIR SharpIR1(SENSOR_DERECHA, model); //sensor derecho
SharpIR SharpIR2(SENSOR_IZQUIERDA, model); //sensor izquierda
SharpIR SharpIR3(SENSOR_FRENTE, model); // sensor delantero

bool estado = false;
bool encendido = true;
long oldValue = 0;
long newValue=0;

int orientacion = 0;
int necesario = 0;
void setup() {
  DER.begin(MOTOR_DER_DIR, MOTOR_DER_VEL, INVERSE);
  IZQ.begin(MOTOR_IZQ_DIR, MOTOR_IZQ_VEL, INVERSE);


  // Espera antes de encender el modulo
  Serial.begin(9600);
  Serial1.begin(9600);
}
void loop() {
  #ifdef ZOMBI
  modoZombie();
  #endif
  #ifdef CONTROL
  if (Serial1.available()) {
    estado = !estado;

    char data = Serial1.read();
    Serial1.println(data);
    switch (data)
    {
      	case 'd':
        	girarDerecha();        
        	break;
      	case 'a':
        	girarIzquierda();
        	break;
		case 'w':
        	avanzar();
        	break;
      	case 'D':
        	girarNecesario(90);
        	break;
      	case 'A':
        	girarNecesario(270);
        	break;
      	case 'W':
			girarNecesario(0);
        	break;
      	case 's':
        	Serial1.println(hayParedIzquierda());
        	Serial1.println(hayParedDerecha());
        	break;
      	case 'r':
        	runPath("DWWAWDWAWAWDWDW");
        	break;
      	case 'p':
        	navegar();
        	break;
        case 's':
          der.stop();
          izq.stop();
          break;
		    case 'c':
			    corregir();
		    break;
    }
  }
#endif
#ifdef NAVEGAR
    navegar(); 
#endif
}


bool hayParedDerecha() {
  //SharpIR SharpIR(SENSOR_DERECHA, model);
  int dis = SharpIR1.distance();
  return dis < 100; //La distacia de frente puede ser menor
}
int distDerecha() {
  return SharpIR1.distance(); //recogida de datos de sensor derecho
}

int distIzquierda() {
  return SharpIR2.distance(); //recogida de datos de sensor izquierdo
}
bool hayParedIzquierda() {
  int dis = SharpIR2.distance();
  return dis < 100;
}
bool hayParedFrente() {
  int dis = SharpIR3.distance();
  return dis < DISTANCIA_MAXIMA;
 // return false;
}
int distFrente() {
  return SharpIR3.distance(); //recogida de datos de sensor delantero
}

/*
   Metodo para que gire a la derecha utilizando metodos move de la libreria
   se pasa la orientacion y la velocidad de giro
   Gira a la derecha mientras leer datos del counter, seguira mientras los datos recogidos sean menores 
   que los datos anteriores (declarado con 0) y los datos de pulsos de giro
*/
void girarDerecha() {
  orientacion += (orientacion == 270) ? -270 : 90;

  /* if(orientacion==270){
     orientacion=orientacion-270;
    }else{
     orientacion=orientacion+90;
    }
  */
  do {
    IZQ.move(FORWARD, VEL_GIRO); // direccion y velocidad
    DER.move(BACKWARD, VEL_GIRO);
    newValue = myCounter.read(); //lectura de datos del encoder
    Serial.println(newValue);

  } while (newValue < oldValue + PULSOS_GIRO);  
  oldValue = newValue;
  DER.stop();
  IZQ.stop();

}


/*
   Metodo para que gire a la izquierda utilizando metodos move de la libreria
   se pasa la orientacion y la velocidad de giro
*/
void girarIzquierda() {
  orientacion -= (orientacion == 0) ? -270 : 90;
  do {
    DER.move(FORWARD, VEL_GIRO);
    IZQ.move(BACKWARD, VEL_GIRO);
    newValue = myCounter.read();
    Serial.println(newValue);
  } while (newValue < oldValue + PULSOS_GIRO);
  oldValue = newValue;
  DER.stop();
  IZQ.stop();

}

/*
   Metodo para avanzar
*/
void avanzar() {
  do {

    
    if (!navegar()) {
      break;
    }
    newValue = myCounter.read();
    //Serial1.println(newValue);

  } while (newValue < oldValue+ PULSOS_AVANCE);
  oldValue = newValue;
  DER.stop();
  IZQ.stop();
}
/*

*/
void runPath(String path) {
  for (int i = 0; i < path.length(); i++) {
    switch (path.charAt(i))
    {
      case 'D':
        girarDerecha();//esto lo cambiaremos por girar necesario
        break;
      case 'A':
        girarIzquierda();
        break;
      case 'W':
        avanzar();
        break;
    }
    delay(300);
  }
}

/*
   Metodo para girar de forma optimizada, se paran los parametros de orientacion orginal y necesario.
   Delta es la diferecia, la orientcion final
*/
void girarNecesario(int need) {

  if (contarGirosDerecha(orientacion, need) > 2) {
    while (orientacion != need) {
      girarIzquierda();
      // Serial1.println(orientacion);
    }
  } else {
    while (orientacion != need) {
      girarIzquierda();
      //Serial1.println(orientacion);
    }
  }
}

int contarGirosDerecha( int origen, int fin) {
  int giros = 0;
  while (origen != fin) {
    origen += 90;
    if (origen == 360) {
      origen = 0;
    }
    giros++;
  }
  Serial1.print("tengo que hacer ");
  Serial1.println(giros);
  return giros;
}



int error = 0;
//int error_anterior;
int vel_der = VEL_AVANCE;
int vel_izq = VEL_AVANCE;
int desfase;

/*
   Metodo principal para navegar por el laberinto
*/
void controlPD(int der, int izq) {
  int error_anterior = error;
  //Serial.println((String)"Der: " + der + "\tizq: " + izq);
  error = der - izq;
  Serial.println(error);
//  error = constrain(error, -40, 40);
  desfase = (KP * error + KD * (error - error_anterior));
  //desfase = constrain(desfase, -10, 10);
  vel_izq = constrain(VEL_AVANCE + desfase, VELOCIDAD_MINIMA, VELOCIDAD_MAXIMA); //si la velocidad mas desfase va a ser mayor que 255 la limito
  vel_der = constrain(VEL_AVANCE - desfase, VELOCIDAD_MINIMA, VELOCIDAD_MAXIMA);

  //vel_izq = VEL_AVANCE + desfase; //si la velocidad mas desfase va a ser mayor que 255 la limito
  //vel_der = VEL_AVANCE - desfase;
  Serial.println((String)"Der: " + vel_der + "\tizq: " + vel_izq);
  DER.move(vel_der);
  IZQ.move(vel_izq);
  delay(DELAY);
}
bool navegar() {
  if (hayParedFrente()) {
    #ifdef DETECTA_FRENTE
      DER.stop();
      IZQ.stop();
    #endif
    //girarIzquierda();
    //girarDerecha();
    //girarDerecha();
    return false;
    
    
  } else {
    if (hayParedDerecha() && hayParedIzquierda()) {
      controlPD(distDerecha(), distIzquierda());
    } else if (!hayParedDerecha()) {
      controlPD(DISTANCIA_MEDIA, distIzquierda());
    } else if (!hayParedIzquierda()) {
      controlPD(distDerecha(), DISTANCIA_MEDIA);
    } else {
     controlPD(DISTANCIA_MEDIA, DISTANCIA_MEDIA);
    }
    return true;
  }
}


int charToDirection(char dir) {
  switch (dir)
  {
    case 'W':
      return 0;
    case 'D':
      return 90;
    case 'A':
      return 270;
    case 'S':
      return 180;
  }

}
void corregir(){
	String data=Serial1.readStringUntil(';');
	int length=data.length();
	int numero=data.substring(1,length).toInt();//-2 para obviar la coma
	switch (data.charAt(0))
  {
  case 'p' :
    KP=numero;
    break;
  case 'd' :
    KD=numero;
    break;
  case 'a' :
    PULSOS_AVANCE=numero;
    break;
    case 'g' :
    PULSOS_GIRO=numero;
    break;
  }
}
  void modoZombie(){
  
      if(!hayParedDerecha()){
        girarDerecha();
        delay(300);
      }else if(hayParedFrente()){
        girarIzquierda();
        delay(300);
      }
      avanzar();
      delay(300);
  }



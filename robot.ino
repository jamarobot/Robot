#include <DcMotor.h>
#include <Counter.h>
#include <SharpIR.h>

#define SENSOR_DERECHA A3
#define SENSOR_IZQUIERDA A1
#define SENSOR_FRENTE A0
#define PULSOS_GIRO 139
#define PULSOS_AVANCE 202
#define ENCODER 7

#define MOTOR_DER_DIR 4
#define MOTOR_DER_VEL 5
#define MOTOR_IZQ_DIR 12
#define MOTOR_IZQ_VEL 6
#define VEL_GIRO 50
#define VEL_AVANCE 50
#define model 20150
#define VELOCIDAD_MINIMA 25
#define VELOCIDAD_MAXIMA 50
#define DISTANCIA_MEDIA 50
#define COMPLETO
int KP = 30;
int KD = 220;


Counter myCounter(ENCODER);
DcMotor IZQ;
DcMotor DER;

SharpIR SharpIR1(SENSOR_DERECHA, model);
SharpIR SharpIR2(SENSOR_IZQUIERDA, model);
SharpIR SharpIR3(SENSOR_FRENTE, model);

bool estado = false;
bool encendido = true;
long oldValue = 0;
long newValue;

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
  #ifdef COMPLETO
  if (Serial1.available()) {
    estado = !estado;

#ifdef COMPLETO
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
          runPath("WWADW");
          break;
        case 'p':
          navegar();
          break;
      }
      estado = false;
    }
#endif
#ifdef NAVEGAR
    navegar();
#endif
  }
}

bool hayParedDerecha() {
  //SharpIR SharpIR(SENSOR_DERECHA, model);
  int dis = SharpIR1.distance();
  return dis < 100; //La distacia de frente puede ser menor
}
int distDerecha() {
  return SharpIR1.distance();
}

int distIzquierda() {
  return SharpIR2.distance();
}
bool hayParedIzquierda() {
  int dis = SharpIR2.distance();
  return dis < 100;
}
bool hayParedFrente() {
  int dis = SharpIR3.distance();
  return dis < 50;
}
int distFrente() {
  return SharpIR3.distance();
}

/*
   Metodo para que gire a la derecha utilizando metodos move de la libreria
   se pasa la orientacion y la velocidad de giro
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
    IZQ.move(FORWARD, VEL_GIRO);
    DER.move(BACKWARD, VEL_GIRO);
    newValue = myCounter.read();


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

    //DER.move(FORWARD, VEL_GIRO);
    //IZQ.move(FORWARD, VEL_GIRO);
    if (!navegar()) {
      break;
    }
    newValue = myCounter.read();
    //Serial1.println(newValue);

  } while (newValue < oldValue + PULSOS_AVANCE);
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
  Serial1.println(error);
  error = der - izq;
  error = constrain(error, -10, 10);
  desfase = (KP * error + KD * (error - error_anterior));
  desfase = constrain(desfase, -10, 10);
  vel_izq = constrain(vel_izq + desfase, VELOCIDAD_MINIMA, VELOCIDAD_MAXIMA); //si la velocidad mas desfase va a ser mayor que 255 la limito
  vel_der = constrain(vel_der - desfase, VELOCIDAD_MINIMA, VELOCIDAD_MAXIMA);
  DER.move(vel_der);
  IZQ.move(vel_izq);

}
bool navegar() {
  if (hayParedFrente()) {
    DER.stop();
    IZQ.stop();
    return false;
    //girarDerecha();
    //girarDerecha();
  } else {
    if (hayParedDerecha() && hayParedIzquierda()) {
      controlPD(distDerecha(), distIzquierda());
    } else if (!hayParedDerecha()) {
      controlPD(DISTANCIA_MEDIA, distIzquierda());
    } else if (!hayParedIzquierda()) {
      controlPD(distDerecha(), DISTANCIA_MEDIA);
    } else {
      avanzar();
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

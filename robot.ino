#include <DcMotor.h>
#include <Counter.h>
#include <SharpIR.h>

#define SENSOR_DERECHA A3
#define SENSOR_IZQUIERDA A1
#define SENSOR_FRENTE A0
#define PULSOS_GIRO 148
#define PULSOS_AVANCE 200
#define ENCODER 7
#define MOTOR_DER_DIR 4
#define MOTOR_DER_VEL 5
#define MOTOR_IZQ_DIR 12
#define MOTOR_IZQ_VEL 6
#define VEL_GIRO 50
#define VEL_AVANCE 50
#define model 20150
Counter myCounter(ENCODER);
DcMotor IZQ;
DcMotor DER;

SharpIR SharpIR1(SENSOR_DERECHA, model);
SharpIR SharpIR2(SENSOR_IZQUIERDA, model);
SharpIR SharpIR3(SENSOR_FRENTE, model);

bool estado=false;
bool encendido=true;
long oldValue  = 0;
long newValue;

int orientacion=0;
void setup(){
    DER.begin(MOTOR_DER_DIR, MOTOR_DER_VEL,INVERSE);
    IZQ.begin(MOTOR_IZQ_DIR, MOTOR_IZQ_VEL,INVERSE);
    pinMode(LED_BUILTIN,OUTPUT);

   
               // Espera antes de encender el modulo
    Serial.begin(9600);
    Serial1.begin(9600);
}
void loop(){
     if (Serial1.available()){
        estado=!estado;
        
        char data=Serial1.read();
        Serial1.println(data);
        switch (data)
        {
        case 'd':
            girarDerecha();
            break;
        
        case 'i':
            girarIzquierda();
            break;
        case 'a':
            avanzar();
            break;
        case 's':
            Serial1.println(paredIzquierda());
            Serial1.println(paredDerecha());
            break;
        case 'r':
            runPath("WWADW");
            break;

        }
        estado=false;
    }
}
    /*if(estado){
        DER.goForward();
        IZQ.goBackward();
        digitalWrite(LED_BUILTIN,HIGH);
    }else{
        digitalWrite(LED_BUILTIN,LOW);
        DER.stop();
        IZQ.stop();
    }
    //digitalWrite(LED_BUILTIN, (encendido)?HIGH:LOW);
    //encendido=!encendido;
    //delay(500);*/

/*long newValue = myCounter.read();
  if (newValue != oldValue) {
    oldValue = newValue;
    Serial.println(newValue);
  }*/

bool paredDerecha(){
    //SharpIR SharpIR(SENSOR_DERECHA, model);
    int dis=SharpIR1.distance();
    return dis<100;
}
int distDerecha(){
    return SharpIR1.distance();
}

int distIzquierda(){
    return SharpIR2.distance();
}
bool paredIzquierda(){
    int dis=SharpIR2.distance();
    return dis<100;
}
bool paredFrente(){
    int dis=SharpIR3.distance();
    return dis<100;
}
int distFrente(){
    return SharpIR3.distance();
}
void girarDerecha(){
    orientacion+=(orientacion==270)?-270:90;
    do{
            IZQ.move(FORWARD,VEL_GIRO);
            DER.move(BACKWARD,VEL_GIRO);
            newValue=myCounter.read();
            //Serial1.println(newValue);
            
    }while(newValue<oldValue+PULSOS_GIRO);
    oldValue=newValue;
    DER.stop();
    IZQ.stop();
    Serial1.println(orientacion);
}

void girarIzquierda(){
    orientacion-=(orientacion==0)?-270:90;
    do{
        DER.move(FORWARD,VEL_GIRO);
        IZQ.move(BACKWARD,VEL_GIRO);
        newValue=myCounter.read();
        //Serial1.println(newValue);
            
    }while(newValue<oldValue+PULSOS_GIRO);
    oldValue=newValue;
    DER.stop();
    IZQ.stop();
    Serial1.println(orientacion);
}
void avanzar(){
    do{
        DER.move(FORWARD,VEL_GIRO);
        IZQ.move(FORWARD,VEL_GIRO);
        newValue=myCounter.read();
        Serial1.println(newValue);
            
    }while(newValue<oldValue+PULSOS_AVANCE);
    oldValue=newValue;
    DER.stop();
    IZQ.stop();
}

void runPath(String path){
    for(int i=0;i<path.length();i++){
        switch (path.charAt(i))
        {
        case 'D':
            girarDerecha();
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
void girarNecesario(int orig, int need){
//int desfase=orig-need;
//if(des)
}
#include <DcMotor.h>



#define MOTOR_DER_DIR 4
#define MOTOR_DER_VEL 5
#define MOTOR_IZQ_DIR 12
#define MOTOR_IZQ_VEL 6

#define BT_IN 7
#define BT_OUT 8

DcMotor IZQ;
DcMotor DER;
bool estado=false;
bool encendido=true;
void setup(){
    DER.begin(MOTOR_DER_DIR, MOTOR_DER_VEL);
    IZQ.begin(MOTOR_IZQ_DIR, MOTOR_IZQ_VEL);
    pinMode(LED_BUILTIN,OUTPUT);

   
               // Espera antes de encender el modulo
    Serial.begin(9600);
    Serial1.begin(9600);
}
void loop(){
     if (Serial1.available()){
        estado=!estado;
        Serial1.read();
             
    }
    if(estado){
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
    //delay(500);
}
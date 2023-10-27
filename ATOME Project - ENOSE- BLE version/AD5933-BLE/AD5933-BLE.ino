/******************************************************************************
 * @brief: .Ino File of the ATOME-E-NOSE
 * 
 * @author: Quentin Defrenne - Pierre Foulon- Louis Routier- Anthyme Cerveaux - Alexandre Westrelin - Bilel Hafsi
 * @version: V 3.1.0
 * @date: 27.10.2023
 * 
 * @copyright MIT Liscene
 * 
 *  Copyright (c) 2023 Intitut of electronic, microelectronics and nanotechnologies (IEMN)
 *  Copyright (c) 2023 Icam school engineering
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 * 
 *****************************************************************************/

#include <Wire.h>
//AD5933
#include "AD5933.h"
//BLE
#include "BLEServer.h"
#include "BLEDevice.h"


#define START_FREQ  (60000)
#define FREQ_INCR   (10)
#define NUM_INCR    (2)
#define REF_RESIST  (20140)

//Init millis
const unsigned long event = 1000;
long previousTime = 0;
int i0=0;
//Définition des UUID, necessaires pour la connexion BLE avec l'ESP32
#define S_UUID_ESP32 "52c414bc-ed2c-4079-9e19-1887e2188fe0" //Service
#define C_UUID_ESP32 "eda7b862-6669-4f98-8396-03a055e5b781" //Characteristic 

//UUID pour avoir l'information état graphique (lancer l'acquisition)
#define S_UUID_Graph "f2308311-0637-4055-95e7-00f1075fb084" //Service
#define C_UUID_Graph "e6f5afa0-36b2-472c-b625-294972430899" //Characteristic

//UUID pour avoir l'information du sampling
#define S_UUID_Sampling "cc76c782-3e95-43e7-be5f-315f3b9d5cb6" //Service
#define C_UUID_Sampling "8212691f-12a5-4267-9dff-78f6ec168ccc" //Characteristic


//Définition des actions possibles sur la characteristic de l'ESP32
BLECharacteristic MyCharac_ESP32(C_UUID_ESP32,
                  BLECharacteristic::PROPERTY_READ |
                  BLECharacteristic::PROPERTY_WRITE);

BLECharacteristic MyCharac_Graph(C_UUID_Graph,
                  BLECharacteristic::PROPERTY_READ |
                  BLECharacteristic::PROPERTY_WRITE);

BLECharacteristic MyCharac_Sampling(C_UUID_Sampling,
                  BLECharacteristic::PROPERTY_READ |
                  BLECharacteristic::PROPERTY_WRITE);                  

double gain[NUM_INCR+1];
int phase[NUM_INCR+1];
long clockSpeed = 16000000;

//Definir les sorties de l'ESP32 pour le MUX
#define EN_GPIO 19
#define S0_GPIO 18
#define S1_GPIO 5
#define S2_GPIO 4
#define S3_GPIO 2
#define RST_GPIO 34

//assign digital pins
int pinCount;
long s;
int o;

long k1;
long k2;
long k3;
long k4;
long k5;
long k6;
long k7;
long k8;
long k9;
long k10;
long k11;
long k12;
long k13;
long k14;
long k15;
String Sk1;
String Sk2;
String Sk3;
String Sk4;
String Sk5;
String Sk6;
String Sk7;
String Sk8;
String Sk9;
String Sk10;
String Sk11;
String Sk12;
String Sk13;
String Sk14;
String Sk15;
String Tps;
long *PtTps = NULL;
int EtatAcq=0;
int EtatAcqBLE=0;
String SCUUID_Graph=""; //String de la caractéristique ble graphique
int dataTemps = 0;
String SdataTemps;
String SEtatAcq;
String SCUUID_Sampling=""; //String de la caractéristique ble Sampling
int Sampling=2000;


void setup(void)
{
  // Begin I2C
  Wire.begin();

  // Begin serial at 9600 baud for output
  Serial.begin(9600);
  //Serial.println("AD5933 Test Started!");
 
 //Set GPIO(output) for ESP32
 pinMode (EN_GPIO, OUTPUT);//EN
 pinMode (S0_GPIO, OUTPUT);//S0
 pinMode (S1_GPIO, OUTPUT);//S1
 pinMode (S2_GPIO, OUTPUT);//S2
 pinMode (S3_GPIO, OUTPUT);//S3
 pinMode (RST_GPIO, OUTPUT);
  digitalWrite (EN_GPIO, HIGH);
  digitalWrite(S0_GPIO, LOW);
  digitalWrite(S1_GPIO, LOW);
  digitalWrite(S2_GPIO, LOW);
  digitalWrite(S3_GPIO, LOW);
  digitalWrite (RST_GPIO, HIGH);
  
 bypass();
 out0();
 AD5933::setControlMode(0x02);

  // Perform initial configuration. Fail if any one of these fail.
  if (!(AD5933::reset() &&
        AD5933::setInternalClock(false) &&
        AD5933::setStartFrequency(START_FREQ) &&
        AD5933::setIncrementFrequency(FREQ_INCR) &&
        AD5933::setNumberIncrements(NUM_INCR) &&
        AD5933::setPGAGain(PGA_GAIN_X1)))
        {
            //Serial.println("FAILED in initialization!");
            while (true) ;
        }

  // Perform calibration sweep at 986
  if (AD5933::calibrate(gain, phase, REF_RESIST, NUM_INCR+1)){
    Serial.println("Calibrated!");
    }
  else
  {
   // Serial.println("Calibration failed...");
   }


//BLE
//create BLE Device
BLEDevice::init("ATOME");

//Create BLE Server
BLEServer *MyServer = BLEDevice::createServer();
//MyServer->setCallbacks(new EtatServeur());//etat ble moniteursérie

//Créer le Service
BLEService *MyServiceESP32 = MyServer->createService(S_UUID_ESP32);
BLEService *MyServiceGraph = MyServer->createService(S_UUID_Graph);
BLEService *MyServiceSampling = MyServer->createService(S_UUID_Sampling);

//Ajout de la Characteristic au service (characteristique definie avant le setup)
MyServiceESP32->addCharacteristic(& MyCharac_ESP32);
MyServiceGraph->addCharacteristic(& MyCharac_Graph);
MyServiceSampling->addCharacteristic(& MyCharac_Sampling);

//Start
Serial.println("lancement des services");
MyServiceESP32->start();
MyServiceGraph->start();
MyServiceSampling->start();

Serial.println("lancement du serveur");
MyServer->getAdvertising()->start();
}



void loop(void)
{
delay(200);

if (i0 ==0) {
  lecture();
  i0 = 1 ;
}

else {
//Init
Serial.println ("Lancer operation ? 1.Oui,lancer. 2.Annuler");
EtatAcq=0;
EtatAcqBLE=0;

//Set 0 pour donner l'information au smartphone que l'acquisition n'est pas lancée. 
MyCharac_Graph.setValue("0");
MyCharac_Graph.notify();

// attente de la commande de lecture d'impédance
while (EtatAcq==0){               //Tant que l'acquisition n'est pas lancée
  if (Serial.available()>0){      //Attendre la commande du serial read ou de l'application PC Matlab
    EtatAcq=Serial.parseInt();    //Si une valeur est entrée par l'utilisateur, récupérer cette valeur 
  }
  else {                                              //Si aucune valeur n'est entrée par l'utilisateur PC
    std::string value = MyCharac_Graph.getValue();    //l'ESP vient lire la valeur de la caractéristique Graph.
    SCUUID_Graph=value.c_str();                       //La valeur reçue est un string,
    EtatAcq=SCUUID_Graph.toInt();                     //il faut la convertir en int.
  }
}                                                     //On sort de la boucle quand EtatAcq!=0

Serial.print("x = ");
Serial.println (EtatAcq);
digitalWrite (RST_GPIO, HIGH);

    
              switch(EtatAcq)
              {
                case 1: Serial.println ("Opération lancee");//EtatAcq = 1
                while (EtatAcq==1)
                {     
                  lecture(); //Fait tourner la fonction de lecture jusqu'à ce qu'on l'arrête
                  Serial.available(); //si existante, lis la valeur du serial moniteur ou de l'application PC Matlab
                  //BLE get Graph information, permet de savoir si on veut arrêter l'acquisition depuis l'app tel
                  std::string value = MyCharac_Graph.getValue();
                  SCUUID_Graph=value.c_str();
                  EtatAcqBLE=SCUUID_Graph.toInt();
                  //BLE get Sampling information, permet de régler le delay entre 2 lecture()
                  std::string value = MyCharac_Sampling.getValue();
                  SCUUID_Sampling=value.c_str();
                  Sampling=SCUUID_Sampling.toInt();
              if (Serial.parseInt()==99 or EtatAcqBLE==99)//Si on veut arrêter l'acquisition 
                  {
                  Serial.println("reset");                //Reset des variables
                  EtatAcq=0; 
                  EtatAcqBLE=0;
                  MyCharac_Graph.setValue("0");
                  MyCharac_Graph.notify();   
                  lecture();          
                  RSTBLE();
                  delay(200);
                  digitalWrite (RST_GPIO, LOW);
                  }
                }
                break;

                
                case 2: 
                Serial.println ("Operation annulee");
                EtatAcq=0;
                EtatAcqBLE=0;
                break;

                case 3:
                RSTBLE();
                break;

             
                default:char dummy=Serial.read();//Récupère les caractères "parasites" du buffer Serial pour le vider
                Serial.println("Pour lancez saisir 1 ,  pour annuler saisir 2.");
                EtatAcq=0;
                EtatAcqBLE=0;
                break;
              }            



//long currentTime = millis();
//if (currentTime - previousTime >= event)//Millis
//{
//lecture();
//previousTime = currentTime; 
//PtTps = &currentTime;

}

}


void lecture(){
bypass;


out1();
o=1;
frequencySweepRaw();
bypass;


out2();
o=2;
frequencySweepRaw();
bypass;

out3();
o=3;
frequencySweepRaw();
bypass;

out4();
o=4;
frequencySweepRaw();
bypass;

out5();
o=5;
frequencySweepRaw();
bypass;

out6();
o=6;
frequencySweepRaw();
bypass;

out7();
o=7;
frequencySweepRaw();
bypass;

out8();
o=8;
frequencySweepRaw();
bypass;

out9();
o=9;
frequencySweepRaw();
bypass;

out10();
o=10;
frequencySweepRaw();
bypass;

out11();
o=11;
frequencySweepRaw();
bypass;

out12();
o=12;
frequencySweepRaw();
bypass;

out13();
o=13;
frequencySweepRaw();
bypass;

out14();
o=14;
frequencySweepRaw();
bypass;

out15();
o=15;
frequencySweepRaw();
bypass;


//Tps = (String)*PtTps;
// affichage adapté à l'interface
  Serial.print("X");
  Serial.print(",");
  Serial.print(k1);
  Serial.print(",");
  Serial.print(k2);
  Serial.print(",");
  Serial.print(k3);
  Serial.print(",");
  Serial.print(k4);
  Serial.print(",");
  Serial.print(k5);
  Serial.print(",");
  Serial.print(k6);
  Serial.print(",");
  Serial.print(k7);
  Serial.print(",");
  Serial.print(k8);
  Serial.print(",");
  Serial.print(k9);
  Serial.print(",");
  Serial.print(k10);
  Serial.print(",");
  Serial.print(k11);
  Serial.print(",");
  Serial.print(k12);
  Serial.print(",");
  Serial.print(k13);
  Serial.print(",");
  Serial.print(k14);
  Serial.print(",");
  Serial.println(k15);
  //Serial.print(",");
  //Serial.println (1);

dataTemps = esp_timer_get_time();   //Obtient le temps grâce à la clock interne de l'ESP32
dataTemps = dataTemps/1000000;      //Convertion en s
SdataTemps = (String)dataTemps;     //Convertion en String
SEtatAcq = (String)EtatAcq;


  String DATA = "X," + SEtatAcq + "," + SdataTemps + "," + Sk1 + "," + Sk2 + "," + Sk3 + "," + Sk4 + "," + Sk5 + "," + Sk6 + "," + Sk7 + "," + Sk8 + "," + Sk9 + "," + Sk10 + "," + Sk11 + "," + Sk12 + "," + Sk13 + "," + Sk14 + "," + Sk15;
  //Serial.println((String)DATA);
  MyCharac_ESP32.setValue(DATA.c_str());
  MyCharac_ESP32.notify();                //Envoie le String DATA au telephone
}

// Easy way to do a frequency sweep. Does an entire frequency sweep at once and
// stores the data into arrays for processing afterwards. This is easy-to-use,
// but doesn't allow you to process data in real time.


// Removes the frequencySweep abstraction from above. This saves memory and
// allows for data to be processed in real time. However, it's more complex.

void frequencySweepRaw() {
 // AD5933::enableTemperature(TEMP_MEASURE);
 //Serial.print("temperature:");
// Serial.println (AD5933::getTemperature());
 
    // Create variables to hold the impedance data and track frequency
    int real, imag, i = 0, cfreq = START_FREQ/1000;
    int k = 0;

    // Initialize the frequency sweep
    if (!(AD5933::setPowerMode(POWER_STANDBY) &&          // place in standby
          AD5933::setControlMode(CTRL_INIT_START_FREQ) && // init start freq
          AD5933::setControlMode(CTRL_START_FREQ_SWEEP))) // begin frequency sweep
         {
             Serial.println("Could not initialize frequency sweep...");
         }

    // Perform the actual sweep
    while ((AD5933::readStatusRegister() & STATUS_SWEEP_DONE) != STATUS_SWEEP_DONE) {
        // Get the frequency data for this frequency point
        if (!AD5933::getComplexData(&real, &imag)) {
            Serial.println("Could not get raw frequency data...");
        }

        // Print out the frequency data
 

        // Compute impedance
        long magnitude = sqrt(pow(real, 2) + pow(imag, 2));
        long impedance = 1/(magnitude*gain[i]);
        long z=0;
         z=impedance-20000;

if (o==1){
if (z<=200000){
k1=z;
}else {
  z= z+500*exp(0.00000600*z);
  k1=z;
  }
  Sk1 = (String)k1;
}

else if (o==2){
  if (z<=200000){
 k2=z;
}else {
   z= z+500*exp(0.00000600*z);
  k2=z;
}
Sk2 = (String)k2;
}

else if (o==3){
if (z<=200000){
k3=z;
}else {
  z= z+500*exp(0.00000600*z);
  k3=z;
}
Sk3 = (String)k3;
}

else if (o==4){
  if (z<=200000){
  k4=z;
}else {
   z= z+500*exp(0.00000600*z);
  k4=z;
}
Sk4 = (String)k4;
}

else if (o==5){
  if (z<=200000){
  k5=z;
}else {
   z= z+500*exp(0.00000600*z);
  k5=z;
}
Sk5 = (String)k5;
}

else if (o==6){
  if (z<=200000){
  k6=z;
}else {
   z= z+500*exp(0.00000600*z);
  k6=z;
}
Sk6 = (String)k6;
}

else if (o==7){
  if (z<=200000){
  k7=z;
}else {
   z= z+500*exp(0.00000600*z);
  k7=z;
}
Sk7 = (String)k7;
}

else if (o==8){
  if (z<=200000){
  k8=z;
}else {
   z= z+500*exp(0.00000600*z);
  k8=z;
}
Sk8 = (String)k8;
}


else if (o==9){
  if (z<=200000){
 k9=z;
}else {
   z= z+500*exp(0.00000600*z);
  k9=z;
}
Sk9 = (String)k9;
}

else if (o==10){
if (z<=200000){
k10=z;
}else {
   z= z+500*exp(0.00000600*z);
  k10=z;
}
Sk10 = (String)k10;
}

else if (o==11){
  if (z<=200000){
  k11=z;
}else {
   z= z+500*exp(0.00000600*z);
  k11=z;
}
Sk11 = (String)k11;
}

else if (o==12){
  if (z<=200000){
  k12=z;
}else {
   z= z+500*exp(0.00000600*z);
  k12=z;
}
Sk12 = (String)k12;
}

else if (o==13){
  if (z<=200000){
  k13=z;
}else {
   z= z+500*exp(0.00000600*z);
  k13=z;
}
Sk13 = (String)k13;
}

else if (o==14){
  if (z<=200000){
  k14=z;
}else {
   z= z+500*exp(0.00000600*z);
  k14=z;
}
Sk14 = (String)k14;
}

else if (o==15){
  if (z<=200000){
  k15=z;
}else {
   z= z+500*exp(0.00000600*z);
  k15=z;
}
Sk15 = (String)k15;
}

        // Increment the frequency
        i++;
        cfreq = FREQ_INCR/1000;
        AD5933::setControlMode(CTRL_INCREMENT_FREQ);
    }

//correction


    // Set AD5933 power mode to standby when finished
    if (!AD5933::setPowerMode(POWER_STANDBY))
        Serial.println("Could not set to standby...");
}
void out0()
{
 digitalWrite (EN_GPIO, LOW);
 digitalWrite (S0_GPIO, LOW);
 digitalWrite (S1_GPIO, LOW);
 digitalWrite (S2_GPIO, LOW);
 digitalWrite (S3_GPIO, LOW);
}
void out1()
{
 digitalWrite (EN_GPIO, LOW);
 digitalWrite (S0_GPIO, HIGH);
 digitalWrite (S1_GPIO, LOW);
 digitalWrite (S2_GPIO, LOW);
 digitalWrite (S3_GPIO, LOW);
}
void out2()
{
 digitalWrite (EN_GPIO, LOW);
 digitalWrite (S0_GPIO, LOW);
 digitalWrite (S1_GPIO, HIGH);
 digitalWrite (S2_GPIO, LOW);
 digitalWrite (S3_GPIO, LOW);
}
void out3()
{
 digitalWrite (EN_GPIO, LOW);
 digitalWrite (S0_GPIO, HIGH);
 digitalWrite (S1_GPIO, HIGH);
 digitalWrite (S2_GPIO, LOW);
 digitalWrite (S3_GPIO, LOW);
}
void out4()
{
 digitalWrite (EN_GPIO, LOW);
 digitalWrite (S0_GPIO, LOW);
 digitalWrite (S1_GPIO, LOW);
 digitalWrite (S2_GPIO, HIGH);
 digitalWrite (S3_GPIO, LOW);
}
void out5()
{
  digitalWrite (EN_GPIO, LOW);
 digitalWrite (S0_GPIO, HIGH);
 digitalWrite (S1_GPIO, LOW);
 digitalWrite (S2_GPIO, HIGH);
 digitalWrite (S3_GPIO, LOW);
}
void out6()
{
 digitalWrite (EN_GPIO, LOW);
 digitalWrite (S0_GPIO, LOW);
 digitalWrite (S1_GPIO, HIGH);
 digitalWrite (S2_GPIO, HIGH);
 digitalWrite (S3_GPIO, LOW);
}
void out7()
{
 digitalWrite (EN_GPIO, LOW);
 digitalWrite (S0_GPIO, HIGH);
 digitalWrite (S1_GPIO, HIGH);
 digitalWrite (S2_GPIO, HIGH);
 digitalWrite (S3_GPIO, LOW);
}
void out8()
{
 digitalWrite (EN_GPIO, LOW);
 digitalWrite (S0_GPIO, LOW);
 digitalWrite (S1_GPIO, LOW);
 digitalWrite (S2_GPIO, LOW);
 digitalWrite (S3_GPIO, HIGH);
}
void out9()
{
 digitalWrite (EN_GPIO, LOW);
 digitalWrite (S0_GPIO, HIGH);
 digitalWrite (S1_GPIO, LOW);
 digitalWrite (S2_GPIO, LOW);
 digitalWrite (S3_GPIO, HIGH);
}
void out10()
{
 digitalWrite (EN_GPIO, LOW);
 digitalWrite (S0_GPIO, LOW);
 digitalWrite (S1_GPIO, HIGH);
 digitalWrite (S2_GPIO, LOW);
 digitalWrite (S3_GPIO, HIGH);
}
void out11()
{
 digitalWrite (EN_GPIO, LOW);
 digitalWrite (S0_GPIO, HIGH);
 digitalWrite (S1_GPIO, HIGH);
 digitalWrite (S2_GPIO, LOW);
 digitalWrite (S3_GPIO, HIGH);
}
void out12()
{
 digitalWrite (EN_GPIO, LOW);
 digitalWrite (S0_GPIO, LOW);
 digitalWrite (S1_GPIO, LOW);
 digitalWrite (S2_GPIO, HIGH);
 digitalWrite (S3_GPIO, HIGH);
}
void out13()
{
 digitalWrite (EN_GPIO, LOW);
 digitalWrite (S0_GPIO, HIGH);
 digitalWrite (S1_GPIO, LOW);
 digitalWrite (S2_GPIO, HIGH);
 digitalWrite (S3_GPIO, HIGH);
}
void out14()
{
 digitalWrite (EN_GPIO, LOW);
 digitalWrite (S0_GPIO, LOW);
 digitalWrite (S1_GPIO, HIGH);
 digitalWrite (S2_GPIO, HIGH);
 digitalWrite (S3_GPIO, HIGH);
}
void out15()
{
 digitalWrite (EN_GPIO, LOW);
 digitalWrite (S0_GPIO, HIGH);
 digitalWrite (S1_GPIO, HIGH);
 digitalWrite (S2_GPIO, HIGH);
 digitalWrite (S3_GPIO, HIGH);
}
void bypass()
{
 digitalWrite (EN_GPIO, HIGH);
}

void RSTBLE()
{
//create BLE Device
BLEDevice::init("ATOME");

//Create BLE Server
BLEServer *MyServer = BLEDevice::createServer();
//MyServer->setCallbacks(new EtatServeur());//etat ble moniteursérie

//Créer le Service
BLEService *MyServiceESP32 = MyServer->createService(S_UUID_ESP32);
BLEService *MyServiceGraph = MyServer->createService(S_UUID_Graph);

//Ajout de la Characteristic au service (characteristique definie avant le setup)
MyServiceESP32->addCharacteristic(& MyCharac_ESP32);
MyServiceGraph->addCharacteristic(& MyCharac_Graph);

//Start
Serial.println("lancement des services");
MyServiceESP32->start();
MyServiceGraph->start();

Serial.println("lancement du serveur");
MyServer->getAdvertising()->start();
}

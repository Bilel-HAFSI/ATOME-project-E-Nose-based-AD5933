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
#include "AD5933.h"

#define START_FREQ  (60000)
#define FREQ_INCR   (10)
#define NUM_INCR    (2)
#define REF_RESIST  (20140)

double gain[NUM_INCR+1];
int phase[NUM_INCR+1];
long clockSpeed = 16000000;

//assign digital pins
int EN = 2;
int S0 = 3;
int S1 = 4;
int S2 = 5;
int S3 = 6;
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


void setup(void)
{
  // Begin I2C
  Wire.begin();

  // Begin serial at 9600 baud for output
  Serial.begin(9600);
  //Serial.println("AD5933 Test Started!");
 pinMode (EN, OUTPUT);
 pinMode (S0, OUTPUT);
 pinMode (S1, OUTPUT);
 pinMode (S2, OUTPUT);
 pinMode (S3, OUTPUT);
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
  if (AD5933::calibrate(gain, phase, REF_RESIST, NUM_INCR+1))
    //Serial.println("Calibrated!");
    ;
  else
   // Serial.println("Calibration failed...");
   ;
}

void loop(void)
{

lecture();

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
}

else if (o==2){
  if (z<=200000){
 k2=z;
}else {
   z= z+500*exp(0.00000600*z);
  k2=z;
}
}

else if (o==3){
if (z<=200000){
k3=z;
}else {
  z= z+500*exp(0.00000600*z);
  k3=z;
}
}

else if (o==4){
  if (z<=200000){
  k4=z;
}else {
   z= z+500*exp(0.00000600*z);
  k4=z;
}
}

else if (o==5){
  if (z<=200000){
  k5=z;
}else {
   z= z+500*exp(0.00000600*z);
  k5=z;
}
}

else if (o==6){
  if (z<=200000){
  k6=z;
}else {
   z= z+500*exp(0.00000600*z);
  k6=z;
}
}

else if (o==7){
  if (z<=200000){
  k7=z;
}else {
   z= z+500*exp(0.00000600*z);
  k7=z;
}
}

else if (o==8){
  if (z<=200000){
  k8=z;
}else {
   z= z+500*exp(0.00000600*z);
  k8=z;
}
}


else if (o==9){
  if (z<=200000){
 k9=z;
}else {
   z= z+500*exp(0.00000600*z);
  k9=z;
}
}

else if (o==10){
if (z<=200000){
k10=z;
}else {
   z= z+500*exp(0.00000600*z);
  k10=z;
}
}

else if (o==11){
  if (z<=200000){
  k11=z;
}else {
   z= z+500*exp(0.00000600*z);
  k11=z;
}
}

else if (o==12){
  if (z<=200000){
  k12=z;
}else {
   z= z+500*exp(0.00000600*z);
  k12=z;
}
}

else if (o==13){
  if (z<=200000){
  k13=z;
}else {
   z= z+500*exp(0.00000600*z);
  k13=z;
}
}

else if (o==14){
  if (z<=200000){
  k14=z;
}else {
   z= z+500*exp(0.00000600*z);
  k14=z;
}
}

else if (o==15){
  if (z<=200000){
  k15=z;
}else {
   z= z+500*exp(0.00000600*z);
  k15=z;
}
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
 digitalWrite (EN, LOW);
 digitalWrite (S0, LOW);
 digitalWrite (S1, LOW);
 digitalWrite (S2, LOW);
 digitalWrite (S3, LOW);
}
void out1()
{
 digitalWrite (EN, LOW);
 digitalWrite (S0, HIGH);
 digitalWrite (S1, LOW);
 digitalWrite (S2, LOW);
 digitalWrite (S3, LOW);
}
void out2()
{
 digitalWrite (EN, LOW);
 digitalWrite (S0, LOW);
 digitalWrite (S1, HIGH);
 digitalWrite (S2, LOW);
 digitalWrite (S3, LOW);
}
void out3()
{
 digitalWrite (EN, LOW);
 digitalWrite (S0, HIGH);
 digitalWrite (S1, HIGH);
 digitalWrite (S2, LOW);
 digitalWrite (S3, LOW);
}
void out4()
{
 digitalWrite (EN, LOW);
 digitalWrite (S0, LOW);
 digitalWrite (S1, LOW);
 digitalWrite (S2, HIGH);
 digitalWrite (S3, LOW);
}
void out5()
{
 digitalWrite (EN, LOW);
 digitalWrite (S0, HIGH);
 digitalWrite (S1, LOW);
 digitalWrite (S2, HIGH);
 digitalWrite (S3, LOW);
}
void out6()
{
 digitalWrite (EN, LOW);
 digitalWrite (S0, LOW);
 digitalWrite (S1, HIGH);
 digitalWrite (S2, HIGH);
 digitalWrite (S3, LOW);
}
void out7()
{
 digitalWrite (EN, LOW);
 digitalWrite (S0, HIGH);
 digitalWrite (S1, HIGH);
 digitalWrite (S2, HIGH);
 digitalWrite (S3, LOW);
}
void out8()
{
 digitalWrite (EN, LOW);
 digitalWrite (S0, LOW);
 digitalWrite (S1, LOW);
 digitalWrite (S2, LOW);
 digitalWrite (S3, HIGH);
}
void out9()
{
 digitalWrite (EN, LOW);
 digitalWrite (S0, HIGH);
 digitalWrite (S1, LOW);
 digitalWrite (S2, LOW);
 digitalWrite (S3, HIGH);
}
void out10()
{
 digitalWrite (EN, LOW);
 digitalWrite (S0, LOW);
 digitalWrite (S1, HIGH);
 digitalWrite (S2, LOW);
 digitalWrite (S3, HIGH);
}
void out11()
{
 digitalWrite (EN, LOW);
 digitalWrite (S0, HIGH);
 digitalWrite (S1, HIGH);
 digitalWrite (S2, LOW);
 digitalWrite (S3, HIGH);
}
void out12()
{
 digitalWrite (EN, LOW);
 digitalWrite (S0, LOW);
 digitalWrite (S1, LOW);
 digitalWrite (S2, HIGH);
 digitalWrite (S3, HIGH);
}
void out13()
{
 digitalWrite (EN, LOW);
 digitalWrite (S0, HIGH);
 digitalWrite (S1, LOW);
 digitalWrite (S2, HIGH);
 digitalWrite (S3, HIGH);
}
void out14()
{
 digitalWrite (EN, LOW);
 digitalWrite (S0, LOW);
 digitalWrite (S1, HIGH);
 digitalWrite (S2, HIGH);
 digitalWrite (S3, HIGH);
}
void out15()
{
 digitalWrite (EN, LOW);
 digitalWrite (S0, HIGH);
 digitalWrite (S1, HIGH);
 digitalWrite (S2, HIGH);
 digitalWrite (S3, HIGH);
}
void bypass()
{
 digitalWrite (EN, HIGH);
}

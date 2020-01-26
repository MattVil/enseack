#include <Arduino.h>
#include "DHT.h"
#include "Adafruit_Sensor.h"
#include "SoftwareSerial.h"
#define SOUND_LEVELS 5

//  PINOUT

#define SOUND_1 52 
#define SOUND_2 50 
#define SOUND_3 48 
#define SOUND_4 46 
#define SOUND_5 44

#define SWITCH 3
#define GAZ A0 
#define DHT_PIN 40

#define BT_STATE 26
#define BT_RX 24
#define BT_TX 22
#define BT_ENABLE 28

 typedef struct {
   int soundLevel;
   float humidity;
   float temperature;
   int waste;
   float co;
   float co2;
   float gpl;
 } Sample;

 Sample sample;
 char buffer[40] = "";
 DHT dht(DHT_PIN, DHT22);
 int soundLevels[5];

bool switchState;

#define         MQ_PIN                       (A0)     //define which analog input channel you are going to use
#define         RL_VALUE                     (5)     //define the load resistance on the board, in kilo ohms
#define         RO_CLEAN_AIR_FACTOR          (9.83)  //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
                                                     //which is derived from the chart in datasheet

/***********************Software Related Macros************************************/
#define         CALIBRATION_SAMPLE_TIMES     (50)    //define how many samples you are going to take in the calibration phase
#define         CALIBRATION_SAMPLE_INTERVAL  (500)   //define the time interal(in milisecond) between each samples in the
                                                     //cablibration phase
#define         READ_SAMPLE_INTERVAL         (50)    //define how many samples you are going to take in normal operation
#define         READ_SAMPLE_TIMES            (5)     //define the time interal(in milisecond) between each samples in 
                                                     //normal operation

/**********************Application Related Macros**********************************/
#define         GAS_LPG                      (0)
#define         GAS_CO                       (1)
#define         GAS_SMOKE                    (2)

/*****************************Globals***********************************************/
float           LPGCurve[3]  =  {2.3,0.21,-0.47};   //two points are taken from the curve. 
                                                    //with these two points, a line is formed which is "approximately equivalent"
                                                    //to the original curve. 
                                                    //data format:{ x, y, slope}; point1: (lg200, 0.21), point2: (lg10000, -0.59) 
float           COCurve[3]  =  {2.3,0.72,-0.34};    //two points are taken from the curve. 
                                                    //with these two points, a line is formed which is "approximately equivalent" 
                                                    //to the original curve.
                                                    //data format:{ x, y, slope}; point1: (lg200, 0.72), point2: (lg10000,  0.15) 
float           SmokeCurve[3] ={2.3,0.53,-0.44};    //two points are taken from the curve. 
                                                    //with these two points, a line is formed which is "approximately equivalent" 
                                                    //to the original curve.
                                                    //data format:{ x, y, slope}; point1: (lg200, 0.53), point2: (lg10000,  -0.22)                                                     
float           Ro           =  10;                 //Ro is initialized to 10 kilo ohms



/****************** MQResistanceCalculation ****************************************
Input:   raw_adc - raw value read from adc, which represents the voltage
Output:  the calculated sensor resistance
Remarks: The sensor and the load resistor forms a voltage divider. Given the voltage
         across the load resistor and its resistance, the resistance of the sensor
         could be derived.
************************************************************************************/ 
float MQResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE*(1023-raw_adc)/raw_adc));
}

/***************************** MQCalibration ****************************************
Input:   mq_pin - analog channel
Output:  Ro of the sensor
Remarks: This function assumes that the sensor is in clean air. It use  
         MQResistanceCalculation to calculates the sensor resistance in clean air 
         and then divides it with RO_CLEAN_AIR_FACTOR. RO_CLEAN_AIR_FACTOR is about 
         10, which differs slightly between different sensors.
************************************************************************************/ 
float MQCalibration(int mq_pin)
{
  int i;
  float val=0;

  for (i=0;i<CALIBRATION_SAMPLE_TIMES;i++) {            //take multiple samples
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBRATION_SAMPLE_TIMES;                   //calculate the average value

  val = val/RO_CLEAN_AIR_FACTOR;                        //divided by RO_CLEAN_AIR_FACTOR yields the Ro 
                                                        //according to the chart in the datasheet 

  return val; 
}
/*****************************  MQRead *********************************************
Input:   mq_pin - analog channel
Output:  Rs of the sensor
Remarks: This function use MQResistanceCalculation to caculate the sensor resistenc (Rs).
         The Rs changes as the sensor is in the different consentration of the target
         gas. The sample times and the time interval between samples could be configured
         by changing the definition of the macros.
************************************************************************************/ 
float MQRead(int mq_pin)
{
  int i;
  float rs=0;

  for (i=0;i<READ_SAMPLE_TIMES;i++) {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }

  rs = rs/READ_SAMPLE_TIMES;

  return rs;  
}

/*****************************  MQGetPercentage **********************************
Input:   rs_ro_ratio - Rs divided by Ro
         pcurve      - pointer to the curve of the target gas
Output:  ppm of the target gas
Remarks: By using the slope and a point of the line. The x(logarithmic value of ppm) 
         of the line could be derived if y(rs_ro_ratio) is provided. As it is a 
         logarithmic coordinate, power of 10 is used to convert the result to non-logarithmic 
         value.
************************************************************************************/ 
float MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}

/*****************************  MQGetGasPercentage **********************************
Input:   rs_ro_ratio - Rs divided by Ro
         gas_id      - target gas type
Output:  ppm of the target gas
Remarks: This function passes different curves to the MQGetPercentage function which 
         calculates the ppm (parts per million) of the target gas.
************************************************************************************/ 
float MQGetGasPercentage(float rs_ro_ratio, int gas_id)
{
  if ( gas_id == GAS_LPG ) {
     return MQGetPercentage(rs_ro_ratio,LPGCurve);
  } else if ( gas_id == GAS_CO ) {
     return MQGetPercentage(rs_ro_ratio,COCurve);
  } else if ( gas_id == GAS_SMOKE ) {
     return MQGetPercentage(rs_ro_ratio,SmokeCurve);
  }    

  return 0;
}



void getSensorsData() {
   int a[5];
   a[0] = !digitalRead(SOUND_1);
   sprintf(buffer,"1 :  %d\n",a[0]);
  //  Serial.print(buffer);
   a[1] = !digitalRead(SOUND_2);
   sprintf(buffer,"2 :  %d\n",a[1]);
  //  Serial.print(buffer);
   a[2] = !digitalRead(SOUND_3);
   sprintf(buffer,"3 :  %d\n",a[2]);
  //  Serial.print(buffer);
   a[3] = !digitalRead(SOUND_4);
   sprintf(buffer,"4 :  %d\n",a[3]);
  //  Serial.print(buffer);
   a[4] = !digitalRead(SOUND_5);
   sprintf(buffer,"5 :  %d\n",a[4]);
  //  Serial.print(buffer);

  sample.soundLevel = 0;
  for(int i=0;i<5;i++)
    sample.soundLevel += a[i];
  sprintf(buffer,"SoundLevel :  %d\n",sample.soundLevel);
  // Serial.print(buffer);

  if (switchState){
    sample.waste = 1;
    switchState = 0;
  }
   sprintf(buffer,"switch :  %d\n",sample.waste);
  //  Serial.print(buffer);

   sample.humidity = dht.readHumidity();
   //on lit l'hygrometrie
  //  Serial.print("humidity :  ");
  //  Serial.print(sample.humidity);
  //  Serial.print("% \n");
   if (isnan(sample.humidity))
   {
     Serial.println("Failed to read from DHT sensor!");
     return;
   }
   sample.temperature = dht.readTemperature();
  //  Serial.print("temperature : ");
  //  Serial.print(sample.temperature);
  //  Serial.print("°C\n");
   if (isnan(sample.temperature))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  sample.gpl = MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_LPG)*100.0;
  sample.co = MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_CO)*100.0;
  sample.co2 = MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_SMOKE)*100.0;

  // Serial.print("gpl : ");
  // Serial.print(sample.gpl);
  // Serial.print("%\n");
  // Serial.print("co : ");
  // Serial.print(sample.co);
  // Serial.print("%\n");
  // Serial.print("smoke : ");
  // Serial.print(sample.co2);
  // Serial.print("%\n");
 }

 void sendDataBT() {
  sprintf(buffer,"%d,%d,%d,%d,%d,%d,%d",(int)round(sample.temperature),(int)round(sample.humidity),
  (int)sample.soundLevel,(int)round(sample.co),(int)round(sample.co2),(int)round(sample.gpl),(int)round(sample.waste));
  Serial2.println(buffer);
  Serial.println(buffer);
  if(sample.waste)
    sample.waste = 0;
 }

void buttonTriggered(){
  switchState = 1;
}

 void setup() {
  //  put your setup code here, to run once:
  
  Serial.begin(115200);
  Serial2.begin(115200);
  pinMode(SOUND_1,INPUT);
  pinMode(SOUND_2,INPUT);
  pinMode(SOUND_3,INPUT);
  pinMode(SOUND_4,INPUT);
  pinMode(SOUND_5,INPUT);
  pinMode(SWITCH,INPUT);
  switchState = 0;
  sample.waste = 0;
  attachInterrupt(digitalPinToInterrupt(SWITCH), buttonTriggered, FALLING);

   pinMode(GAZ,INPUT);
   dht.begin();
   Ro = MQCalibration(MQ_PIN);                       //Calibrating the sensor. Please make sure the sensor is in clean air 
   pinMode(BT_ENABLE,OUTPUT);
   digitalWrite(BT_ENABLE,HIGH);
 }

void loop() {
  //  put your main code here, to run repeatedly:
  getSensorsData();
  sendDataBT();
  // Serial.print("--------------\n");
  delay(1000);
}
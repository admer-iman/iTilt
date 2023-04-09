// START WIFI Connection 
  #include <ESP8266WiFi.h>
  #ifndef STASSID
  #define STASSID "Redmi 7" // hna diri smiya d wifi
  #define STAPSK "123456700" // code dyalo
  // nfs lwifi li ghadiri hna connecti bih 7tta l pc bax ytlia server li kayn f pc m3a ESP8266
  #endif
  // WIFI Connection *******************
  const char* ssid = STASSID;
  const char* password = STAPSK;

  const char* host = "192.168.43.219"; // hna diri IP li kitl3 lik f cmd mnin ktkhdm server
  // kitl3 lik b7al hakka :
  /* 
        * Running on all addresses (0.0.0.0)
        * Running on http://127.0.0.1:4000
        * Running on http://192.168.43.219:4000"
  */
  // l ip lkhrani howa li ghthzzih "192.168.43.219"
  const uint16_t port = 4000;
  String HTTP_METHOD = "GET";
// END WIFI Connection *******************


//define your default values here,
char tiltOffset[5] = "0";
char coefficientx3[16] = "0.0000010000000"; //model to convert tilt to gravity
char coefficientx2[16] = "-0.000131373000";
char coefficientx1[16] = "0.0069679520000";
char constantterm[16] = "0.8923835598800";
char originalgravity[6] = "1.05";

// start MPU6050 global decleration
  #include <MPU6050.h> //Instal from Arduino IDE
  #include <Wire.h>
  #define SDA_PIN D2
  #define  SCL_PIN D1
  int i2c_address=0x68;
  MPU6050 accelgyro(i2c_address);
  int16_t ax, ay, az;
// end MPU6050 global decleration

// start DS18B20 global decleration
  #include <OneWire.h>  //Instal from Arduino IDE
  #include <DallasTemperature.h> //Instal from Arduino IDE
  #define ONE_WIRE_BUS 2
  OneWire oneWire(ONE_WIRE_BUS);
  DallasTemperature sensors(&oneWire);
// end DS18B20 global decleration
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  float tilt=calcTilt(); //
  sensors.begin();  
  connectToWiFi() ;
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("caclule de ABV : ") ;
  Serial.println(calcABV()) ;
  Serial.print("caclule de Gravity : ") ;
  Serial.println(calcGrav()) ;
  Serial.print("caclule de Offset : ") ;
  Serial.println(calcOffset()) ;
  Serial.print("caclule de Temperature via dallas : ") ;
  Serial.println(calcTemp()) ;
  Serial.print("caclule de Temperature via MPU6050 : ") ;
  Serial.println(calcGyroTemp()) ;
  Serial.print("caclule de Tilt : ") ;
  Serial.println(calcTilt()) ;
  Serial.print("caclule de Roll : ") ;
  Serial.println(calcRoll()) ;
  /* End Serial Print------------------------------------------------------------ */ 
  // Start send request ***********************
  /* http://127.0.0.1:4000/send?tilt=5.5&roll=30.5&grav=87&abv=76&tem=76&intemp=52 */
    WiFiClient client;
    String PATH_NAME = "/send";
    // connect to web server on port 80:
    if(client.connect(host, port)) {
      // if connected:
      Serial.println("Connected to server");
      // make a HTTP request:
      // send HTTP header
      client.println(HTTP_METHOD+" "+PATH_NAME+"?tilt="+calcTilt()+"&roll="+ calcRoll()+"&grav="+ calcGrav()+"&abv="+ calcABV()+"&tem="+ calcTemp()+"&intemp="+ calcGyroTemp()+" HTTP/1.1");
      client.println("Host: " + String(host));
      client.println(); // end HTTP header

      while(client.connected()) {
        if(client.available()){
          // read an incoming byte from the server and print it to serial monitor:
          char c = client.read();
          // Serial.print(c);
        }
      }

      // the server's disconnected, stop the client:
      client.stop();
      Serial.println();
      Serial.println("disconnected");
    } else {// if not connected:
      Serial.println("connection failed");
    }
  // end send request ***********************
  delay(1000) ;
}
// *************************************************************************
// ----------------------- Tilt
float calcTilt()
{  
  float reading;
  float areading=0;
  float n=100;

  pinMode(SDA_PIN, OUTPUT);//This need to be done, possable conflict between WiFimanager and Wire Library (i2c). It must be before Wire.begin()
  pinMode(SCL_PIN, OUTPUT); //This need to be done, possable conflict between WiFimanager and Wire Library (i2c). It must be before Wire.begin()
  digitalWrite(SDA_PIN, LOW); //This need to be done, possable conflict between WiFimanager and Wire Library (i2c). It must be before Wire.begin()
  digitalWrite(SCL_PIN, LOW);  //This need to be done, possable conflict between WiFimanager and Wire Library (i2c). It must be before Wire.begin()
  delay(100);
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.beginTransmission(i2c_address);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
 
  accelgyro.initialize();
  //------------------------------------------------------------until not nan?
  for (int i=0;i<1000;i++)
  {
    accelgyro.getAcceleration(&ax, &az, &ay);
    reading=acos(az / (sqrt(ax * ax + ay * ay + az * az))) * 180.0 / M_PI;
    // Serial.println("You are in calcTilt(). reading="+String(reading));
    if (String(reading)!="nan")
    {
      break;
    }
    if (i==999)
    {
      Serial.println("You are in calcTilt(). The MPU6050 could not provide numerical readings for 1000 times.");
    }
  }
  
  for (int i=0; i<n; i++)
  {
    accelgyro.getAcceleration(&ax, &az, &ay);
    reading=acos(az / (sqrt(ax * ax + ay * ay + az * az))) * 180.0 / M_PI;
    areading+=(1/n)*reading;
  //    Serial.println("Calibrating: "+String(reading));
  //    Serial.println("Avarage Reading: "+String(areading));
  }
  areading+=atof(tiltOffset);
  //  Serial.println("Gyro Readings: "+String(ax)+","+String(ay)+","+String(az));
  //  Serial.println("Calculated Tilt: "+String(reading));
  return areading;

}
// -------------------------- Roll
float calcRoll()
{ 
  float reading;
  float areading=0;
  float n=100;

  pinMode(SDA_PIN, OUTPUT);//This need to be done, possable conflict between WiFimanager and Wire Library (i2c). It must be before Wire.begin()
  pinMode(SCL_PIN, OUTPUT); //This need to be done, possable conflict between WiFimanager and Wire Library (i2c). It must be before Wire.begin()
  digitalWrite(SDA_PIN, LOW); //This need to be done, possable conflict between WiFimanager and Wire Library (i2c). It must be before Wire.begin()
  digitalWrite(SCL_PIN, LOW);  //This need to be done, possable conflict between WiFimanager and Wire Library (i2c). It must be before Wire.begin()
  delay(100);
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.beginTransmission(i2c_address);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
 
  accelgyro.initialize();
  //Get Gyroscope readings untill it stoped reading nan (Not a Number)
  for (int i=0;i<1000;i++)
  {
    accelgyro.getAcceleration(&ax, &az, &ay);
    reading=acos(ax / (sqrt(ax * ax + ay * ay + az * az))) * 180.0 / M_PI;  //reading=acos(az / (sqrt(ax * ax + ay * ay + az * az))) * 180.0 / M_PI;
    // Serial.println("You are in calcRoll(). reading="+String(reading));
    if (String(reading)!="nan")
    {
      break;
    }
    if (i==999)
    {
      Serial.println("You are in calcRoll(). The MPU6050 could not provide numerical readings for 1000 times. Check Soldering or MPU6050");
    }
  }
  
  for (int i=0; i<n; i++)
  {
    accelgyro.getAcceleration(&ax, &az, &ay);
    reading=acos(ax / (sqrt(ax * ax + ay * ay + az * az))) * 180.0 / M_PI;  //reading=acos(az / (sqrt(ax * ax + ay * ay + az * az))) * 180.0 / M_PI;
    areading+=(1/n)*reading;
  //    Serial.println("Calibrating: "+String(reading));
  //    Serial.println("Avarage Reading: "+String(areading));
  }
  
  //  Serial.println("Gyro Readings: "+String(ax)+","+String(ay)+","+String(az));
  //  Serial.println("Calculated Roll: "+String(areading));
  accelgyro.setSleepEnabled(true);//-----------------------------------------------------------------------------------------------
  return areading;

}
// ----------------------- Gravity
float calcGrav()
{
  float tilt = calcTilt();
  float fcoefficientx3 = atof(coefficientx3);
  float fcoefficientx2 = atof(coefficientx2);
  float fcoefficientx1 = atof(coefficientx1);
  float fconstantterm = atof(constantterm);
  return fcoefficientx3 * (tilt * tilt * tilt) + fcoefficientx2 * tilt * tilt + fcoefficientx1 * tilt + fconstantterm;
}
// ------------------------- ABV
float calcABV()
{
  float abv = 131.258 * (atof(originalgravity) - calcGrav());
  return abv;
}
// ------------------------ Temp
float calcTemp()
{
    sensors.requestTemperatures();
    return sensors.getTempCByIndex(0);
}
// ------------------------- GyroTemp
float calcGyroTemp()
{
  pinMode(SDA_PIN, OUTPUT);//This need to be done, possable conflict between WiFimanager and Wire Library (i2c). It must be before Wire.begin()
  pinMode(SCL_PIN, OUTPUT); //This need to be done, possable conflict between WiFimanager and Wire Library (i2c). It must be before Wire.begin()
  digitalWrite(SDA_PIN, LOW); //This need to be done, possable conflict between WiFimanager and Wire Library (i2c). It must be before Wire.begin()
  digitalWrite(SCL_PIN, LOW);  //This need to be done, possable conflict between WiFimanager and Wire Library (i2c). It must be before Wire.begin()
  delay(100);
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.beginTransmission(i2c_address);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
 
  accelgyro.initialize();
  float temp=accelgyro.getTemperature()/340 +36.53;
  accelgyro.setSleepEnabled(true);
  return temp;
}
// ----------------------- Offset
float calcOffset()
{ 
  float reading;
  float areading=0;
  float n=100;

  pinMode(SDA_PIN, OUTPUT);//This need to be done, possable conflict between WiFimanager and Wire Library (i2c). It must be before Wire.begin()
  pinMode(SCL_PIN, OUTPUT); //This need to be done, possable conflict between WiFimanager and Wire Library (i2c). It must be before Wire.begin()
  digitalWrite(SDA_PIN, LOW); //This need to be done, possable conflict between WiFimanager and Wire Library (i2c). It must be before Wire.begin()
  digitalWrite(SCL_PIN, LOW);  //This need to be done, possable conflict between WiFimanager and Wire Library (i2c). It must be before Wire.begin()
  delay(100);
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.beginTransmission(i2c_address);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  accelgyro.initialize();
  for (int i=0; i<n; i++)
  {
    accelgyro.getAcceleration(&ax, &az, &ay);
    reading=acos(az / (sqrt(ax * ax + ay * ay + az * az))) * 180.0 / M_PI;
    areading+=(1/n)*reading;
    // Serial.println("Calibrating: "+String(reading));
    // Serial.println("Avarage Reading: "+String(areading));
  }
  float offset=89.0-areading;
  return offset;
}
// ----------------------------------- Connect to WIFI
void connectToWiFi(){
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
}

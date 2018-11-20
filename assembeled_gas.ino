#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <HX711_ADC.h>
#include <SoftwareSerial.h>

//initialize the LCD
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

//Key pad initiallization
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad


//byte rowPins[ROWS] = {7, 6, 5, 4}; //connect to the row pinouts of the keypad
//byte colPins[COLS] = {3, 2, 1, 0}; //connect to the column pinouts of the keypad
//


//initialize an instance of class NewKeypad
Keypad CKey = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

enum state {WAITING, LOOPING} current_state = WAITING;

//servo initialization
Servo myservo;
int mot_min = 0;   //min servo angle  (set yours)
int mot_max = 180; //Max servo angle   (set yours)


//load cell initialization
HX711_ADC LoadCell(A3, A2);

//Methane Sensor initialization
const int AOUTpin=A0;//the AOUT pin of the methane sensor goes into analog pin A0 of the arduino
const int DOUTpin=10;//the DOUT pin of the methane sensor goes into digital pin D8 of the arduino
const int ledPin=A5;//the anode of the LED connects to digital pin D13 of the arduino
int limit;
int value;

//GSM nitialzation
SoftwareSerial mySerial(12, 13);//12=TX,13=RX


//start of interface set up
void setup(){
  
  Serial.begin(115200);//boad rate for any communication
  mySerial.begin(115200);
  
  myservo.attach(11);//servo signal pin is attached to port 10 in board
  myservo.write(mot_max);// max=180'
  
  LoadCell.begin(); // start connection to HX711
  LoadCell.start(2000); // load cells gets 2000ms of time to stabilize
  LoadCell.setCalFactor(439.15); // calibration factor for load cell
  
  lcd.begin(16,2);
  lcd.setCursor(0,0);//optional bcz takes by default
  lcd.print("<--Smart Gas-->");
  lcd.setCursor(0,1);
  lcd.print("pres * to start");


  pinMode(DOUTpin, INPUT);//sets the pin as an input to the arduino
//  pinMode(ledPin, OUTPUT);//sets the pin as an output of the arduino

  delay(2000);

}

float i; //to store weight
float jj;
  
void loop(){
  LoadCell.update(); // retrieves data from the load cell
  i = LoadCell.getData(); // get output value
  jj=(i/4000)*100;
  char key = CKey.getKey();
  value= analogRead(AOUTpin);//reads the analaog value from the methane sensor's AOUT pin
  limit= digitalRead(DOUTpin);//reads the digital value from the methane sensor's DOUT pin
  delay(100);
  if(key){
  keypadEvent(key);
  }
  else if (limit == LOW)
  {   
  //digitalWrite(ledPin, HIGH);
  myservo.write(mot_max);//if limit has been reached, LED turns on as status indicator
  //GSM code
 SendTextMessage();
  }
  else{
  //digitalWrite(ledPin, LOW);
  //myservo.write(mot_min);//if threshold not reached, LED remains off
  } 
  }
  //else // do nothing
 //delay(2000);
 
  
void keypadEvent(KeypadEvent key){
  switch (CKey.getState()){
    case PRESSED:
      switch (key){
        case 'A': 
                  current_state = LOOPING; 
                  myservo.write(mot_max);
                  lcd.clear();
                  lcd.print("valve closed");
                  delay(1000);
                  break; // enter the function with "#"
        case 'B': 
                  current_state = LOOPING;
                  myservo.write(mot_min);
                  lcd.clear();
                  lcd.print("valve opend");delay(1000); 
                  break;
        case 'C': 
                  current_state = LOOPING;
                  
                  lcd.clear();
                  lcd.setCursor(0,0);
                  lcd.print("Weight[%]");
                  lcd.setCursor(0,1);
                  lcd.print(jj);
                  delay(3000);
                  SendLowTextMessage();
                  break;
        case '*':
                  
                  break;
        default : 
                  current_state = LOOPING;
                  lcd.clear();
                  lcd.print("Enter Valid Key");delay(1000);
                  break;
        
      }
    case IDLE:disp();

    break;
  }
  
}
void disp(){
                lcd.clear();
                lcd.print("Enter A=close ");
                lcd.setCursor(0,1);
                lcd.print("B=open C=%OfGas");
                delay(1000);}

void SendTextMessage()
{
mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+918217603917\"\r"); // Replace x with mobile number
  delay(1000);
  mySerial.println("I am SMS from GSM Module");// The SMS text you want to send
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  }

//low lpg sms
void SendLowTextMessage(){
mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+918095331413\"\r"); // Replace x with mobile number
  delay(1000);
  mySerial.println("I am SMS from GSM Module");// The SMS text you want to send
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  }  
  
  
  }
 

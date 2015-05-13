#include <Bounce.h>
#include <LiquidCrystal.h>

// autopilot in P3D
// using Arduino

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
Bounce button1 = Bounce( 3,5 ); // Button digital pin 3 LEFT
Bounce button2 = Bounce( 2,5 ); // Button digital pin 2 RIGHT
Bounce button3 = Bounce( 4,5 ); // Button digital pin 5 ENTER

int button1_oldVal=LOW;
int button2_oldVal=LOW;
int button3_oldVal=LOW;


short menuPage=-1; // Splash screen
const int totPages=6;
char menuPages[][16]={
    "AP Module",
    "Enable",
    "Track",
    "Speed",
    "Heading",
    "Altitude",
    "V. Speed"

};


int values[6]={0,0,0,0,0,0};


bool editing=false;


boolean serialStarted=false;
short curr_index=0; 
int temp_val[6]={0,0,0,0,0,0};


void setup() {
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  pinMode(4,INPUT);
  
  pinMode(7,OUTPUT);
  pinMode(6,OUTPUT);
  lcd.begin(16, 2);
  Serial.begin(9600);
  
  
  

  
}

void loop() {
  
  readSerial();
  
  button1.update ( );
  button2.update ( );
  button3.update();  
  int button1_val = button1.read();
  int button2_val = button2.read();  
  int button3_val = button3.read();  
  
  
  
  
  // ############# LED
    if(values[0]) // Enable bit
     digitalWrite(7,HIGH);
   else
     digitalWrite(7,LOW);
  
  // ************ FIRST ROW
  lcd.setCursor(0, 0);
    if(menuPage!=-1) {
    String t="/6  ";
    lcd.print(menuPage+1+t);
  }
  lcd.print(menuPages[menuPage+1]);
  
  // ************ SECOND ROW

  
  lcd.setCursor(0,1);
  int value=getValue();
  switch(menuPage){
      case -1: // Main
        lcd.print("Firmware 1.1");
        Serial.println("[AP] Firmware v.20150101");
        break;
      default:
        lcd.print(value);
        lcd.print("  ->  ");
        lcd.print(values[menuPage]);
        lcd.print("     ");
        break;
  }
  
  
  if(buttonIsPressed(button1.read(), &button1_oldVal))  
      if(menuPage<totPages-1){
          lcd.clear();
          menuPage++;
        }
  
  if(buttonIsPressed(button2.read(), &button2_oldVal))  
      if(menuPage>-1){
          lcd.clear();
          menuPage--;
        }
  
  if(buttonIsPressed(button3.read(), &button3_oldVal))
      if(menuPage>-1){
        values[menuPage]=value;
        if(values[0]==0 && menuPage==0){
        tone(6,1500,200);
        delay(100);
        tone(6,1400,200);
        delay(100);
        tone(6,1500,200);        
        delay(100);
        tone(6,1400,200);                
      }  
        writeSerial();
      }

}

int getValue() {

  switch(menuPage) {
    case 0: //Enable
      return (int)(round(analogRead(0)/1023.0));

    case 1: // Track
      return (int)(analogRead(0)/1023.0*360.0);
      break;
    case 2: // Speed
      return (int)(analogRead(0)/1023.0*250.0)+100;
      break;
    case 3: // Heading
      return (int)(analogRead(0)/1023.0*360.0);
      break;
    
    case 4: // Altitude
      return (int)(analogRead(0)/1023.0*19000.0/100.0)*100+1000;
      break;
      
    case 5: // Vertical Speed
      return (int)((analogRead(0)/1023.0*4000.0-2000.0)/100.0)*100;
      break;
    
    
  }
}

void writeSerial() {
 String json="{ \"en\":"+String(values[0])+", \"trk\" : "+String(values[1])+", \"spd\" : "+String(values[2])+", \"hdg\" : "+String(values[3])+", \"alt\" : "+String(values[4])+", \"vsp\" : "+String(values[5])+" }";
 Serial.println(json); 
}


void readSerial() {
   
   while(Serial.available()) {
      byte curr=Serial.read();
      
      if(curr==60) // < Begin packet
      {
        serialStarted=true;
        curr_index=0;
        for(int cont=0;cont<6;cont++)
          temp_val[cont]=0; 
        
      }
      else if(curr>47 && curr<58 && serialStarted) { // It's a number
        temp_val[curr_index]=(temp_val[curr_index]*10)+(curr-48);
      }
      else if(curr==44) { // , delimiter
        curr_index++;
      }
      else if(curr==62) {
        if(serialStarted) { // Communication was properly initialized 
          loadValues();
          serialStarted=false;
        }
      }
    
    
   } 
}

void loadValues() {
  Serial.println("[OK-RCV]");
 for(int cont=0;cont<6;cont++)
    values[cont]=temp_val[cont];
  
}


boolean buttonIsPressed(int buttValue, int* buttOldVal) {
   if(buttValue!= *buttOldVal){
      *buttOldVal=buttValue;
      if(buttValue==HIGH)
        return true;
   } 
   return false;
}




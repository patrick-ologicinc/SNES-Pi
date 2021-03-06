
const int PiTX = 2;
const int MOSFET = 4;
const int statusLED = 12;
const int level3v3to5vCtrl = 18;
const int level5vto3v3Ctrl = 19;
const int powerSwitch = 3;





int PowerState = 0; //assume power is off
int MOSFET_State = 0;//start with MOSFET Disabled
int Invert_MOSFET_Out = 1;//Drive P-Channel MOSFETs low to turn on. set to 1 to invert MOSFET control output. Else, 0 to make "On" Logic 1.
int i =0;


void setup() 
{
  Serial.begin(115200);
  
  pinMode(powerSwitch, INPUT);//Set Power Switch Pin as input
  digitalWrite(powerSwitch, HIGH); //Turns on internal Pullup Resistor. Drive Pin low to turn on power.
  
  pinMode(statusLED,OUTPUT);
  digitalWrite(statusLED, LOW);
  
  pinMode(PiTX, INPUT);//Set Pi's TX output Pin as input. If pin is driven low, then the Pi should have successfully shut down.
  
  pinMode(MOSFET, OUTPUT);//Set MOSFET Control as an output
  
  //Control level translation chips. Default is to have chips in high impedence. When system is on, chips are enabled.
  pinMode(level3v3to5vCtrl, OUTPUT);
  pinMode(level5vto3v3Ctrl, OUTPUT);
  
  turnLevelShiftersOff();//Make sure level translators are off
  
  
  
  turnMOSFEToff(Invert_MOSFET_Out); //Turn off MOSFET
}

void loop()
{
  
  if (PowerState == 0) // If power is Off
   {
   if ( powerSwitchOn() ) //AND power Switch is On
     turnMOSFETon(Invert_MOSFET_Out); //Turn On System Power
   }
  else if ( (PowerState == 1) && (powerSwitchOn() == 0) )//If Power Is On, but Power Switch is off...
  {
    PowerOffViaSerial();//Send Pi commands to Power Off via serial connection
    delay(5000);//Wait for system to power down
    
    i = 10;
    while(i>0)
    {      
      if (isPiOn() == 0 )//check to see if Pi has safely shut down
      {
       digitalWrite(statusLED,HIGH);
       delay(3000);  
       digitalWrite(statusLED,LOW);
       i=0;
      }
      else//If Pi is STILL on..wait
      {
       delay(5000);
       i--;
       
       if(i == 4)
        PowerOffViaSerial();//Try to power off via serial once more
      }
    }
    
    turnMOSFEToff(Invert_MOSFET_Out);
    
  }
    
}


int powerSwitchOn(){
 if (digitalRead(powerSwitch) == 0 )
  return 1;
 else
  return 0;
 }


int PowerOffViaSerial()
{  /*
     // $: "exit"
     Serial.write("exit\n");
     //*<sleep 2>
    delay(2000);
    //<ctrl+c>
    Serial.print(0x03, BIN);
    
    */
     //*<sleep 2>
    delay(2000);
    //<username>
    Serial.write("powermanagement\n");
    //*<sleep 1>
    delay(2000);
    //<password>
    Serial.write("poweruser\n");
    //*<sleep 1>
    delay(2000);

    //$: "poweroff"
    Serial.write("sudo poweroff\n");
}    

void turnMOSFETon(int invertOut)
{  
      // turn Power on:    
    MOSFET_State = 1; 
    PowerState = 1;
    
    if(invertOut)
     digitalWrite(MOSFET, 0);  
    else
     digitalWrite(MOSFET, MOSFET_State);  
       
    delay(20000);
    
    turnLevelShiftersOn();
}

void turnMOSFEToff(int invertOut)
{
      // turn Power off:    
    MOSFET_State = 0; 
    PowerState = 0;
    
    turnLevelShiftersOff();
    
    if(invertOut)
     digitalWrite(MOSFET, 1);  
    else
     digitalWrite(MOSFET, MOSFET_State);  
     
     
    delay(2000);  
}



int isPiOn()
{
 int j =0;
 for(j=0;j<10;j++)
 {
  if(digitalRead(PiTX) == HIGH)
   return(1);
  else
   delay(50); 
 }

 return(0);
}

void turnLevelShiftersOff()
{
  
 digitalWrite(level3v3to5vCtrl,HIGH);
 digitalWrite(level5vto3v3Ctrl,LOW) ;
}


void turnLevelShiftersOn()
{
 digitalWrite(level3v3to5vCtrl,LOW);
 digitalWrite(level5vto3v3Ctrl,HIGH) ;
}

  
  
  



    void(* resetFunc) (void) = 0; //software reset vector
    #include <EEPROM.h> 
    #include <LiquidCrystal.h>
    LiquidCrystal lcd(42, 44, 52, 50, 48, 46);
    
    //buttons variables
    const int start_buttonPin = 2;     // the number of the start pushbutton pin
    const int stop_buttonPin = 3;     // the number of the stop pushbutton pin
    const int select_buttonPin = 4;
    const int piezoPin = 12;
    const int run_relay =  13;
    const int run_lamp =  14;
    const int sens_in1 =  6;
    const int sens_in2 =  7;
    const int sens_in3 =  8;
    const int sens_out1 = 9;
    const int sens_out2 = 10;
    const int sens_out3 = 11;
    int lastButtonPushed = 0;
    int sens_in_interrupt = HIGH;
    int sens_out_interrupt = HIGH;
    int read_start;    //start button flag
    int read_stop;    // stop button flag
    int run_status = 0;         //run status flag
    int lastStartButtonState = HIGH;  
    int lastStopButtonState = HIGH;   
    int lastSelectButtonState = HIGH;   
    int lastSensIn1State = HIGH;
    int lastSensIn2State = HIGH;
    int lastSensIn3State = HIGH;   
    int lastSensOut1State = HIGH;
    int lastSensOut2State = HIGH;
    int lastSensOut3State = HIGH;
    int frame1 = LOW;
    int frame2 = LOW;
    int frame3 = LOW;
    int sens1_active = LOW;
    int sens2_active = LOW;
    int sens3_active = LOW;
    
    long lastStartDebounceTime = 0;  
    long lastStopDebounceTime = 0;  
    long lastSelectDebounceTime = 0;
    long lastSensIn1DebounceTime = 0;
    long lastSensIn2DebounceTime = 0;
    long lastSensIn3DebounceTime = 0;
    long lastSensOut1DebounceTime = 0;     
    long lastSensOut2DebounceTime = 0;
    long lastSensOut3DebounceTime = 0;          
    long debounceDelay = 150;    // button the debounce time  
    long SensdebounceDelay = 500; 
    long holddebounceDelay = 2000;
    
    double Sens1Time = 0;
    double Sens1LastCheck = millis();
    double Sens2Time = 0;
    double Sens2LastCheck = millis();
    double Sens3Time = 0;
    double Sens3LastCheck = millis();
    int SensIn1Flag = 0;
    int SensOut1Flag = 0;
    int SensIn2Flag = 0;
    int SensOut2Flag = 0;
    int SensIn3Flag = 0;
    int SensOut3Flag = 0;
      
    int menu_display = 0;//0-default display 
    int last_menu=0; //last displayed menu flag
    int manage_status = 0;
    int manage_mode = LOW; //high = auto , low = manual
    String error_msg;
    long frame_counter;
   
    long EEPROM_read_time = 0;
    long time_var = 0;   
    int EE_addr1 = 0;
    
    void setup()  {
       Serial.begin(9600);
       pinMode(start_buttonPin, INPUT_PULLUP);
       pinMode(stop_buttonPin, INPUT_PULLUP);
       pinMode(select_buttonPin, INPUT_PULLUP);
       pinMode(sens_in1, INPUT_PULLUP);
       pinMode(sens_in2, INPUT_PULLUP);
       pinMode(sens_in3, INPUT_PULLUP);
       pinMode(sens_out1, INPUT_PULLUP);
       pinMode(sens_out2, INPUT_PULLUP);
       pinMode(sens_out3, INPUT_PULLUP);
       pinMode(run_lamp, OUTPUT);
       pinMode(run_relay, OUTPUT);
       digitalWrite(run_relay, LOW); //make sure soft reset will reset the relay
       digitalWrite(run_lamp, LOW);
       //attachInterrupt(digitalPinToInterrupt(sens_in),read_sens_in , FALLING);
       //attachInterrupt(digitalPinToInterrupt(sens_out),read_sens_out , FALLING);
       
       if (digitalRead(start_buttonPin)==0)  {
        delay(1500);
        long default_time = 60; // default time out 5 minutes
       
        EEPROM.put(EE_addr1, default_time);
        }
      //read stored settings
      EEPROM.get(EE_addr1,EEPROM_read_time);
      time_var = EEPROM_read_time;     
      //time_var = 120;  // testing time delay
       scroll("Starting up ....",13,1);
       lcd.begin(16, 2);
       lcd.print("Time Out=");
       lcd.print(time_var);
       delay(1000);     
       }

       
void read_inputs() {
 // read the state of the switches/sensors into a local variable:
  int reading;
  int start_buttonState=HIGH;             
  int stop_buttonState=HIGH;             
  int select_buttonState=HIGH;     
  int SensIn1State = HIGH;
  int SensIn2State = HIGH;
  int SensIn3State = HIGH;
  int SensOut1State = HIGH;
  int SensOut2State = HIGH;
  int SensOut3State = HIGH;
  
  //start button              
  reading = digitalRead(start_buttonPin);
  if (reading != lastStartButtonState) {
   lastStartDebounceTime = millis();
     }                
      if ((millis() - lastStartDebounceTime) > debounceDelay) {
        start_buttonState = reading;
        lastStartDebounceTime=millis();
        }                               
        lastStartButtonState = reading; 
 //select button              
  reading = digitalRead(select_buttonPin);
  if (reading != lastSelectButtonState) {
   lastSelectDebounceTime = millis();
     }                
      if ((millis() - lastSelectDebounceTime) > debounceDelay) {
        select_buttonState = reading;
        lastSelectDebounceTime=millis();
        }                               
        lastSelectButtonState = reading; 

 //stop button              
  reading = digitalRead(stop_buttonPin);
  if (reading != lastStopButtonState) {
   lastStopDebounceTime = millis();
     }                
      if ((millis() - lastStopDebounceTime) > debounceDelay) {
        stop_buttonState = reading;
        lastStopDebounceTime=millis();
        }                               
        lastStopButtonState = reading;
        
//Sensors
  reading = digitalRead(sens_in1);
  if (reading != lastSensIn1State) {
   lastSensIn1DebounceTime = millis();
     }                
      if ((millis() - lastSensIn1DebounceTime) > SensdebounceDelay) {
        SensIn1State = reading;
        lastSensIn1DebounceTime=millis();
        }                               
        lastSensIn1State = reading;
  
  reading = digitalRead(sens_in2);
  if (reading != lastSensIn2State) {
   lastSensIn2DebounceTime = millis();
     }                
      if ((millis() - lastSensIn2DebounceTime) > SensdebounceDelay) {
        SensIn2State = reading;
        lastSensIn2DebounceTime=millis();
        }                               
        lastSensIn2State = reading;

   reading = digitalRead(sens_in3);
  if (reading != lastSensIn3State) {
   lastSensIn3DebounceTime = millis();
     }                
      if ((millis() - lastSensIn3DebounceTime) > SensdebounceDelay) {
        SensIn3State = reading;
        lastSensIn3DebounceTime=millis();
        }                               
        lastSensIn3State = reading;  
                 
   reading = digitalRead(sens_out1);
  if (reading != lastSensOut1State) {
   lastSensOut1DebounceTime = millis();
     }                
      if ((millis() - lastSensOut1DebounceTime) > SensdebounceDelay) {
        SensOut1State = reading;
        lastSensOut1DebounceTime=millis();
        }                               
        lastSensOut1State = reading;   

   reading = digitalRead(sens_out2);
  if (reading != lastSensOut2State) {
   lastSensOut2DebounceTime = millis();
     }                
      if ((millis() - lastSensOut2DebounceTime) > SensdebounceDelay) {
        SensOut2State = reading;
        lastSensOut2DebounceTime=millis();
        }                               
        lastSensOut2State = reading;   

   reading = digitalRead(sens_out3);
  if (reading != lastSensOut3State) {
   lastSensOut3DebounceTime = millis();
     }                
      if ((millis() - lastSensOut3DebounceTime) > SensdebounceDelay) {
        SensOut3State = reading;
        lastSensOut3DebounceTime=millis();
        }                               
        lastSensOut3State = reading;   
                            
//main menu actions

 if(start_buttonState==LOW && menu_display==0){
  tone( piezoPin, 2000, 250);
  lastButtonPushed=1;
 }else if(select_buttonState==LOW && menu_display==0){
  tone( piezoPin, 2100, 250);
  lastButtonPushed=2;
 }else if(start_buttonState==LOW && menu_display==2){
  tone( piezoPin, 2100, 250);
  lastButtonPushed=3;
 }else if(stop_buttonState==LOW && (run_status == 0 || run_status == 1)) {
  tone( piezoPin, 2100, 250);
  lastButtonPushed=5;
 }else if(select_buttonState==LOW && menu_display==2){
  tone( piezoPin, 2100, 250);
  lastButtonPushed=4;
 }else if(select_buttonState==LOW && menu_display==4){
  tone( piezoPin, 2100, 250);
  lastButtonPushed=0; 
 }else if(start_buttonState==LOW && (lastButtonPushed==3 || lastButtonPushed==5 )){
  tone( piezoPin, 2100, 250);
  lastButtonPushed=2;     
 }
//Sensor actions
  else if(SensIn1State==LOW){
    SensIn1Flag = SensIn1Flag + 1;
  }
  else if(SensOut1State==LOW){
    SensOut1Flag = SensOut1Flag + 1;
  }
  else if(SensIn2State==LOW){
    SensIn2Flag = SensIn2Flag + 1;
  }
  else if(SensOut2State==LOW){
    SensOut2Flag = SensOut2Flag + 1;
  }
  else if(SensIn3State==LOW){
    SensIn3Flag = SensIn3Flag + 1;
  }
  else if(SensOut3State==LOW){
    SensOut3Flag = SensOut3Flag + 1;
  }
}
 
void lcd_display(int col, int row, String msg1, String msg2)  {
      lcd.setCursor(col, row);
      lcd.print(msg1);
      lcd.print(msg2); 
  }
  

void set_time() {
    
   EEPROM.put(EE_addr1, time_var);
   lcd.clear();
   EEPROM.get(EE_addr1,EEPROM_read_time);
   time_var = EEPROM_read_time; 
   lcd_display(0,0,"Settings Saved!"," ");
   delay(2000);
   run_status = 0;
   lastButtonPushed=2; //reset the lastButtonPushed variable
   }



 void read_user_set() {
  //read stored settings
      EEPROM.get(EE_addr1,EEPROM_read_time);
      time_var = EEPROM_read_time;     
 }

  void default_display()  {
      lcd.setCursor(0, 0);
      lcd.print("<<< OFFLINE >>> ");
      lcd.setCursor(0, 1);
      lcd.print(" ENTER To START ");  
  }
  
  void set_menu_time() {
      run_status = 3;
      lcd.setCursor(0, 0);
      lcd.print("HIT  ENT to SAVE");
      lcd.setCursor(0, 1);
      lcd.print("Time Out Sec=");
      time_var = EEPROM_read_time;
      lcd.print(time_var);
      
      delay(2000);
      while (lastButtonPushed==3 || lastButtonPushed==5) {
        read_inputs();
        if (digitalRead(select_buttonPin)== 0) {
          delay(250);
          time_var = time_var+1;
        }
        if (digitalRead(stop_buttonPin)== 0) {
          delay(250);
          time_var = time_var-1;
      }
      if (time_var == 99) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("HIT  ENT to SAVE");
        }
      if (time_var == 9) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("HIT  ENT to SAVE");
        }
      lcd.setCursor(0, 1);   
      lcd.print("Time Out Sec=");
      lcd.print(time_var);       
    }
    set_time();
    
  }
  

  void scroll (String msg, int pos, int repeat) {
  
 for (int rep = 0; rep < repeat; rep++)  { 
  lcd.clear();
  lcd.begin(16, 2);
  lcd.print(msg);
  delay(50);
  // scroll 13 positions (string length) to the left
  // to move it offscreen left:
  for (int positionCounter = 0; positionCounter < pos; positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayLeft();
    // wait a bit:
    read_inputs();
    delay(50);
  }

  // scroll 13 positions (string length + display length) to the right
  // to move it offscreen right:
  for (int positionCounter = 0; positionCounter < pos; positionCounter++) {
    // scroll one position right:
    lcd.scrollDisplayRight();
    // wait a bit:
    read_inputs();
    delay(50);
  }
 
  // delay at the end of the full loop:
  delay(50);
  read_inputs();
 }
}
 
  void display_lcd(int col, int row, String msg) {
  
  lcd.setCursor(col,row);
  lcd.print(msg);
}
  void main_menu() {
  
   switch (lastButtonPushed) {
    case 0:
      default_display();
      menu_display = 0;
      last_menu = menu_display;
      run_status = 0;
       delay(150);
      break;
    
    case 1:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("<<< ON-LINE >>> ");
      menu_display = 1;
      last_menu = menu_display;
      digitalWrite(run_lamp, HIGH);
      run_status = 1;
       delay(150);
      break;
    
    case 2:
      
      lcd.setCursor(0, 0);
      lcd.print("ENTER TO CHANGE!");
      lcd.setCursor(0, 1);
      lcd.print("Time Out Sec=");
      lcd.print(EEPROM_read_time,1);
      menu_display = 2;
      last_menu = menu_display;
      delay(150);
      break;
      
   case 3:
      lastButtonPushed=3;
      menu_display = 3;      
      last_menu = menu_display;    
      delay(150);
      set_menu_time();
      break;  
      

   case 4:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Frame  Counter ");
      lcd.setCursor(0, 1);
      lcd.print("No of Frame =");
      lcd.print(frame_counter);
      menu_display = 4;
      last_menu = menu_display;
      delay(150);
      break;

  case 5:
    lastButtonPushed = 0;
    if (digitalRead(stop_buttonPin) == LOW && run_status == 0) {
          Serial.println("Reset 0");
          resetFunc();
      }    
    if (digitalRead(stop_buttonPin) == LOW && run_status == 1) {
          Serial.println("Stop");
          run_status = 0;
          digitalWrite(run_lamp, LOW);
          lastButtonPushed = 0;
          //reset_sens1();
          //reset_sens2();
          //reset_sens3(); 
          SensIn1Flag = 0;
          frame1 = LOW;
          SensIn2Flag = 0;
          frame2 = LOW;
          SensIn3Flag = 0;
          frame3 = LOW;
          sens1_active == LOW;
          sens2_active == LOW; 
          sens3_active == LOW;   
          delay (150);         
      }

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("STOP  ACTIVATED!");
      run_status = 0;
      break;
   }
   
  } 
  
  void error_alarm()  {
      lcd.clear();
      digitalWrite(run_relay, HIGH);
      digitalWrite(run_lamp, LOW);
      Serial.println(error_msg);
      scroll(" <<< ERROR >>>  ",14,2);
      display_lcd(0,0,error_msg); 
      display_lcd(0,1," <<< ERROR >>>  ");      
        
      tone( piezoPin, 2100, 250);
      delay(200);
      noTone(piezoPin);
      delay(200);
     
      tone( piezoPin, 2100, 250);
      delay(200);
      noTone(piezoPin);
      delay(200);
     
      tone( piezoPin, 2000, 250);
      delay(200);
      tone( piezoPin, 2100, 250);
     
      delay(200);
      noTone(piezoPin);
      delay(200);
      tone( piezoPin, 2100, 250);
      
      delay(200);
      noTone(piezoPin);
      delay(200);
      tone( piezoPin, 2000, 250);
  }
  void set_sens1() {
    Serial.println("SensIn1 Flag = ");
    Serial.println(SensIn1Flag);
    Serial.println("S1 is set");
    Sens1Time = time_var*1000;
    Sens1LastCheck = millis();
    frame1 = HIGH;
    SensOut1Flag = 0;
    lcd.setCursor(0, 0);
    lcd.print("<<SENSOR 1 SET>>");
    tone( piezoPin, 1750, 150);
  }
  
  void set_sens2() {
    Serial.println("SensIn2 Flag = ");
    Serial.println(SensIn2Flag);
    Serial.println("S2 is set");
    Sens2Time = time_var*1000;
    Sens2LastCheck = millis();
    frame2 = HIGH;
    SensOut2Flag = 0;
    lcd.setCursor(0, 0);
    lcd.print("<<SENSOR 2 SET>>");
    tone( piezoPin, 1850, 150);         
  }
  void set_sens3() {
    Serial.println("SensIn3 Flag = ");
    Serial.println(SensIn3Flag);
    Serial.println("S3 is set");
    Sens3Time = time_var*1000;
    Sens3LastCheck = millis();
    frame3 = HIGH;
    SensOut3Flag = 0;
    lcd.setCursor(0, 0);
    lcd.print("<<SENSOR 3 SET>>");
    tone( piezoPin, 1950, 150);         
  }
  void reset_sens1() {
    SensIn1Flag = 0;
    Serial.println("S1 reset.....");
    frame1 = LOW;
    lcd.setCursor(0, 0);
    lcd.print("<<SENSOR 1 RST>>");
    tone( piezoPin, 2200, 150);
    frame_counter = frame_counter+1;            
  }
  void reset_sens2() {
    SensIn2Flag = 0;
    Serial.println("S2 reset.....");
    frame2 = LOW;
    lcd.setCursor(0, 0);
    lcd.print("<<SENSOR 2 RST>>");
    tone( piezoPin, 2300, 150);
    frame_counter = frame_counter+1;             
  }
  void reset_sens3() {
    SensIn3Flag = 0;
    Serial.println("S3 reset.....");
    frame3 = LOW;
    lcd.setCursor(0, 0);
    lcd.print("<<SENSOR 3 RST>>"); 
    tone( piezoPin, 2400, 150);
    frame_counter = frame_counter+1;            
  }
  void update_sens1() {
    Sens1Time -= ((millis() - Sens1LastCheck));
    if (floor(Sens1Time/1000) == 99) {
     lcd.setCursor(0, 1);
     lcd.print("1=   ");
      }
    if (floor(Sens1Time/1000) == 9) {
     lcd.setCursor(0, 1);
     lcd.print("1=   ");
      }  
    lcd.setCursor(0, 1);
     lcd.print("1=");
    lcd.print(Sens1Time/1000,0);
    Sens1LastCheck = millis();
    if (Sens1Time <= 0) {
      error_msg = " Error Frame 1 ";
      error_alarm();
      return;  
    }
    Serial.println("S1 updated");
    Serial.println(SensIn1Flag);
  }
   void update_sens2() {
    Sens2Time -= ((millis() - Sens2LastCheck));
    if (floor(Sens2Time/1000) == 99) {
     lcd.setCursor(6, 1);
     lcd.print("2=   ");
      }
    if (floor(Sens2Time/1000) == 9) {
     lcd.setCursor(6, 1);
     lcd.print("2=   ");
      }  
    lcd.setCursor(6, 1);
     lcd.print("2=");
    lcd.print(Sens2Time/1000,0);
    Sens2LastCheck = millis();
    if (Sens2Time <= 0) {
      error_msg = "  Error Frame 2 ";
      error_alarm();
      return;  
    }
    Serial.println("S2 updated");
    Serial.println(SensIn2Flag);
  }
  void update_sens3() {
    Sens3Time -= ((millis() - Sens3LastCheck));
    if (floor (Sens3Time/1000) == 99) {
      lcd.setCursor(11, 1);
    lcd.print("3=   ");
      }
    if (floor(Sens3Time/1000) == 9) {
      lcd.setCursor(11, 1);
    lcd.print("3=   ");
      }  
    lcd.setCursor(11, 1);
    lcd.print("3=");
    lcd.print(Sens3Time/1000,0);
    Sens3LastCheck = millis();
    if (Sens3Time <= 0) {
      error_msg = "  Error Frame 3 ";
      error_alarm();
      return;  
    }
    Serial.println("S3 updated");
    Serial.println(SensIn3Flag);
  }
  void check_sensor1() {
    //Serial.println("SensIn1Flag = ");
    //Serial.println(SensIn1Flag);
    //Serial.println("SensOut1Flag = ");
    //Serial.println(SensOut1Flag);
    if (SensOut1Flag == 2 && frame1 == LOW && sens1_active == LOW ) {
      //Serial.println(sens1_active);
      sens1_active = HIGH;
      lcd.setCursor(0, 0);
      lcd.print("SENSOR 1 ACTIVED");
      frame_counter = frame_counter+1;
      SensIn1Flag = 0;
      SensOut1Flag = 0;
    } 
    if (SensIn1Flag == 2 && frame1 == LOW && sens1_active == HIGH) {
      set_sens1();  
    }
    if (SensOut1Flag == 2 && frame1 == HIGH && sens1_active == HIGH) {
      reset_sens1();
    }
    if (frame1 == HIGH && sens1_active == HIGH) {
     update_sens1();   
    }
  }
  void check_sensor2() {
    //Serial.println("SensIn2Flag = ");
    //Serial.println(SensIn2Flag);
    //Serial.println("SensOut2Flag = ");
    //Serial.println(SensOut2Flag);
   if (SensOut2Flag == 2 && frame2 == LOW && sens2_active == LOW) {
      //Serial.println(sens2_active);
      sens2_active = HIGH;
      lcd.setCursor(0, 0);
      lcd.print("SENSOR 2 ACTIVED");
      frame_counter = frame_counter+1;
      SensIn2Flag = 0;
      SensOut2Flag = 0;
    } 
    
    if (SensIn2Flag == 2 && frame2 == LOW && sens2_active == HIGH) {
      set_sens2();  
    }
    if (SensOut2Flag == 2 && frame2 == HIGH && sens2_active == HIGH) {
      reset_sens2();
    }
    if (frame2 == HIGH && sens2_active == HIGH) {
     update_sens2();   
    }
  }
  void check_sensor3() {
    //Serial.println("SensIn3Flag = ");
    //Serial.println(SensIn3Flag);
    //Serial.println("SensOut3Flag = ");
    //Serial.println(SensOut3Flag);
    if (SensOut3Flag == 2 && frame3 == LOW && sens3_active == LOW) {
      //Serial.println(sens1_active);
      sens3_active = HIGH;
      lcd.setCursor(0, 0);
      lcd.print("SENSOR 3 ACTIVED");
      frame_counter = frame_counter+1;
      SensIn3Flag = 0;
      SensOut3Flag = 0;
    } 
    if (SensIn3Flag == 2 && frame3 == LOW && sens3_active == HIGH) {
      set_sens3();  
    }
    if (SensOut3Flag == 2 && frame3 == HIGH && sens3_active == HIGH) {
      reset_sens3();
    }
    if (frame3 == HIGH && sens3_active == HIGH) {
     update_sens3();   
    }
  }

  void sensor_manager() {
    if (run_status == 1) {
      read_inputs();
      check_sensor1();
      read_inputs();
      check_sensor2();
      read_inputs();
      check_sensor3();
      
    }    
  }
  
  void loop() {
   read_inputs();
   main_menu(); 
   while (lastButtonPushed == 1)  {
    sensor_manager(); 
    read_inputs();
   }   
  }



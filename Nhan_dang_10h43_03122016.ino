#include<EEPROM.h>
/*   khai bao pin quang tro nhan tin hieu tu led quet  */
#define Led_Signal_1 A0
#define Led_Signal_2 A1
#define Led_Signal_3 A2
#define Led_Signal_4 A3
#define Led_Signal_5 A4
#define Led_Signal_6 A5
/*<==========================>*/
/*   khai bao led quet  */
#define Led_1 2
#define Led_2 3
#define Led_3 4
#define Led_4 5
#define Led_5 6
#define Led_6 7
/*<==========================>*/
/*   khai bao led STT  */
#define Circle_LED 10
#define Square_LED 11
#define Triangular_LED 12
/*<==========================>*/
/*   khai bao led hanh trinh  */
#define Journey_Led 13
#define Journey_Signal A6
int Var_Journey_Signal = 0;
/*<==========================>*/
/*   khai bao nut nhan */
#define Button A7
#define Using_Mode 1 
#define Trainning_Mode 2
#define Square_Imagine 3
#define Circle_Imagine 4
#define Triangular_Imagine 5
int Label_Image = 0;   //
int Set_Imagine_Type = 0;
int Imagine_Type = 0;
int Mode_Choice = 0;
int Current_Mode = 0;
/*<==========================>*/
/*   khai bao bien su dung */
int Setting_STT = 0;
float Min_SG[7], Max_SG[7];
int Find_Min = 0;
int Find_Max = 0;

int STT_Val = 0;
float Area = 0;         // Feature 1 = 0;
float Volume_Image = 0; // Feature 2 = 0;
float Max_Signal = 0; ///// Feature 3 = 0
int Journey_Time = 0;  /// Feature 4 = 0
int Journey_Mid = 940;

float Sample_Array[7];   /// array tin hieu
float Basic_Signal[7];    /// array lay mau ( nen )
float Array_Feature[26][9] = {0}; /// array scan

/*<==========================>*/
/*   khai bao bien EEPROM su dung */
int Add_Begin_Save_data = 20;
int Count_Save_Data = 0;
int Add_Save_Finish = 115;
int Add_Save_Setting = 120;
//=======***********************  MAIN  *************************=========================
void setup()
{
//  for( int i = 0; i <= 20; i++) Save_Feature();
//  Save_Setting_MixMax();
//writeEEPROM(Add_Save_Setting,0);
//  delay(5000);
  Pin_Init();   // setting fuction of microcontrol pin
  Load_Feature();  // load data
  if( Setting_STT == 0)
  {
    Setting_MinMax_SG();
  }
  STT_Init();   //  Start again
  if(STT_Val == 1) 
    {
      for(int i = 0; i <= 9; i ++) Sample_Led_SG();
      Show_Basic_Sample();
      STT_Val = 0;  
    }
  Serial.println("<=====Pls put button to choice system mode and Stype of thing ====>");
  Show_Debug_Save_Min_Max_SG();
  Show_Debug_Load_Data_Array();
} 
void(* resetFunc) (void) = 0;
void loop() 
{
//  Sample_Led_SG();
    Change_STT();
  /* kiem tra mode */
    Process_Button();
    Process_Journey_Led();
  /*<======current = 1 --> mode Using  || current = 2 --> mode Training  ====================>*/
  /* chon mode hoat dong */
    if(Var_Journey_Signal == 1)
    {
      switch(Current_Mode)
      {
        case Using_Mode:{ Process_Using_Mode(); break;}
        case Trainning_Mode:{ Process_Training_Mode(); break;}
      }
    }

}
//=======***********************  THE END OF MAIN *************************=========================

//=======***********************  SOME FUNCTION *************************=========================
void Process_Button()
{
    
    if(Set_Imagine_Type <= 0) Set_Imagine_Type = 0;
    Mode_Choice = Key_Mode_Button();
    if(Mode_Choice != 0) Choice_And_Show_Mode();
    if(Current_Mode == Trainning_Mode)
    {
      while(Set_Imagine_Type == 0)
      {
        Set_Imagine_Type = Key_Imagine_Button();
        if(Set_Imagine_Type != 0)
        {
          Imagine_Type = Set_Imagine_Type;
          Serial.println(Imagine_Type);
          Label_On_Led(Imagine_Type);
        }
        Mode_Choice = Key_Mode_Button();
        if(Mode_Choice != 0)
        {
          Choice_And_Show_Mode();
          Set_Imagine_Type = -1;
        }
      }
    }
}
void Choice_And_Show_Mode()
{
  Current_Mode = Mode_Choice; 
  switch(Current_Mode)
    {
      case Using_Mode:{ Serial.println("MODE Using==============>");delay(100); break;}
      case Trainning_Mode:{ Set_Imagine_Type = 0; Serial.println("MODE Training==============>");delay(100);break;}
    }
}
void Process_Using_Mode()
{
  Process_Led_SG();
  if(Area >=3) 
  {
    Label_Image = Compare_Image();
    Label_On_Led(Label_Image);
    Show_Debug();
    for(int i = 0; i <= 2; i ++) Sample_Led_SG();
    Show_Basic_Sample(); 
  }

}
void Process_Training_Mode()
{
  Set_Imagine_Type = 0;
  Process_Led_SG();
//  if(Area >=3) 
//  {
  Save_Feature();  // lưu eeprom training\
  delay(5);
  Load_Feature();
  Show_Debug_Load_Data_Array();
  Count_Save_Data ++;
//  }
  Show_Debug();
  if(Count_Save_Data == 20)
  {
      Count_Save_Data = 0;
      Load_Feature();
      Show_Debug_Load_Data_Array();
  }
  delay(2000);
  for(int i = 0; i <= 2; i ++) Sample_Led_SG();
  Show_Basic_Sample();
}

int LED, LED_IN;
int Var_LED_IN = 0;  // tin hieu led thu
void Remap_Pin(int tt)
{
      if (tt == 1)      {LED = Led_1; LED_IN = Led_Signal_1;}
      else if (tt == 2) {LED = Led_2; LED_IN = Led_Signal_2;}
      else if (tt == 3) {LED = Led_3; LED_IN = Led_Signal_3;}
      else if (tt == 4) {LED = Led_4; LED_IN = Led_Signal_4;}
      else if (tt == 5) {LED = Led_5; LED_IN = Led_Signal_5;}
      else              {LED = Led_6; LED_IN = Led_Signal_6;}
}
void Process_Led()
{
    digitalWrite(LED,HIGH);delay(2); 
    Var_LED_IN = analogRead(LED_IN);delay(5);
    digitalWrite(LED,LOW);  delay(2);    
}
void Setting_MinMax_SG()
{
  for( int w = 1; w < 7; w++){ Min_SG[w] = 0; Max_SG[w] = 0;}
  Serial.println("Dang lay SG Max========================>");
  Find_Min = 0;Find_Max = 1;
  Find_Min_Max_SG();
  Serial.println("co 10s de lay vat ra lay SG Mmin========================>");
  delay(10000);
  Find_Max = 0;Find_Min = 1;
  Find_Min_Max_SG();
  delay(1000);
  for(int ii = 1; ii < 7; ii++)
  {
    Serial.print("Min ");Serial.print(ii);Serial.print(":  ");Serial.print(Min_SG[ii]);Serial.print("\t");
    Serial.print("Max ");Serial.print(ii);Serial.print(":  ");Serial.print(Max_SG[ii]);
    Serial.print("\n");
  }
   delay(2000);
  Setting_STT = 1;
  Save_Setting_MixMax();
}
void Find_Min_Max_SG()
{
  for(int Var_Sample = 0; Var_Sample < 11; Var_Sample++)
  {
    for( int qq = 1; qq < 7; qq++)
      {
        Remap_Pin(qq);
        Process_Led();
        Sample_Array[qq] =  (float(Var_LED_IN));
//        Serial.print(Sample_Array[qq]);Serial.print("\t");
        if(Find_Min == 1)  Min_SG[qq] = (Sample_Array[qq]/11) + Min_SG[qq];
        if(Find_Max == 1)  Max_SG[qq] = (Sample_Array[qq]/11) + Max_SG[qq];
      }
//      Serial.print("\n");
  }
//  Serial.print("\n");
}

void Sample_Led_SG()
{
  Area = 0;         // dien tich = 0;
  Journey_Time = 0;  /// thoi gian hanh trinh = 0
  Volume_Image = 0; // the tich = 0;
  Max_Signal = 0; ///// max size cao = 0
  Imagine_Type = 0; //// hinh dang reset
  for(int Var_Sample = 0; Var_Sample <=10; Var_Sample++)
  {
    for( int qq = 1; qq<=6; qq++)
      {
        Remap_Pin(qq);
        Process_Led();
        Sample_Array[qq] =  (float(Var_LED_IN)-Min_SG[qq])*40/(Max_SG[qq]-Min_SG[qq]);
//        Sample_Array[qq] =  (float(Var_LED_IN));
        if(Sample_Array[qq] <= 3 ) Sample_Array[qq] = 0;
//        Serial.print(Sample_Array[qq]);Serial.print("\t");
        if(Var_Sample == 0) Basic_Signal[qq] =  float(Sample_Array[qq]);   // lần đầu tiên set nền = mẫu signal đầu tiên thu được
        else Basic_Signal[qq] = (Basic_Signal[qq]*4 +  float(Sample_Array[qq])) / 5; 
      }
//      Serial.print("\n");
  }
//  Serial.print("\n");
}
void Process_Led_SG()
{
   Max_Signal = Sample_Array[0];
   for(int i = 0; i <= 50; i++)
   {
     Process_Journey_Led();
     if(Var_Journey_Signal == 1) Journey_Time++;//////////////////////////////////////////////////thoi gian hanh trinh
     for(int ii = 1; ii<=6 ; ii ++)
      {
          Remap_Pin(ii);
          Process_Led();
          Sample_Array[ii] =  (float(Var_LED_IN)-Min_SG[ii])*40/(Max_SG[ii]-Min_SG[ii]);
//          Sample_Array[ii] =  (float(Var_LED_IN));
          if(Sample_Array[ii] <= 3) Sample_Array[ii] = 0;
          Serial.print(Sample_Array[ii]);Serial.print("\t");
          if(Sample_Array[ii] > Max_Signal)  (Max_Signal =  Sample_Array[ii]);///max size cao
          if(Sample_Array[ii] >=5) 
          {
            Volume_Image = (Volume_Image +  Sample_Array[ii]);    //////////////////// the tich
            Area ++; // //////////////////////////////////////////////////////////tinh dien tich
          }
      }
      Serial.print("\n");
    }
    Max_Signal = Max_Signal/2;
    Volume_Image = Volume_Image/10;
    Serial.print("\n");
}
int Compare_Image()
{
  float d = 0;
  float Min_d = sqrt(sq(Volume_Image - Array_Feature[0][1]) + sq(Max_Signal - Array_Feature[0][2]));
  Serial.print("min_d = ");Serial.print(Min_d);Serial.print("\t");
  Serial.print("\n");
  for( int w = 0; w < 20; w++)
  {
    d = sqrt( sq(Volume_Image - Array_Feature[w][1]) + sq(Max_Signal - Array_Feature[w][2]));
    if(d < Min_d) {Min_d = d; Label_Image = Array_Feature[w][4];}
    Serial.print("d = ");Serial.print(d);Serial.print("\t");
    Serial.print("min_d = ");Serial.print(Min_d);Serial.print("\t");
    Serial.print("loai = ");Serial.print(Label_Image);Serial.print("\t");
    Serial.print("\n");
  }
  return Label_Image;
}
void Label_On_Led(int LABEL)
{
  switch(LABEL)
  {
    case 3:{Change_Led_STT(Triangular_LED);break;}
    case 4:{Change_Led_STT(Square_LED);break;}
    case 5:{Change_Led_STT(Circle_LED);break;}
  }
}
void Change_Led_STT(int LED)
{
  for(int ee = 0; ee < 10; ee++)
  {
    digitalWrite(LED,LOW); delay(100);
    digitalWrite(LED,HIGH); delay(100);
  }
  digitalWrite(LED,HIGH);
  
}
void Show_Debug()
{
  if(Current_Mode == Trainning_Mode)
  {
    Serial.print("Area \t");Serial.print("Volume_Image \t");Serial.print("Max_Signal \t");
    Serial.print("Journey_Time \t");Serial.print("Imagine_Type \t");Serial.print("\n");
    Serial.print(Area);Serial.print("\t");
    Serial.print(Volume_Image);Serial.print("\t\t");
    Serial.print(Max_Signal);Serial.print("\t\t");
    Serial.print(Journey_Time);Serial.print("\t\t");
    Show_Label(Imagine_Type);
    //Serial.print(Imagine_Type);Serial.print("\t");
    Serial.print("\n");
    
  }
  else if(Current_Mode == Using_Mode)
  {
    Serial.print("Area \t");Serial.print("Volume_Image \t");Serial.print("Max_Signal \t");
    Serial.print("Journey_Time \t");Serial.print("Imagine_Type \t");Serial.print("\n");
    Serial.print(Area);Serial.print("\t");
    Serial.print(Volume_Image);Serial.print("\t\t");
    Serial.print(Max_Signal);Serial.print("\t\t");
    Serial.print(Journey_Time);Serial.print("\t\t");
    Show_Label(Label_Image);
//    Serial.print(Label_Image);
    Serial.print("\n");
  }
}
void Show_Label(int label)
{
  switch(label)
  {
    case 0: Serial.print("KHONG PHAN BIET DUOC");break;
    case 3: Serial.print("CU TOI");break;
    case 4: Serial.print("HINH VUONG");break;
    case 5: Serial.print("HINH TRON");break;
  }
}
void Show_Debug_Save_Min_Max_SG()
{

    Serial.print("Setting STT = "); Serial.print(Setting_STT);Serial.print("\n");
   for( int rr = 1; rr < 7; rr ++)
    {
    Serial.print("Min ");Serial.print(rr);Serial.print(":  ");Serial.print(Min_SG[rr]);Serial.print("\t");
    Serial.print("Max ");Serial.print(rr);Serial.print(":  ");Serial.print(Max_SG[rr]);Serial.print("\n");
    }
}
void Show_Debug_Load_Data_Array()
{
  for(int i = 0; i < 15; i++)
  {
    for( int ii = 0; ii <5; ii++)
    {
      Serial.print(Array_Feature[i][ii]);Serial.print("\t");
    }
    Serial.print("\n");
  }
  Serial.print("xong======================>\n"); 
}
void Show_Basic_Sample()
{
   Serial.print("Basic_Signal \t");
    for( int i = 1; i <= 6; i++)
    {
      Serial.print(Basic_Signal[i]);Serial.print("\t");
    }
    Serial.print("\n");
}
void STT_Init()
{
  STT_Val = 1;
}
void Process_Journey_Led()
{
  digitalWrite(Journey_Led,HIGH); 
  delay(1); 
  Var_Journey_Signal = analogRead(Journey_Signal); 
  digitalWrite(Journey_Led,LOW); 
  delay(2); 
  if(Var_Journey_Signal <= Journey_Mid) {Var_Journey_Signal = 1;} // have thing
  else Var_Journey_Signal = 0;                            // don't have thing
}
void Pin_Init()
{
  pinMode(Led_Signal_1, INPUT);
  pinMode(Led_Signal_2, INPUT);
  pinMode(Led_Signal_3, INPUT);
  pinMode(Led_Signal_4, INPUT);
  pinMode(Led_Signal_5, INPUT);
  pinMode(Led_Signal_6, INPUT);

  pinMode(Led_1, OUTPUT);
  pinMode(Led_2, OUTPUT);
  pinMode(Led_3, OUTPUT);
  pinMode(Led_4, OUTPUT);
  pinMode(Led_5, OUTPUT);
  pinMode(Led_6, OUTPUT);

  pinMode(Journey_Signal,INPUT);
  pinMode(Journey_Led,OUTPUT);
  
  pinMode(Circle_LED,OUTPUT);
  pinMode(Square_LED,OUTPUT);
  pinMode(Triangular_LED,OUTPUT);
  
  digitalWrite(Circle_LED,HIGH);
  digitalWrite(Square_LED,HIGH);
  digitalWrite(Triangular_LED,HIGH);
  
  Serial.begin(115200);
}
void Show(int a)
{
  Serial.println(a);
}
int Key_Mode_Button()
{
  int Temp = 0;
  Temp = analogRead(Button);
  if(Temp <=150 && Temp >= 110) return Using_Mode;
  else if(Temp <=60 && Temp >= 50) return Trainning_Mode;
  else return 0;
}
int Key_Imagine_Button()
{
  int Temp = 0;
  Temp = analogRead(Button);
  if(Temp <=750 && Temp >= 700) return Square_Imagine;
  else if(Temp <=70 && Temp >=62 ) return Circle_Imagine;
  else if(Temp <=35 && Temp >= 25) return Triangular_Imagine;
  else return 0;
}
void Change_STT()
{
  int Temp = 0;
  Temp = analogRead(Button);
//  Serial.println(Temp);
  if(Temp <= 300 && Temp >= 200)
   {
      writeEEPROM(Add_Save_Setting,0);
      Serial.println(" da thay doi trang thai khoi dong lai ngay sau 10s =============================>");
      delay(10000);
      resetFunc();
   }
}
void Save_Feature()
{
  switch(Imagine_Type)
  {
    case Circle_Imagine:{Save_Feature_Circle_Imagine(); break;}
    case Square_Imagine:{Save_Feature_Square_Imagine(); break;}
    case Triangular_Imagine:{Save_Feature_Triangular_Imagine(); break;}
  }
}
void Save_Setting_MixMax()
{
  writeEEPROM(Add_Save_Setting,Setting_STT);
  for( int ee = 1; ee < 7; ee++)
    {
      writeEEPROM(ee + Add_Save_Setting,Min_SG[ee]);
      writeEEPROM(ee + 6 + Add_Save_Setting,Max_SG[ee]);
    }
}
//void Save_Setting_MixMax()
//{
//  writeEEPROM(Add_Save_Setting,14);
//  for( int ee = 1; ee < 7; ee++)
//    {
//      writeEEPROM(ee + Add_Save_Setting,1);
//      writeEEPROM(ee + 6 + Add_Save_Setting,0);
//    }
//}
//int Add_Save = Add_Begin_Save_data;
//void Save_Feature()
//{
//  if(Add_Save <= Add_Save_Finish)
//  {
//    Serial.println("Start");
//    writeEEPROM(Add_Save,1);delay(1);
//    writeEEPROM(Add_Save+1,1);delay(1);
//    writeEEPROM(Add_Save+2,0);delay(1);
//    writeEEPROM(Add_Save+3,2);delay(1);
//    writeEEPROM(Add_Save+4,2);delay(1);
//    Add_Save = Add_Save + 5;
//  }
//  else Add_Save = Add_Begin_Save_data;
//}
int Add_Save_Square = 0, Add_Begin_Save_Square = 0, Add_Save_Square_Finish = 24;
int Add_Save_Circle = 25, Add_Begin_Save_Circle = 25,Add_Save_Circle_Finish = 49;
int Add_Save_Triangular = 50, Add_Begin_Save_Triangular = 50,Add_Save_Triangular_Finish = 74;
void Save_Feature_Square_Imagine()
{
  if(Add_Save_Square <= Add_Save_Square_Finish)
  {
    writeEEPROM(Add_Save_Square,Area);
    writeEEPROM(Add_Save_Square+1,Volume_Image);
    writeEEPROM(Add_Save_Square+2,Max_Signal);
    writeEEPROM(Add_Save_Square+3,Journey_Time);
    writeEEPROM(Add_Save_Square+4,Imagine_Type);  
    Add_Save_Square = Add_Save_Square + 5;
  }
  else Add_Save_Square = Add_Begin_Save_Square;
}
void Save_Feature_Circle_Imagine()
{
  if(Add_Save_Circle <= Add_Save_Circle_Finish)
  {
    writeEEPROM(Add_Save_Circle,Area);
    writeEEPROM(Add_Save_Circle+1,Volume_Image);
    writeEEPROM(Add_Save_Circle+2,Max_Signal);
    writeEEPROM(Add_Save_Circle+3,Journey_Time);
    writeEEPROM(Add_Save_Circle+4,Imagine_Type);  
    Add_Save_Circle = Add_Save_Circle + 5;
  }
  else Add_Save_Circle = Add_Begin_Save_Circle;
}
void Save_Feature_Triangular_Imagine()
{
  if(Add_Save_Triangular <= Add_Save_Triangular_Finish)
  {
    writeEEPROM(Add_Save_Triangular,Area);
    writeEEPROM(Add_Save_Triangular+1,Volume_Image);
    writeEEPROM(Add_Save_Triangular+2,Max_Signal);
    writeEEPROM(Add_Save_Triangular+3,Journey_Time);
    writeEEPROM(Add_Save_Triangular+4,Imagine_Type);  
    Add_Save_Triangular = Add_Save_Triangular + 5;
  }
  else Add_Save_Triangular = Add_Begin_Save_Triangular;
}
int Add_Load = Add_Begin_Save_Square;
void Load_Feature()
{
    Setting_STT = readEEPROM(Add_Save_Setting);
    for( int tt = 1; tt < 7; tt++)
    {
      Min_SG[tt] = readEEPROM(tt + Add_Save_Setting);
      Max_SG[tt] = readEEPROM(tt + 6 + Add_Save_Setting);
    }
    for( int i = 0; i < 15; i++)
    {
        Array_Feature[i][0] = readEEPROM(Add_Load);
        Array_Feature[i][1] = readEEPROM(Add_Load+1);
        Array_Feature[i][2] = readEEPROM(Add_Load+2);
        Array_Feature[i][3] = readEEPROM(Add_Load+3); 
        Array_Feature[i][4] = readEEPROM(Add_Load+4);
        Add_Load = Add_Load + 5; 
    }
    Add_Load = Add_Begin_Save_Square;
}
typedef union{
  float flt;
  byte array[4];
} FloatConverter;
//write
void writeEEPROM(int address, float value)
{
  address = (address * 3)+ address;
  FloatConverter aConverter; //create a new variable of type FloatConverter
  aConverter.flt = value; //set its value (using the float blueprint) to the value of config
  for(byte i = 0; i < 4; i++){
    EEPROM.write(address+i,aConverter.array[i]); //store each of the 4 bytes of aConverter to the EEPROM, accessing them using the byte[4] blueprint
//    Serial.print(address+i);Serial.print("\t");Serial.print(aConverter.array[i]);Serial.println("----------------------");
  };
//    Serial.println();
}
//read
float readEEPROM(int address)
{
    address = (address * 3)+address;
    float value;
  FloatConverter aConverter; //create a new variable of type FloatConverter
  for(byte i = 0; i < 4; i++){
    aConverter.array[i] = EEPROM.read(address+i); //read 4 bytes from the EEPROM to aConverter using the byte[4] blueprint
//    Serial.print(address+i);Serial.print("\t");Serial.print(aConverter.array[i]);Serial.println("++++++++++++++");
  }
  value = aConverter.flt; //set the value of config to the value of aConverter using the float blueprint}
//  Serial.print((int)value);//Serial.println("++++++++++++++");
  return value;
}
//-------------------------------END EEPROM FUNCTION

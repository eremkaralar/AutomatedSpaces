#include <LiquidCrystal.h>
#include <Servo.h>
#include <HCSR04.h>

HCSR04 sensor1(8,9);
HCSR04 sensor2(7,10);
HCSR04 sensor3(13,6);

int const gazpini = A1;

int sirames= 0;
int mekanicimes = 0;
int cikismes = 0;

int sirasay= 0;
int mekanicisay = 0;
int cikissay = 0;
  
int sirakapasite = 25;
int mekanicikapasite = 109;

Servo myservo;
int value;
int secvalue;
int senkdvalue;
double angle;

float sensor=0;
float celsius=0;
float voltage=0;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {

  lcd.begin(16, 2);  
  
  Serial.begin(9600);
  
   myservo.attach(9);
}

void loop() {
  lcd.clear();
  
  int gazyogunluk = analogRead(gazpini);
  
  sensor=analogRead(0);
  voltage=sensor*5000/1024;
  voltage=voltage-500;
  celsius=voltage/50;
  
  sirames = sensor1.dist();
  mekanicimes = sensor2.dist();
  cikismes = sensor3.dist();

  Serial.println("\nSirames:");
  Serial.println(sirames);
  Serial.println("\n");

  Serial.println("\nMekanicimes:");
  Serial.println(mekanicimes);
  Serial.println("\n");

  Serial.println("\nCikismes:");
  Serial.println(cikismes);
  Serial.println("\n");
  
  gazyogunluk = map(gazyogunluk, 300, 750, 0, 100);
  
  Serial.println("\nGaz yogunluk:");
  Serial.println(gazyogunluk);
  Serial.println("\n");

  Serial.println("\nSicaklik:");
  Serial.println(celsius);
  Serial.println("\n");
 
 
  if(gecis(sirames)==true){
    sirasay++;
    mekanicisay++;
  }
  
  if(gecis(mekanicimes)==true){
    mekanicisay++;
    sirasay--;
    
  }
  
  if(gecis(cikismes)==true){
    cikissay++;
    mekanicisay--;
  }
  
  
  int sirayogunluk = 0; 
  int mekaniciyogunluk = 0;
  int havalandirma = 0;
  int klima = 0;
  int beklemesuresi = 0;
  
  
  sirayogunluk = yogunlukHesaplayici(sirasay,sirakapasite);
  mekaniciyogunluk = yogunlukHesaplayici(mekanicisay,mekanicikapasite);
  havalandirma = havalandirmaCalistirici(gazyogunluk);
  klima = klimaCalistirici(celsius);
  beklemesuresi = bekleme(sirasay);
 
  
  value = girisKilit(sirasay,sirakapasite);
  secvalue = girisKilit(mekanicisay,mekanicikapasite);
  senkdvalue = kilitSenkron(value,secvalue);
    
  angle = map(senkdvalue, 0, 1023, 0, 180); 
  //Serial.println(angle);
  myservo.write(angle);  
  
  
  if(sirayogunluk >= 100 || mekaniciyogunluk >= 100){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kapasite dolu.");
    lcd.setCursor(0,1);
    lcd.print("Girmeyiniz!");
    sirasay = sirakapasite;
     if(gecis(mekanicimes)==true){
   
    sirasay--;
  }
    delay(300);
  }
  

  else{
    lcd.setCursor(0, 0);
    lcd.print("Sira:%");
    lcd.setCursor(7,0);
    lcd.print(sirayogunluk);

    lcd.setCursor(0,1);
    lcd.print("Mekanici:%");  
    lcd.setCursor(10,1);
    lcd.print(mekaniciyogunluk);
    Serial.print(mekanicisay);

    delay(750);

    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Hvlndr S.:"); 
    lcd.setCursor(11,0);
    lcd.print(havalandirma);

    lcd.setCursor(0,1);
    lcd.print("Klima S.:");
    lcd.setCursor(10,1);
    lcd.print(klima);

    delay(750);

    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Tahmini bekleme"); 

    lcd.setCursor(0,1);
    lcd.print("suresi:");
    lcd.setCursor(8,1);
    lcd.print(beklemesuresi);
    lcd.setCursor(11,1);
    lcd.print("dk.");

    delay(750);
  }    
}



bool gecis(int mesafe){
  if(mesafe>50){
    return false;
  }
  else if(mesafe <= 50 && mesafe >= 0){
    return true;
  }
  else
    return false;
}


int yogunlukHesaplayici(int kisi,int kapasite){
  int yogunluk = 0;
  yogunluk = ((100*kisi)/(kapasite));
  
  return yogunluk;
}


int havalandirmaCalistirici(int gazyogunluk){
  
  if(gazyogunluk <= 30){
    Serial.print("Havalandirma calismiyor\n");
    return 0;
  }else if(gazyogunluk > 30 && gazyogunluk <= 50){
    //Hava az kirli
    Serial.print("Havalandirma az calisiyor\n");
    return 1;
  }else if(gazyogunluk > 50 && gazyogunluk <= 80){
    //Hava kirli
    Serial.print("Havalandirma calisiyor\n");
    return 2;
  }else if(gazyogunluk > 80){
    //Hava çok kirli
    Serial.print("Havalandirma cok calisiyor\n");
    return 3;
  }else{
    //Error
    Serial.print("Havalandirici cildirmasi!\n");
    return 4;
  }
  
}


int klimaCalistirici(int celsius){
  
  if(celsius >= 28){
    Serial.print("Max sogutma\n");
    return -3;
  }
  else if(celsius < 28 && celsius >= 25){    
    Serial.print("Mid sogutma\n");
    return -2;
  }
  else if(celsius < 25 && celsius >= 22){
    Serial.print("Min sogutma\n");
    return -1;  
  }
  else if(celsius < 22 && celsius >= 20){
    Serial.print("Sogutma/Isitma yok\n");
    return 0;
  }
  else if(celsius < 20 && celsius >= 18){
    Serial.print("Min isitma\n");
    return 1;  
  }
  else if(celsius < 18 && celsius >= 14){
    Serial.print("Mid isitma\n");
    return 2;  
  }
  else if(celsius < 14){
    Serial.print("Max isitma\n");
    return 3;  
  }
  else{
    //Error
    Serial.print("Termometre hatasi!\n");
    return 4;
  }  
}


int girisKilit(int kisi,int kapasite){
  int kilitDegeri = 0;
  
  if((kisi/kapasite)>= 1){
    kilitDegeri = 0;
  }
  else if((kisi/kapasite)< 1){
    kilitDegeri = 500;
  }
  else{
  Serial.print("Turnike cildirdi!");
  }
  return kilitDegeri;
}


int bekleme(int kisi){
  int beklemesuresi = 0;
  beklemesuresi = 1 * kisi;
  Serial.print("Tahmini bekleme suresi ");
  Serial.print(beklemesuresi);
  Serial.print(" dakikadir.");
  return beklemesuresi;
}

int kilitSenkron(int val,int secval){
 int anaval = 500;
  
  if(val == 0 || secval == 0){
    anaval = 0;
  }
   else if(val == 500 && secval == 500){
     anaval = 500;
  }
  else{
    Serial.print("Kilitlerde senkronizasyon hatasi!");
  }

  return anaval;
}

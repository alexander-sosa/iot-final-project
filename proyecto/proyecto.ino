#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include "config.h"

// Environment
int analog_mq135 = 0;
int percentage;
int ven1 = D5;
int ven2 = D6;
int ven3 = D7;
int ven4 = D8;

// RGB
int Red = D0;
int Green = D1;
int Blue = D2;

// mode
bool manual = true;
bool state = false;

// Website Render
String qualityImage, qualityMessage, qualityPercentage, currentMode, modeButton;
String fan1Image, fan1State = "Off", fan1Button;
String fan2Image, fan2State = "Off", fan2Button;
String fan3Image, fan3State = "Off", fan3Button;
String fan4Image, fan4State = "Off", fan4Button;


// Creamos el servidor AsyncWebServer en el puerto 80
AsyncWebServer server(80);

// Getters and setters
void setAirData(){
  analog_mq135 = analogRead(A0);
  Serial.print(analog_mq135, DEC);
  //Serial.println(" ppm");

  percentage = (float((analog_mq135 - 113))/220) * 100;
  //percentage = round(((float)analog_mq135/1023)*100);
  Serial.print("Porcentaje: ");Serial.println(percentage);
  qualityPercentage = String(percentage);

  if(percentage < 33){
    qualityImage = "https://raw.githubusercontent.com/alexander-sosa/iot-final-project/main/img/ok.png";
    qualityMessage = "Air quality is good";   
    if(!manual){
      digitalWrite(ven1, HIGH);
      digitalWrite(ven2, HIGH);
      digitalWrite(ven3, HIGH);
      digitalWrite(ven4, HIGH);
      digitalWrite(Green,HIGH);
      fan1State = "Off";
      fan2State = "Off";
      fan3State = "Off";
      fan4State = "Off";
    }
    digitalWrite(Green,LOW);
    digitalWrite(Red,HIGH);
    digitalWrite(Blue,HIGH);
  }
  else if(percentage < 66){
    qualityImage = "https://raw.githubusercontent.com/alexander-sosa/iot-final-project/main/img/caution.png";
    qualityMessage = "Danger! CO2 values detected.";
    digitalWrite(Green,LOW);
    digitalWrite(Red,LOW);
    digitalWrite(Blue,HIGH);
    if(!manual){
      qualityMessage = "Danger! CO2 values detected. Activating 2 fans...";
      digitalWrite(ven1, LOW);
      digitalWrite(ven2, LOW);
      digitalWrite(ven3, HIGH);
      digitalWrite(ven4, HIGH);
      fan1State = "On";
      fan2State = "On";
      fan3State = "Off";
      fan4State = "Off";
    }
  }
  else{
    qualityImage = "https://raw.githubusercontent.com/alexander-sosa/iot-final-project/main/img/danger.png";
    qualityMessage = "Red Alert! High CO2 values detected.";
    digitalWrite(Red,LOW);
    digitalWrite(Green,HIGH);
    digitalWrite(Blue,HIGH);
    if(!manual){
      qualityMessage = "Red Alert! High CO2 values detected. Activating ALL fans...";
      digitalWrite(ven1, LOW);
      digitalWrite(ven2, LOW);
      digitalWrite(ven3, LOW);
      digitalWrite(ven4, LOW);
      fan1State = "On";
      fan2State = "On";
      fan3State = "On";
      fan4State = "On";
    }
  } 

  if(fan1State == "On"){
    fan1Button = "<a href='/off_fan1' class='btn btn-danger'>Turn off</a>";
    fan1State = "On";
    fan1Image = "https://raw.githubusercontent.com/alexander-sosa/iot-final-project/main/img/fan on.png";
  }
  else if(fan1State == "Off"){
    fan1Button = "<a href='/on_fan1' class='btn btn-success'>Turn on</a>";      
    fan1State = "Off";
    fan1Image = "https://raw.githubusercontent.com/alexander-sosa/iot-final-project/main/img/fan off.png";
  }
  if(fan2State == "On"){
    fan2Button = "<a href='/off_fan2' class='btn btn-danger'>Turn off</a>";
    fan2State = "On";
    fan2Image = "https://raw.githubusercontent.com/alexander-sosa/iot-final-project/main/img/fan on.png";
  }
  else if(fan2State == "Off"){
    fan2State = "Off";
    fan2Image = "https://raw.githubusercontent.com/alexander-sosa/iot-final-project/main/img/fan off.png";
    fan2Button = "<a href='/on_fan2' class='btn btn-success'>Turn on</a>";      
  }
  if(fan3State == "On"){
    fan3State = "On";
    fan3Image = "https://raw.githubusercontent.com/alexander-sosa/iot-final-project/main/img/fan on.png";
    fan3Button = "<a href='/off_fan3' class='btn btn-danger'>Turn off</a>";
  }
  else if(fan3State == "Off"){
    fan3State = "Off";
    fan3Image = "https://raw.githubusercontent.com/alexander-sosa/iot-final-project/main/img/fan off.png";
    fan3Button = "<a href='/on_fan3' class='btn btn-success'>Turn on</a>";      
  }
  if(fan4State == "On"){
    fan4State = "On";
    fan4Image = "https://raw.githubusercontent.com/alexander-sosa/iot-final-project/main/img/fan on.png";
    fan4Button = "<a href='/off_fan4' class='btn btn-danger'>Turn off</a>";
  }
  else if(fan4State == "Off"){
    fan4State = "Off";
    fan4Image = "https://raw.githubusercontent.com/alexander-sosa/iot-final-project/main/img/fan off.png";
    fan4Button = "<a href='/on_fan4' class='btn btn-success'>Turn on</a>";      
  }
  
  if(manual){
    currentMode = "Manual";
    modeButton = "<a href='/off_manual' class='btn btn-warning'>Enable Automatic Mode</a>";
  }
  else{
    fan1Button = "";fan4Button = "";
    fan2Button = "";fan3Button = "";
    currentMode = "Automatic";
    modeButton = "<a href='/on_manual' class='btn btn-warning'>Enable Manual Control</a>";
  }
}

void resetFan(){
  digitalWrite(ven1, HIGH);
  digitalWrite(ven2, HIGH);
  digitalWrite(ven3, HIGH);
  digitalWrite(ven4, HIGH); 
  fan1State = "Off";
  fan2State = "Off";
  fan3State = "Off";
  fan4State = "Off";
}

void resetLED(){
  digitalWrite(Red, HIGH);
  digitalWrite(Green, HIGH);
  digitalWrite(Blue, HIGH);
  
}


String getStateSys(){
  setAirData();
  if(state){
    return "checked";  
  }else{
    return "";  
  }
  
}

String getQualityImage(){
  setAirData();
  return qualityImage;
}

String getQualityMessage(){
  setAirData();
  return qualityMessage;
}

String getQualityPercentage(){
  setAirData();
  return qualityPercentage;
}

String getCurrentMode(){
  setAirData();
  return currentMode;
}

String getModeButton(){
  setAirData();
  return modeButton;
}

String getFan1Image(){
  setAirData();
  return fan1Image;
}

String getFan1State(){
  setAirData();
  return fan1State;
}

String getFan1Button(){
  setAirData();
  return fan1Button;
}

String getFan2Image(){
  setAirData();
  return fan2Image;
}

String getFan2State(){
  setAirData();
  return fan2State;
}

String getFan2Button(){
  setAirData();
  return fan2Button;
}

String getFan3Image(){
  setAirData();
  return fan3Image;
}

String getFan3State(){
  setAirData();
  return fan3State;
}

String getFan3Button(){
  setAirData();
  return fan3Button;
}

String getFan4Image(){
  setAirData();
  return fan4Image;
}

String getFan4State(){
  setAirData();
  return fan4State;
}

String getFan4Button(){
  setAirData();
  return fan4Button;
}

String processor(const String& var)
{
    Serial.println(var);
    setAirData();
    if (var == "checkedState"){
      return getStateSys();
    }
    else if (var == "qualityImage"){
      return getQualityImage();
    }
    else if (var == "qualityMessage"){
       return getQualityMessage();
    }
    else if (var == "qualityPercentage"){
       return getQualityPercentage();
    }
    else if (var == "currentMode"){
       return getCurrentMode();
    }
    else if (var == "modeButton"){
       return getModeButton();
    }
    else if (var == "fan1Image"){
       return getFan1Image();
    }
    else if (var == "fan1State"){
       return getFan1State();
    }
    else if (var == "fan1Button"){
       return getFan1Button();
    }
    else if (var == "fan2Image"){
       return getFan2Image();
    }
    else if (var == "fan2State"){
       return getFan2State();
    }
    else if (var == "fan2Button"){
       return getFan2Button();
    }
    else if (var == "fan3Image"){
       return getFan3Image();
    }
    else if (var == "fan3State"){
       return getFan3State();
    }
    else if (var == "fan3Button"){
       return getFan3Button();
    }
    else if (var == "fan4Image"){
       return getFan4Image();
    }
    else if (var == "fan4State"){
       return getFan4State();
    }
    else if (var == "fan4Button"){
       return getFan4Button();
    }
}


void setup() {
  Serial.begin(9600);
  pinMode(ven1, OUTPUT);
  pinMode(ven2, OUTPUT);
  pinMode(ven3, OUTPUT);
  pinMode(ven4, OUTPUT);
  digitalWrite(ven1, HIGH);
  digitalWrite(ven2, HIGH);
  digitalWrite(ven3, HIGH);
  digitalWrite(ven4, HIGH);
  pinMode(Red, OUTPUT);
  pinMode(Green, OUTPUT);
  pinMode(Blue, OUTPUT);
  digitalWrite(Red, HIGH);
  digitalWrite(Green, HIGH);
  digitalWrite(Blue, HIGH);

  // Iniciamos  SPIFFS
  if(!SPIFFS.begin())
     { Serial.println("ha ocurrido un error al montar SPIFFS");
       return; }
  // conectamos al Wi-Fi
  WiFi.begin(ssid, password);
  // Mientras no se conecte, mantenemos un bucle con reintentos sucesivos
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      // Esperamos un segundo
      Serial.println("Conectando a la red WiFi..");
    }
  Serial.println();
  Serial.println(WiFi.SSID());
  Serial.print("Direccion IP:\t");
  // Imprimimos la ip que le ha dado nuestro router
  Serial.println(WiFi.localIP());

  // Ruta para cargar el archivo index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/index.html", String(), false, processor);
            });
            
  // Ruta para cargar el archivo style.css
  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/styles.css", "text/css");
            });  

  server.on("/on_fan1", HTTP_GET, [](AsyncWebServerRequest *request){
            if(manual){
              fan1State = "On";
              digitalWrite(ven1, LOW);
            }
            request->send(SPIFFS, "/index.html", String(), false, processor);
            });
  server.on("/on_fan2", HTTP_GET, [](AsyncWebServerRequest *request){
             if(manual){
              fan2State = "On";
              digitalWrite(ven2, LOW);
             }
            request->send(SPIFFS, "/index.html", String(), false, processor);
            });
  server.on("/on_fan3", HTTP_GET, [](AsyncWebServerRequest *request){
             if(manual){
              fan3State = "On";
              digitalWrite(ven3, LOW);
             }
            request->send(SPIFFS, "/index.html", String(), false, processor);
            });
  server.on("/on_fan4", HTTP_GET, [](AsyncWebServerRequest *request){
             if(manual){
              fan4State = "On";
              digitalWrite(ven4, LOW);
             }
            request->send(SPIFFS, "/index.html", String(), false, processor);
            });



  server.on("/off_fan1", HTTP_GET, [](AsyncWebServerRequest *request){
            if(manual){
              fan1State = "Off";
              digitalWrite(ven1, HIGH);
            }
            request->send(SPIFFS, "/index.html", String(), false, processor);
            });
  server.on("/off_fan2", HTTP_GET, [](AsyncWebServerRequest *request){
            if(manual){
              fan2State = "Off";
              digitalWrite(ven2, HIGH);
            }
            request->send(SPIFFS, "/index.html", String(), false, processor);
            });
  server.on("/off_fan3", HTTP_GET, [](AsyncWebServerRequest *request){
            if(manual){
              fan3State = "Off";
              digitalWrite(ven3, HIGH);
            }
            request->send(SPIFFS, "/index.html", String(), false, processor);
            });
  server.on("/off_fan4", HTTP_GET, [](AsyncWebServerRequest *request){
            if(manual){
              fan4State = "Off";
              digitalWrite(ven4, HIGH);
            }
            request->send(SPIFFS, "/index.html", String(), false, processor);
            });

  server.on("/on_manual", HTTP_GET, [](AsyncWebServerRequest *request){
            manual = true;
            resetFan();
            request->send(SPIFFS, "/index.html", String(), false, processor);
            });
  server.on("/off_manual", HTTP_GET, [](AsyncWebServerRequest *request){
            manual = false;
            resetFan();
            request->send(SPIFFS, "/index.html", String(), false, processor);
            });

  server.on("/qualityMessage", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send_P(200, "text/plain", getQualityMessage().c_str());
            });

  server.on("/qualityPercentage", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send_P(200, "text/plain", getQualityPercentage().c_str());
            });

  server.on("/currentMode", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send_P(200, "text/plain", getCurrentMode().c_str());
            });

  server.on("/qualityImage", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send_P(200, "text/plain", getQualityImage().c_str());
            });

  server.on("/fan1Image", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send_P(200, "text/plain", getFan1Image().c_str());
            });

  server.on("/fan2Image", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send_P(200, "text/plain", getFan2Image().c_str());
            });

  server.on("/fan3Image", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send_P(200, "text/plain", getFan3Image().c_str());
            });

  server.on("/fan4Image", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send_P(200, "text/plain", getFan4Image().c_str());
            });

  server.on("/fan1State", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send_P(200, "text/plain", getFan1State().c_str());
            });

  server.on("/fan2State", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send_P(200, "text/plain", getFan2State().c_str());
            });

  server.on("/fan3State", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send_P(200, "text/plain", getFan3State().c_str());
            });

  server.on("/fan4State", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send_P(200, "text/plain", getFan4State().c_str());
            });

  server.on("/system_on", HTTP_GET, [](AsyncWebServerRequest *request){
            state = true;
            request->send(SPIFFS, "/index.html", String(), false, processor);
            });

  server.on("/system_off", HTTP_GET, [](AsyncWebServerRequest *request){
            state = false;
            resetFan();
            resetLED();
            request->send(SPIFFS, "/index.html", String(), false, processor);
            });

  server.begin();
}

void loop() {
  /*
  //aire
  if(analog_mq135 <= 110){
     Serial.println("Aire normal");
     digitalWrite(Red,HIGH);
     digitalWrite(Green,LOW);
     digitalWrite(Blue,LOW);
     digitalWrite(ven1,HIGH);
     delay(50);
  }else{
    digitalWrite(ven1,LOW);  
    digitalWrite(ven2,LOW);  
    digitalWrite(ven3,LOW);  
    digitalWrite(ven4,LOW);  
  }
  */
 

}


// 350 calidad alta
// 350 - 500 calidad buena
// 500 - 800 calidad moderada
// 800 - 1200 calidad mala

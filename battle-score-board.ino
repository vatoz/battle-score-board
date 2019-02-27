
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <Adafruit_NeoPixel.h>

#ifndef STASSID
  #define STASSID "Vatoz"
  #define STAPSK  "borovice"
#endif

#define STRIP_LL 0
#define STRIP_LH 1
#define STRIP_RL 2
#define STRIP_RH 3
#define STRIP_GUI 4
#define STRIP_GUI2 5


#define ColoRand (random(15)<<16)+(random(15)<<8)+random(15)

const char *ssid = STASSID;
const char *password = STAPSK;


unsigned char val_left=0;
unsigned char val_right=0;
unsigned char val_foul_left=0;
unsigned char val_foul_right=0;

uint32_t barva_left=(0<<16) + (0<<8)+30;//random
uint32_t barva_right=(20<<16) + (20<<8)+20;


ESP8266WebServer server(80);
Adafruit_NeoPixel rgbWS_ll = Adafruit_NeoPixel(7*8, 0, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rgbWS_lh = Adafruit_NeoPixel(7*8, 2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rgbWS_rl = Adafruit_NeoPixel(7*8, 5, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rgbWS_rh = Adafruit_NeoPixel(7*8, 4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rgbWS_gui = Adafruit_NeoPixel(6*8, 14, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rgbWS_gui2 = Adafruit_NeoPixel(2, 12, NEO_GRB + NEO_KHZ800);


const int led = 13;

void Np_Set(int i,int led, int color){
  switch (i){
    case STRIP_LL:
       rgbWS_ll.setPixelColor(led,color);break;
    case STRIP_LH:
      rgbWS_lh.setPixelColor(led,color);break;
    case STRIP_RH:
      rgbWS_rh.setPixelColor(led,color);break;
    case STRIP_RL:
      rgbWS_rl.setPixelColor(led,color);break;
   case STRIP_GUI:
      rgbWS_gui.setPixelColor(led,color);break;
   
   case STRIP_GUI2:
      rgbWS_gui2.setPixelColor(led,color);break;
    }
}
void Np_Show(int i){
  yield();delay(0);ESP.wdtFeed();
  switch (i){
    case STRIP_LL:
       rgbWS_ll.show();break;
    case STRIP_LH:
      rgbWS_lh.show();break;
    case STRIP_RH:
      rgbWS_rh.show();break;
    case STRIP_RL:
      rgbWS_rl.show();break;
   case STRIP_GUI:
      rgbWS_gui.show();break;
   
   case STRIP_GUI2:
      rgbWS_gui2.show();break;
    }
}



void NastavDigitu (int  port, int  cislo, int  barva, boolean zero=false){
  int mask=0;
  //Serial.print("Sp");
  //Serial.print(cislo);
  switch (cislo){
    case 0:
      if (zero) mask=1+2+4+64+32+16;
      break;
    case 1:
      mask=4+64;      break;
    case 2:
      mask=2+4+8+16+32;      break;
    case 3:
      mask= 2+8+32+4+64;      break;
    case 4:
      mask=1+8+4+64; break;
    case 5:
      mask = 2+1+8+64+32; break;
    case 6:
      mask=1+2+8+16+32+64; break;
    case 7:
      mask=2+4+64; break;
    case 8:
      mask =1+2+4+8+16+32+64;  break;
    case 9:
      mask=1+2+4+8+32+64; break;
    }
    
  for(int i=0;i<7;i++){
    
      for (int j=0;j<8;j++){
        Np_Set(port,(i*8)+j ,(mask & 1)?    barva:0);
        }
    mask=mask>>1;
    yield();
      }
    
  // aktualizace barev na všech modulech
  yield();
  Np_Show(port);
  

  }
void fauly(){
  int mask=0;
  
  if (val_foul_right==3)mask+=1;
  if (val_foul_right>1)mask+=2; 
  if(val_foul_right>0)mask+=4;
  if (val_foul_left==3)mask+=8; 
  if(val_foul_left>1)mask+=16;
  if(val_foul_left>0)mask+=32;     
for(int i=0;i<6;i++){  
      for (int j=0;j<8;j++){
        Np_Set(STRIP_GUI,(i*8)+j ,(mask & 1)?    20<<16:0);
        }
    mask=mask>>1;
   
      }
   Np_Show(STRIP_GUI);
  
  }
void Vykresli(){
  NastavDigitu(STRIP_LH,(val_left- (val_left % 10  ))/10,barva_left, false );
  NastavDigitu(STRIP_LL, (val_left % 10  ),barva_left, true );
  NastavDigitu(STRIP_RH,(val_right- (val_right % 10  ))/10,barva_right, false );
  NastavDigitu(STRIP_RL, (val_right % 10  ),barva_right, true );
fauly();
  

}


void handleRoot() {
  digitalWrite(led, 1);
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  String out =  "<html>  <head>  <meta http-equiv='content-type' content='text/html; charset=utf-8'>     <meta http-equiv='refresh' content='5'/><link rel='stylesheet' type='text/css' href='/milligram.css' />   <title>Tabule Improliga</title>      </head>";
  out+= "<table border=1>\n";
  out += "<tr><td>Kdo:</td><td><h2>Vlevo</h2></td><td><h2>Vpravo</h2></td></tr>\n";
  out += "<tr><td>Body:</td><td>"+String(val_left)+"</td><td>"+String(val_right)+"</td></tr>\n";
  out += "<tr><td>Uprav body+:</td><td>"+Tlacitko("/l_bod","Přidat bod  levému")+"</td><td>"+Tlacitko("/p_bod","Přidat bod Pravému")+"</td></tr>\n";
  out += "<tr><td>Uprav body-:</td><td>"+Tlacitko("/l_minbod","Ubrat bod levému")+"</td><td>"+Tlacitko("/p_minbod","Ubrat bod Pravému")+"</td></tr>\n";
  out += "<tr><td>Fauly:</td><td>"+String(val_foul_left)+"</td><td>"+String(val_foul_right)+"</td></tr>\n";
  out += "<tr><td>Uprav fauly+:</td><td>"+Tlacitko("/l_faul",(val_foul_left<3)?"Přidat levému":"Přidat vítězný bod pravému týmu")+"</td><td>"+Tlacitko("/p_faul",(val_foul_left<3)?"Přidat pravému":"Přidat vítězný bod levému týmu")+"</td></tr>\n";
  out += "<tr><td>Uprav fauly-:</td><td>"+Tlacitko("/l_minfaul","Ubrat levému")+"</td><td>"+Tlacitko("/p_minfaul","Ubrat Pravému")+"</td></tr>\n";
  //out += "<tr><td>Barva:</td><td>"+Tlacitko("/l_barva","Nastav barvu l")+"</td><td>"+Tlacitko("/p_barva","Nastav barvu p")+"</td></tr>\n";
  //out += "<tr><td>Barva:</td><td>"+Tlacitko("/l_barva","Levý je zlatý")+"</td><td>"+Tlacitko("/p_barva","Nastav barvu p")+"</td></tr>\n";
  
  
  out += "</table>\n";
  out += Tlacitko("/prohodit","Prohodit") + " <br>\n";
   out += Tlacitko("/kis","Dnes hraje KIŠ") + " <br>\n";
  out += Tlacitko("/vip","Dnes hraje VIP") + " <br>\n";

   out +="</body></html>";
  server.send(200, "text/html", out);
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}


void rozsvit (int port,int pocet){
for(int i=0;i<pocet;i++){
    Np_Set(port,i, ColoRand);    
     }
      Np_Show(port);
   
  }
  void test_ada(){
    rozsvit(STRIP_LL,7*8);
      rozsvit(STRIP_LH,7*8);
      rozsvit(STRIP_RL,7*8);
      rozsvit(STRIP_RH,7*8);
      rozsvit(STRIP_GUI,6*8);
      rozsvit(STRIP_GUI2,2);
        
    }
    int buffer;
    void test_nr(){
  
   NastavDigitu(STRIP_LH, buffer,ColoRand, true );
  NastavDigitu(STRIP_LL, random(10),ColoRand, true );
  NastavDigitu(STRIP_RH, random(10),ColoRand, true );
  NastavDigitu(STRIP_RL, random(10),ColoRand, true );
  delay(1500);

  buffer++;
  if(buffer>9)buffer=0;
  }
      
void setup(void) {
 Serial.begin(115200);
 rgbWS_lh.begin();
 rgbWS_ll.begin();
 rgbWS_rh.begin();
 rgbWS_rl.begin();
 rgbWS_gui.begin();
 rgbWS_gui2.begin();
     ESP.wdtDisable();
     Serial.println("Setup");
  test_ada();

  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    ESP.wdtFeed();
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("improliga")) {
    Serial.println("MDNS responder started");
    MDNS.addService("http", "tcp", 80);
  }

  server.on("/", handleRoot);

  server.on("/l_bod", []() { val_left++;  go_home();  });
  server.on("/p_bod", []() { val_right++;  go_home();  });
  server.on("/l_minbod", []() {if(val_left>0) val_left--;  go_home();  });
  server.on("/p_minbod", []() { if(val_right>0) val_right--;  go_home();  });


  server.on("/l_faul", []() { val_foul_left++; if(val_foul_left==4){val_foul_left=0;val_right++;}  go_home();  });
  server.on("/p_faul", []() { val_foul_right++;  if(val_foul_right==4){val_foul_right=0;val_left++;} go_home();  });
  server.on("/l_minfaul", []() {if(val_foul_left>0) val_foul_left--;  go_home();  });
  server.on("/p_minfaul", []() { if(val_foul_right>0) val_foul_right--;  go_home();  });
  server.on("/kis", []() { barva_left=(6<<16) + (21<<8)+24;  go_home();  });
  server.on("/vip", []() { barva_left=(25<<16) + (20<<8)+00;  go_home();  });


  server.on("/milligram.css", milligramCss);
  server.on("/prohodit", []() {
    int pom=val_right;
    val_right=val_left;
    val_left=pom;

    pom=val_foul_right;
    val_foul_right=val_foul_left;
    val_foul_left=pom;

    uint32_t hold=barva_right;
    barva_right=barva_left;
    barva_left=hold;

    go_home();
  });

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

}

void go_home(){
   server.sendHeader("Location", "/",true); //Redirect to our html web page
   server.send(302, "text/plane","");
   Vykresli();
}


void loop(void) {
//Serial.println("!loop");  
//test_ada();
//test_nr();  
//yield();


  server.handleClient();
  MDNS.update();
  Vykresli();
}

String Tlacitko(String href,String popis){
  return "<A class=button href=\"" + href + "\">" + popis + "</A>\n";
}




void milligramCss() {
  server.send(200, "text/css", "\n*,*:after,*:before{box-sizing:inherit}html{box-sizing:border-box;font-size:62.5%}body{color:#606c76;font-family:'Roboto', 'Helvetica Neue', 'Helvetica', 'Arial', sans-serif;font-size:1.6em;font-weight:300;letter-spacing:.01em;line-height:1.6}blockquote{border-left:0.3rem solid #d1d1d1;margin-left:0;margin-right:0;padding:1rem 1.5rem}blockquote *:last-child{margin-bottom:0}.button,button,input[type='button'],input[type='reset'],input[type='submit']{background-color:#9b4dca;border:0.1rem solid #9b4dca;border-radius:.4rem;color:#fff;cursor:pointer;display:inline-block;font-size:1.1rem;font-weight:700;height:3.8rem;letter-spacing:.1rem;line-height:3.8rem;padding:0 3.0rem;text-align:center;text-decoration:none;text-transform:uppercase;white-space:nowrap}.button:focus,.button:hover,button:focus,button:hover,input[type='button']:focus,input[type='button']:hover,input[type='reset']:focus,input[type='reset']:hover,input[type='submit']:focus,input[type='submit']:hover{background-color:#606c76;border-color:#606c76;color:#fff;outline:0}.button[disabled],button[disabled],input[type='button'][disabled],input[type='reset'][disabled],input[type='submit'][disabled]{cursor:default;opacity:.5}.button[disabled]:focus,.button[disabled]:hover,button[disabled]:focus,button[disabled]:hover,input[type='button'][disabled]:focus,input[type='button'][disabled]:hover,input[type='reset'][disabled]:focus,input[type='reset'][disabled]:hover,input[type='submit'][disabled]:focus,input[type='submit'][disabled]:hover{background-color:#9b4dca;border-color:#9b4dca}.button.button-outline,button.button-outline,input[type='button'].button-outline,input[type='reset'].button-outline,input[type='submit'].button-outline{background-color:transparent;color:#9b4dca}.button.button-outline:focus,.button.button-outline:hover,button.button-outline:focus,button.button-outline:hover,input[type='button'].button-outline:focus,input[type='button'].button-outline:hover,input[type='reset'].button-outline:focus,input[type='reset'].button-outline:hover,input[type='submit'].button-outline:focus,input[type='submit'].button-outline:hover{background-color:transparent;border-color:#606c76;color:#606c76}.button.button-outline[disabled]:focus,.button.button-outline[disabled]:hover,button.button-outline[disabled]:focus,button.button-outline[disabled]:hover,input[type='button'].button-outline[disabled]:focus,input[type='button'].button-outline[disabled]:hover,input[type='reset'].button-outline[disabled]:focus,input[type='reset'].button-outline[disabled]:hover,input[type='submit'].button-outline[disabled]:focus,input[type='submit'].button-outline[disabled]:hover{border-color:inherit;color:#9b4dca}.button.button-clear,button.button-clear,input[type='button'].button-clear,input[type='reset'].button-clear,input[type='submit'].button-clear{background-color:transparent;border-color:transparent;color:#9b4dca}.button.button-clear:focus,.button.button-clear:hover,button.button-clear:focus,button.button-clear:hover,input[type='button'].button-clear:focus,input[type='button'].button-clear:hover,input[type='reset'].button-clear:focus,input[type='reset'].button-clear:hover,input[type='submit'].button-clear:focus,input[type='submit'].button-clear:hover{background-color:transparent;border-color:transparent;color:#606c76}.button.button-clear[disabled]:focus,.button.button-clear[disabled]:hover,button.button-clear[disabled]:focus,button.button-clear[disabled]:hover,input[type='button'].button-clear[disabled]:focus,input[type='button'].button-clear[disabled]:hover,input[type='reset'].button-clear[disabled]:focus,input[type='reset'].button-clear[disabled]:hover,input[type='submit'].button-clear[disabled]:focus,input[type='submit'].button-clear[disabled]:hover{color:#9b4dca}code{background:#f4f5f6;border-radius:.4rem;font-size:86%;margin:0 .2rem;padding:.2rem .5rem;white-space:nowrap}pre{background:#f4f5f6;border-left:0.3rem solid #9b4dca;overflow-y:hidden}pre>code{border-radius:0;display:block;padding:1rem 1.5rem;white-space:pre}hr{border:0;border-top:0.1rem solid #f4f5f6;margin:3.0rem 0}input[type='email'],input[type='number'],input[type='password'],input[type='search'],input[type='tel'],input[type='text'],input[type='url'],textarea,select{-webkit-appearance:none;-moz-appearance:none;appearance:none;background-color:transparent;border:0.1rem solid #d1d1d1;border-radius:.4rem;box-shadow:none;box-sizing:inherit;height:3.8rem;padding:.6rem 1.0rem;width:100%}input[type='email']:focus,input[type='number']:focus,input[type='password']:focus,input[type='search']:focus,input[type='tel']:focus,input[type='text']:focus,input[type='url']:focus,textarea:focus,select:focus{border-color:#9b4dca;outline:0}select{background:url('data:image/svg+xml;utf8,<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"14\" viewBox=\"0 0 29 14\" width=\"29\"><path fill=\"#d1d1d1\" d=\"M9.37727 3.625l5.08154 6.93523L19.54036 3.625\"/></svg>') center right no-repeat;padding-right:3.0rem}select:focus{background-image:url('data:image/svg+xml;utf8,<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"14\" viewBox=\"0 0 29 14\" width=\"29\"><path fill=\"#9b4dca\" d=\"M9.37727 3.625l5.08154 6.93523L19.54036 3.625\"/></svg>')}textarea{min-height:6.5rem}label,legend{display:block;font-size:1.6rem;font-weight:700;margin-bottom:.5rem}fieldset{border-width:0;padding:0}input[type='checkbox'],input[type='radio']{display:inline}.label-inline{display:inline-block;font-weight:normal;margin-left:.5rem}.container{margin:0 auto;max-width:112.0rem;padding:0 2.0rem;position:relative;width:100%}.row{display:flex;flex-direction:column;padding:0;width:100%}.row.row-no-padding{padding:0}.row.row-no-padding>.column{padding:0}.row.row-wrap{flex-wrap:wrap}.row.row-top{align-items:flex-start}.row.row-bottom{align-items:flex-end}.row.row-center{align-items:center}.row.row-stretch{align-items:stretch}.row.row-baseline{align-items:baseline}.row .column{display:block;flex:1 1 auto;margin-left:0;max-width:100%;width:100%}.row .column.column-offset-10{margin-left:10%}.row .column.column-offset-20{margin-left:20%}.row .column.column-offset-25{margin-left:25%}.row .column.column-offset-33,.row .column.column-offset-34{margin-left:33.3333%}.row .column.column-offset-50{margin-left:50%}.row .column.column-offset-66,.row .column.column-offset-67{margin-left:66.6666%}.row .column.column-offset-75{margin-left:75%}.row .column.column-offset-80{margin-left:80%}.row .column.column-offset-90{margin-left:90%}.row .column.column-10{flex:0 0 10%;max-width:10%}.row .column.column-20{flex:0 0 20%;max-width:20%}.row .column.column-25{flex:0 0 25%;max-width:25%}.row .column.column-33,.row .column.column-34{flex:0 0 33.3333%;max-width:33.3333%}.row .column.column-40{flex:0 0 40%;max-width:40%}.row .column.column-50{flex:0 0 50%;max-width:50%}.row .column.column-60{flex:0 0 60%;max-width:60%}.row .column.column-66,.row .column.column-67{flex:0 0 66.6666%;max-width:66.6666%}.row .column.column-75{flex:0 0 75%;max-width:75%}.row .column.column-80{flex:0 0 80%;max-width:80%}.row .column.column-90{flex:0 0 90%;max-width:90%}.row .column .column-top{align-self:flex-start}.row .column .column-bottom{align-self:flex-end}.row .column .column-center{-ms-grid-row-align:center;align-self:center}@media (min-width: 40rem){.row{flex-direction:row;margin-left:-1.0rem;width:calc(100% + 2.0rem)}.row .column{margin-bottom:inherit;padding:0 1.0rem}}a{color:#9b4dca;text-decoration:none}a:focus,a:hover{color:#606c76}dl,ol,ul{list-style:none;margin-top:0;padding-left:0}dl dl,dl ol,dl ul,ol dl,ol ol,ol ul,ul dl,ul ol,ul ul{font-size:90%;margin:1.5rem 0 1.5rem 3.0rem}ol{list-style:decimal inside}ul{list-style:circle inside}.button,button,dd,dt,li{margin-bottom:1.0rem}fieldset,input,select,textarea{margin-bottom:1.5rem}blockquote,dl,figure,form,ol,p,pre,table,ul{margin-bottom:2.5rem}table{border-spacing:0;width:100%}td,th{border-bottom:0.1rem solid #e1e1e1;padding:1.2rem 1.5rem;text-align:left}td:first-child,th:first-child{padding-left:0}td:last-child,th:last-child{padding-right:0}b,strong{font-weight:bold}p{margin-top:0}h1,h2,h3,h4,h5,h6{font-weight:300;letter-spacing:-.1rem;margin-bottom:2.0rem;margin-top:0}h1{font-size:4.6rem;line-height:1.2}h2{font-size:3.6rem;line-height:1.25}h3{font-size:2.8rem;line-height:1.3}h4{font-size:2.2rem;letter-spacing:-.08rem;line-height:1.35}h5{font-size:1.8rem;letter-spacing:-.05rem;line-height:1.5}h6{font-size:1.6rem;letter-spacing:0;line-height:1.4}img{max-width:100%}.clearfix:after{clear:both;content:' ';display:table}.float-left{float:left}.float-right{float:right}\n\n");
}

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "...";
const char* password = "...";

ESP8266WebServer server(80);


ESP8266WiFiMulti WiFiMulti;

HTTPClient http;

bool is_authentified() {
  Serial.println("Enter is_authentified");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;
}

void handleCadastrar(){
  Serial.println("Enter handleCadastrar");
  String msg;
  String header;
  if (!is_authentified()) {
    server.sendHeader("Location", "/Login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  /*
  String content = "<html><body><H2>AQUI SERA CADASTRADO UM USUARIO! com RFID : " + server.arg("RFID")+" </H2><br>";
  if (server.hasHeader("User-Agent")) {
    content += "conectado com o dispositivo: " + server.header("User-Agent") + "<br><br>";
  }*/
  /*
  content +="<form action='/Confirma' method='POST'>Cadastre um novo usuario<br>";
  content += "Nome:<input type='text' name='NOME' placeholder='nome'><br>";
  content += "Sobrenome:<input type='text' name='SOBRENOME' placeholder='sobrenome'><br>";
  content += "Cpf:<input type='text' name='CPF' placeholder='cpf'><br>";
  content += "RFID: <input type='text' name='RFID' value='" + server.arg("RFID")+ "' readonly><br> ";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";

  content += "Clique aqui para fazer <a href=\"/Login?DISCONNECT=YES\">logout</a></body></html>";
  */
  String content = "<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  content += "<style>\nbody {font-family: Arial, Helvetica, sans-serif;}\n input[type=text],input[type=password]{width:100%; padding:12px 20px ;margin:8px 0 ;display: inline-block; border: 1px solid #ccc; box-sizing:border-box; }";
  content += "button{background-color:rgb(83,7,7);color:white;padding:14px 20px;margin:8px 0;border: none;cursor:pointer;width:100%;display: inline-block;}";
  content += ".container{padding:16px;}";
  content += ".modal{display:none;position:fixed;z-index:1;left:0;top:0;width:100%;heigth:100%;overflow:auto;background-color:rgb(0,0,0);background-color:rgba(0,0,0,0.4);padding-top:50px;}";
  content += ".modal-content{background-color:#fefefe;margin:5% auto 15% auto;border:1px solid #888;width:80%;}</style>";
  content += "</head>";
  content += "<body><h1>Cadastro de Usuarios</h1>";
  content += "<form class='modal-content animate' action='/Confirma' method='POST'><div class='container'>";
  content += "<label for='nome'><b>Nome</b></label><input type='text' name='NOME' required>";
  content += "<label for='sobrenome'><b>Sobrenome</b></label><input type='text' name='SOBRENOME' required>";
  content += "<label for='cpf'><b>CPF</b></label><input type='text' name='CPF' required>";
  content += "<label for='rfid'><b>ID</b></label><input type='text' name='RFID' value='"+server.arg("RFID")+"' readonly>";
  content += "<button type='submit' name='SUBMIT' href='/Confirma'>CADASTRAR</button></div></form>";
  content += "<form class='modal-content animate' action='/RFIDRead' method='GET'><div class='container'><button type='submit' name='SUBMIT' href='/RFIDRead'>Cancelar</button>";
  content += "</div></form></body></html>";
  
  server.send(200, "text/html", content);
  
}

boolean makePost(String rfid, String nome, String sobrenome, String cpf){
  boolean b=false;
  String payload="cpf="+cpf+"&codigoIdentificacao="+rfid+"&nome="+nome+"&sobrenome="+sobrenome;

  if((WiFiMulti.run()==WL_CONNECTED)){
    Serial.print("[HTTP] begin...\n");
    http.begin("http://openmyway.herokuapp.com/usuario/cadastrar");
    Serial.print("[HTTP] POST...\n");
    http.addHeader("Content-Type","application/x-www-form-urlencoded");
    http.addHeader("Content-Lenght",String(payload.length()));
    int httpCode= http.POST(payload);

    if(httpCode>0){
      Serial.printf("[HTTP] POST ... Response Code: %d\n",httpCode);
      if(httpCode==HTTP_CODE_OK){
        String payload = http.getString();
        Serial.println(payload);
        b=true;
      }
      
    }else{
      Serial.printf("[HTTP] POST... failed, Error: %s \n", http.errorToString(httpCode).c_str());
    }
  
    http.end();
    delay(10000);
  }
  return b;
}

void handleConfirma(){
  Serial.println("Enter handleConfirma");
  String header;

  if(!is_authentified()){
    server.sendHeader("Locaton","/Login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  if(server.hasArg("NOME")&&server.hasArg("SOBRENOME")&&server.hasArg("CPF")&&server.hasArg("RFID")){
    if(makePost(server.arg("RFID"),server.arg("NOME"),server.arg("SOBRENOME"),server.arg("CPF"))){
      Serial.println("FEITO O POST");
      String content = "<html><body><H2> CADASTRO FEITO COM SUCESSO</H2><br>";
      content+="NOME: "+server.arg("NOME")+"<br>";
      content+="SOBRENOME: "+server.arg("SOBRENOME")+ "<br>";
      content+="CPF: " +server.arg("CPF")+"<br>";
      content+="RFID: "+server.arg("RFID")+"<br>";
      content+="<a href=\"/RFIDRead\">NOVO USUARIO</a> ou <a href=\"/Login?DISCONNECT=YES\">SAIR<\a></body></html>";
      server.send(200,"text/html",content);
      return;
    }
  }
    String content = "<html><body><H2> ERRO AO CADASTRAR  USUARIO</H2></body></html>";
   server.send(200,"text/html",content); 
}

void handleRFID(){
  Serial.println("Enter handleRFID");
  String header;

  if(!is_authentified()){
    server.sendHeader("Location", "/Login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  String content = "<html><body><H2> PASSE O CARTAO RFID PARA OBTER LEITURA</H2><br>";
  
  content += "Clique aqui para adicionar um novo <a href=\"/Cadastrar?RFID=ID8266AFF\">RFID</a></body></html>";
  
  server.send(200,"text/html",content);
}

void handleLogin(){
String msg;
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    server.sendHeader("Location", "/Login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
    if (server.arg("USERNAME") == "admin" &&  server.arg("PASSWORD") == "admin") {
      server.sendHeader("Location", "/RFIDRead");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
      server.send(301);
      Serial.println("Log in Successful");
      return;
    }
    msg = "Wrong username/password! try again.";
    Serial.println("Log in Failed");
  }/*
  String content = "<html><body><form action='/Login' method='POST'>To log in, please use : admin/admin<br>";
  content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
  content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";*/
  String content = "<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  content += "<style>\nbody {font-family: Arial, Helvetica, sans-serif;}\n input[type=text],input[type=password]{width:100%; padding:12px 20px ;margin:8px 0 ;display: inline-block; border: 1px solid #ccc; box-sizing:border-box; }";
  content += "button{background-color:rgb(83,7,7);color:white;padding:14px 20px;margin:8px 0;border: none;cursor:pointer;width:100%;}";
  content += ".container{padding:16px;}";
  content += ".modal{display:none;position:fixed;z-index:1;left:0;top:0;width:100%;heigth:100%;overflow:auto;background-color:rgb(0,0,0);background-color:rgba(0,0,0,0.4);padding-top:50px;}";
  content += ".modal-content{background-color:#fefefe;margin:5% auto 15% auto;border:1px solid #888;width:80%;}</style>";
  content += "</head>";
  content += "<body><h1>Administrador Loyola</h1>";
  content += "<form class=\"modal-content animate\">";
  content += "<div class=\"container\"><label for=\"uname\"><b>Usuario</b></label><input type='text' name='USERNAME' required>";
  content += "<label for=\"psw\"><b>Senha</b></label><input type='password' name='PASSWORD' required> ";
  content += "<button type='submit' name='SUBMIT'>Login</button>"+msg+"</div></form></body></html>";

  
  server.send(200, "text/html", content);

  
}

void handleRoot() {
  Serial.println("Enter handleRoot");
  String header;
  if (!is_authentified()) {
    server.sendHeader("Location", "/Login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }/*
  String content = "<html><body>PASSE O CARTAO RFID NO SENSOR PARA REALIZAR LEITURA<H2></H2><br>";
  if (server.hasHeader("User-Agent")) {
    content += "the user agent used is : " + server.header("User-Agent") + "<br><br>";
  }
  content += "You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></body></html>";
  server.send(200, "text/html", content);*/
    server.sendHeader("Location", "/RFIDRead");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
}



void handleNotFound() {
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
}

void setup() {
Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  server.on("/", handleRoot);
  server.on("/Login", handleLogin);
  server.on("/RFIDRead", handleRFID);
  server.on("/Cadastrar",handleCadastrar);
  server.on("/Confirma",handleConfirma);


  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize);
  server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  server.handleClient();

}

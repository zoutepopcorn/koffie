#include <WiFiManager.h>  
// webupdate
const char* form = "<form method='POST' action='/edit' enctype='multipart/form-data'><input type='file' name='upload'><input type='submit' value='upload'></form>";
// 
const char* upd = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

File fsUploadFile;
const char* host = "koffie";

void web() {
      SPIFFS.begin();
      WiFiManager wifiManager;
      wifiManager.setTimeout(180);
      if(!wifiManager.autoConnect("koffie", "aaaaaaaa")) {
          delay(3000);
          ESP.reset();
          delay(5000);
      }
      MDNS.begin(host);
      server.onNotFound([](){
        String url = "" + server.uri();
        if(!handleFileRead(url)) {
            server.send(200, "text/html", "..<br>");
        }
      });

      server.on("/edit", HTTP_POST, [](){ server.send(200, "text/plain", "yep"); }, handleFileUpload);
      
      server.on("/form", HTTP_GET, [](){ server.send(200, "text/html", form); }, handleFileUpload);

      server.on("/up", HTTP_GET, [](){ server.send(200, "text/html", upd); }, handleFileUpload);

      server.on("/update", HTTP_POST, [](){
            server.sendHeader("Connection", "close");
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
            ESP.restart();
      },[](){
            HTTPUpload& upload = server.upload();
            if(upload.status == UPLOAD_FILE_START){
//              Serial.setDebugOutput(true);
              WiFiUDP::stopAll();
//              Serial.printf("Update: %s\n", upload.filename.c_str());
              uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
              if(!Update.begin(maxSketchSpace)){//start with max available size
                  Update.printError(Serial);
              }
            } else if(upload.status == UPLOAD_FILE_WRITE){
              if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
                    Update.printError(Serial);
              }
            } else if(upload.status == UPLOAD_FILE_END){
              if(Update.end(true)){ //true to set the size to the current progress
                Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
              } else {
                Update.printError(Serial);
              }
//              Serial.setDebugOutput(false);
            }
            yield();
      });

      server.begin();
      MDNS.addService("http", "tcp", 80);
}

bool handleFileRead(String path){
      if(path.endsWith("/")) path += "index.htm";
      if(SPIFFS.exists(path)){
          File file = SPIFFS.open(path, "r");
          String contentType = getContentType(path);
          size_t sent = server.streamFile(file, contentType);
          file.close();
          return true;
      }
      return false;
}

void handleFileUpload(){
  HTTPUpload& upload = server.upload();
    if(upload.status == UPLOAD_FILE_START){
      String filename = upload.filename;
      if(!filename.startsWith("/")) filename = "/"+filename;
      fsUploadFile = SPIFFS.open(filename, "w");
      filename = String();
    } else if(upload.status == UPLOAD_FILE_WRITE){
      //DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
      if(fsUploadFile)
        fsUploadFile.write(upload.buf, upload.currentSize);
    } else if(upload.status == UPLOAD_FILE_END){
      if(fsUploadFile)
        fsUploadFile.close();
    }
}


String getContentType(String filename){
    if(server.hasArg("download")) return "application/octet-stream";
    else if(filename.endsWith(".htm")) return "text/html";
    else if(filename.endsWith(".html")) return "text/html";
    else if(filename.endsWith(".css")) return "text/css";
    else if(filename.endsWith(".js")) return "application/javascript";
    else if(filename.endsWith(".png")) return "image/png";
    else if(filename.endsWith(".gif")) return "image/gif";
    else if(filename.endsWith(".jpg")) return "image/jpeg";
    else if(filename.endsWith(".ico")) return "image/x-icon";
    else if(filename.endsWith(".xml")) return "text/xml";
    else if(filename.endsWith(".pdf")) return "application/x-pdf";
    else if(filename.endsWith(".zip")) return "application/x-zip";
    else if(filename.endsWith(".gz")) return "application/x-gzip";
    return "text/plain";
}

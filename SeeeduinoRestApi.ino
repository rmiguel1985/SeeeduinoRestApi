#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
 
// Listen to the default port 5555, the Yún webserver
// will forward there all the HTTP requests you send
BridgeServer server;
 
int RELAY3 = 8;
 
void setup() {
  // Bridge startup
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);
  pinMode(RELAY3, OUTPUT);
 
  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();
}
 
void loop() {
  // Get clients coming from server
  BridgeClient client = server.accept();
 
  // There is a new client?
  if (client) {
    // Process request
    process(client);
 
    // Close connection and free resources.
    client.stop();
  }
 
  delay(50); // Poll every 50ms
}
 
void process(BridgeClient client) {
  // read the command
  String command = client.readStringUntil('/');
 
  // is "digital" command?
  if (command == "digital") {
    digitalCommand(client);
  }
 
  // is "status" command?
  if (command == "status") {
    statusCommand(client);
  }
}
 
void digitalCommand(BridgeClient client) {
  int pin, value;
 
  // Read pin number
  pin = client.parseInt();
 
  // If the next character is a '/' it means we have an URL
  // with a value like: "/digital/13/1"
  if (client.read() == '/') {
    value = client.parseInt();
    digitalWrite(pin, value);
  } else {
    value = digitalRead(pin);
  }
 
  // Send feedback to client
  client.print(F("{\"pin\":"));
  client.print(pin);
  client.print(F(" ,\"status\":"));
  client.print(value);
  client.print(F("}"));
 
  // Update datastore key with the current pin value
  String key = "D";
  key += pin;
  Bridge.put(key, String(value));
  
}
 
 
void statusCommand(BridgeClient client) {
  int pin, value;
 
  // Read pin number
  pin = client.parseInt();
  value = digitalRead(pin);
   
  // Send feedback to client
  client.print(F("{\"pin\":"));
  client.print(pin);
  client.print(F(" ,\"status\":"));
  client.print(value);
  client.print(F("}"));
}

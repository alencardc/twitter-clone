#include <string>
#include <iostream>
#include <algorithm>
#include <stdlib.h>

#include "GlobalConfig.hpp"
#include "lib/utils/string.hpp"
#include "lib/socket/TCPClient.hpp"
#include "lib/packet/Packet.hpp"
#include "ConnectionConsumer.hpp"
#include "lib/Queue.hpp"
#include "lib/SyncAccess.hpp"

#include "ui/App.hpp"
#include "ui/Window.hpp"
#include "ui/Label.hpp"
#include "ui/TimeoutLabel.hpp"
#include "ui/TextInput.hpp"
#include "ui/NotificationList.hpp"

#define MAX_SIZE_USERNAME 20
#define MIN_SIZE_USERNAME 4

// "127.0.0.1", 8080
bool tryLogin(std::string username, TCPConnection* connection);
void handleNotificationWindow(App& app, Window& window, NotificationList& list);
void handleCommand(std::string command, TCPConnection* connection);
bool usernameIsValid(std::string username);
TCPConnection* connectToAnyProvider();

int main(int argc, char** argv) {
  if (argc != 2) { 
    printf("Invalid parameters. Usage: %s <username>\n", argv[0]);
    return -1;
  }

  std::string username = argv[1];

  TCPConnection* connection = connectToAnyProvider();
  if (connection == NULL) {
    printf("Unable establish a connection with the server.\n");
    return -1;
  }
  
  if (tryLogin(username, connection) == true) {
    // User Interface (UI) creation -----------------------------
    App app = App();
    int maxY = app.maxY();
    int width = 60; int pY = 2;
    int inputHeight = 5; int sideBoxWidth = 25;

    Window* userWindow = app.createWindow({1, pY}, {sideBoxWidth, 3});
    Label *lName = new Label({1,1}, username.c_str());
    userWindow->addName("  Hello,  ");
    userWindow->addWidget(lName);
    int usageY = userWindow->position().y + userWindow->size().y + pY;
    
    Window* usageWindow = app.createWindow({1, usageY}, {sideBoxWidth, 9});
    usageWindow->addName("  Usage  ");
    Label *lExit = new Label({1,1}, "F1 - Back/Exit");
    Label *lCommand = new Label({1,3}, "c  - Enter command box");
    Label *lFeed = new Label({1,5}, "f  - Enter feed list");
    Label *lSend = new Label({1,7}, "Return - Send command");
    usageWindow->addWidget(lExit); usageWindow->addWidget(lCommand);
    usageWindow->addWidget(lFeed); usageWindow->addWidget(lSend);

    int inputXpos = usageWindow->position().x + usageWindow->size().x + 2*pY;
    Window* inputWindow = app.createWindow({inputXpos, maxY-pY-inputHeight}, {width, inputHeight});
    inputWindow->addName("  Commands  ");

    Window* feedWindow = app.createWindow({inputXpos, pY}, {width, inputWindow->position().y-3});
    feedWindow->addName("  Notifications feed  ");
    
    TextInput* textInput = new TextInput({1,1}, {inputWindow->size().x-2, inputWindow->size().y-2});
    textInput->setMaxSize(Notification::MAX_SIZE);
    inputWindow->addWidget(textInput);

    NotificationList* feedList = new NotificationList({1,1}, {width-2, inputWindow->position().y-3-1});
    feedWindow->addWidget(feedList);

    int responseY = inputWindow->position().y + inputWindow->size().y;
    Window* responseWindow = app.createWindow({inputXpos, responseY}, {width, 2});
    responseWindow->setShowBorder(false);
    TimeoutLabel* tlResponse = new TimeoutLabel({0,0}, "");
    tlResponse->setTimeout(4);
    responseWindow->addWidget(tlResponse);
    // ----------------------------------------------------------
    
    SyncAccess<bool> shouldReconnect;
    shouldReconnect.set(false);
    ConnectionConsumer* packetReceiver = new ConnectionConsumer(
      *connection, *feedList, *tlResponse, shouldReconnect
    );
    packetReceiver->start();

    // Client handle --------------------------------------------
    int screenEvent;
    while ((screenEvent = app.getKeyEvent()) != KEY_F(1)) {
      if (shouldReconnect.get() == true){
        delete packetReceiver; packetReceiver = NULL;
        connection = connectToAnyProvider();
        if (connection != NULL && tryLogin(username, connection) == true) {
          tlResponse->setText("Reconnected!");
          packetReceiver = new ConnectionConsumer(
            *connection, *feedList, *tlResponse, shouldReconnect
          );
          packetReceiver->start();
        } else
          tlResponse->setText("Unable to reconnect.");
        shouldReconnect.set(false);
      }
      
      if (screenEvent == 'c') {
        std::string command = textInput->input(*inputWindow);
        handleCommand(command, connection);
      } else if (screenEvent == 'n') {
        handleNotificationWindow(app, *feedWindow, *feedList);
      }
      app.draw();
    }
  }

  delete connection;
  return 0;
}

bool tryLogin(std::string username, TCPConnection* connection) {
  if (usernameIsValid(username) == false) {
    printf("Invalid username.\n");
    printf("A valid username starts with '@' and has a minimum of 4 characters and maximum of 20 characters.\n");
    return false;
  }

  Packet request = Packet(LOGIN, username.c_str());
  connection->send(&request);
  Packet* response = connection->receive();
  
  if(response->type() != SUCCESS) {
    printf("Unable to login: %s\n", response->payload());
    return false;
  }
  delete response;
  return true;
}

void handleNotificationWindow(App& app, Window& window, NotificationList& list) {
  int event;
  while ((event = window.getKeyEvent()) != KEY_F(1)) {
    switch (event) {
      case KEY_UP:
        list.showPrevious();
        app.draw();
        break;
      case KEY_DOWN: 
        list.showNext();
        app.draw();
        break;
    }
  }
}

void handleCommand(std::string command, TCPConnection* connection) {
  if (command.rfind("SEND ", 0) == 0) {
    std::string message = removePrefix(command, "SEND ");
    Packet packet = Packet(SEND, message.c_str());
    connection->send(&packet);
  } else if (command.rfind("FOLLOW ", 0) == 0) {
    std::string username = removePrefix(command, "FOLLOW ");
    if (usernameIsValid(username)) {
      Packet packet = Packet(FOLLOW, username.c_str());
      connection->send(&packet);
    }
  }
}

bool usernameIsValid(std::string username) {
  if (username[0] != '@') {
    return false;
  }

  return (username.size() >= MIN_SIZE_USERNAME+1 
        && username.size() <= MAX_SIZE_USERNAME+1);
}

TCPConnection* connectToAnyProvider() {
  TCPClient client = TCPClient();
  TCPConnection* connection = NULL;

  srand(time(NULL));
  unsigned tries = 0;
  unsigned idxCurr = rand() % GlobalConfig::FRONTEND_ADDRESSES.size();
  while (connection == NULL && tries < GlobalConfig::FRONTEND_ADDRESSES.size()) {
    unsigned idxAddr = idxCurr % GlobalConfig::FRONTEND_ADDRESSES.size();

    auto addr = GlobalConfig::FRONTEND_ADDRESSES[idxAddr];
    connection = client.connect(addr.ip, addr.port);

    tries += 1;
    idxCurr += 1;
  }

  return connection;
}
#include <string>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include "lib/utils/string.hpp"
#include "lib/socket/TCPClient.hpp"
#include "lib/packet/Packet.hpp"
#include "ConnectionConsumer.hpp"
#include "lib/Queue.hpp"

#include <ncurses.h>

using namespace std;

// "127.0.0.1", 8080

int main(int argc, char** argv) {
  if (argc != 4) { 
    cout << "Invalid parameters. Usage: " << argv[0] << "<ip> <port> <username>" << endl;
  }

  string ip = argv[1];
  int port = atoi(argv[2]);
  string username = argv[3];

  TCPClient* client = new TCPClient();
  TCPConnection* connection = client->connect(ip, port);

  if (connection == NULL) {
    printf("Unable stablish a connection with the server.\n");
    return 1;
  }
  
  Packet loginPacket = Packet(LOGIN, username.c_str());
  connection->send(&loginPacket);

  Packet* packet = connection->receive();
  printf("%s\n", packet->serialize().c_str());
  if (packet->type() == (PacketType)4) {
    printf("Logged in as: %s\n", username.c_str());

    Queue<Packet*> dataQueue;
    ConnectionConsumer packetReceiver = ConnectionConsumer(*connection, dataQueue);
    packetReceiver.start();

    string line; 

    initscr();
    raw();
    halfdelay(5);
    noecho();
    keypad(stdscr, TRUE);
    int row, col;
    getmaxyx(stdscr,row,col);
    //printw("row:%d col:%d", row, col);

    bool exit = false;
    char inputBuff[2];
    inputBuff[1] = '\0';
    std::string notificationsBuffer;
    while (exit == false) {
      int input = getch();

      if (input != ERR && input < 256) {
        inputBuff[0] = (char) input;

        if (inputBuff[0] == '\n') {
          if (line.rfind("SEND ", 0) == 0) {
            Packet packet = Packet(SEND, removePrefix(line, "SEND ").c_str());
            connection->send(&packet);
            //mvprintw(0,10,"%s", line.c_str());
            //cout << "[sent]: " << line << endl;
          }
          mvprintw(0,0, std::string(line.size(), ' ').c_str());
          move(0,0);
          line.clear();
        } else {
          line.append(inputBuff);
          mvprintw(0,0, line.c_str());
        } 
      } else if (input == KEY_BACKSPACE && line.empty() == false) {
        line.pop_back();
        mvprintw(0,getcurx(stdscr)-1, " ");
        move(0,getcurx(stdscr)-1);
      }

      auto packetOrError = dataQueue.tryRemove();
      if (packetOrError.first == true) {
        mvprintw(6, 0,"[receivedOnMain] %s\n", packetOrError.second->serialize().c_str());
      }
      /*
      //getline(cin, line);

      if (line.rfind("SEND ", 0) == 0) {
        Packet packet = Packet(SEND, removePrefix(line, "SEND ").c_str());
        connection->send(&packet);

        cout << "[sent]: " << line << endl;
        
      } else if (line.rfind("FOLLOW", 0) == 0) {
        Packet packet = Packet(FOLLOW, removePrefix(line, "FOLLOW ").c_str());
        connection->send(&packet);

        cout << "[sent]: " << line << endl;
      } else if (line.rfind("QUIT", 0) == 0) {
        exit = true;
      }*/

      refresh();
    }

    endwin();
  } else {
    printf("Unable to login: %s\n", packet->payload());
  }

  delete packet;
  delete connection;

  return 0;
}
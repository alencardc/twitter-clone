# Notification System (~Twitter clone~)
Notification System (~Twitter clone~) is a distributed system to handle sending and receving of notifications. It's divided in three type of process:
* **Client `(./src/client/app_client.cpp)`:** the client process is used by users to follow other users, send and receive notifications.
* **Front-end `(./src/frontend/app_frontend.cpp)`:** this process goal is to allow the communications between Client apps and ReplicaManager servers.
* **ReplicaManager `(./src/server/app_server.cpp)`:** implements passive replication. It's responsible to process the requests, send back responses and keep all the data consistency among all backup replicas.


## Contributors
* **Alencar da Costa**
* **Filipe Faria Dias**
* **Raphael Scherpinski**


## Tech
This repo uses a number of known techs to work properly:
* C++ (std=C++11)
* Sockets API (POSIX)
* ncurses (v6.2)

## Compilation
1. In case you haven't installed nurses yet, you can install it by running this command `sudo apt-get install libncurses-dev`
2. Then, to compile all three apps run: `make` in the root directory of the project 
3. The executable can be found at `./build`

## Apps execution

#### Client
In the root directory of the project run `./build/app_client <username>`.
`<username>` is the identifier of the user you want to log in and must starts with `@` and have at least 4 characters.

Example: `./build/app_client @alencar`

**It's super important** to config the terminal that you are going execute the client process, in order to be able to visualize the client app correctly. The terminal must have *at least* **24 rows** and **100 columns**.

##### How to use the client app
The client app is very simple, it has three commands:
**Send requests/commands:** To send a request/command to the notification server, it's required to select the command input box and, to do so, you can press the key `c`. This will move the cursor to the command box and then you can start writing your commands. In order to send the typed command press `Enter` and to leave the command box press `F1`.

Currently, there are two commands available: `SEND` and `FOLLOW`.
To **follow** other user the command to use is: `FOLLLOW <username>` where `<username>` is the identifier of the user you want to follow. Remember: the identifier begins with `@` and has at least 4 characters. Example: `FOLLOW @student`.
To **send** a notification to all your followers execute `SEND <message>` where `<message>` is your notification message. Example: `SEND Hello, world!`.

**Read old notifications:** In order to read old notifications that were scrolled down in the Notification Box, you can press `n` and use the keys `Up arrow` and `Down arrow` to navigate in the Notification Box. To leave the notification box you need to press `F1`.

**Close the app:** To close the app you can press the key `F1` when you are outside any box.

#### Front-end
In the root directory of the project run `./build/app_frontend`. The front-end will init and try to start a server in one available addresses that were confiured in the file `/src/GlobalConfig.hpp`.

#### ReplicaManager
To init the first ReplicaManager that will automatically start as leader run `./build/app_server <port>`. It will start the internal communication TCP server at `0.0.0.0:<port>` and the notification server at `0.0.0.0:8080`. You can change the IP used by replicas and port used by the notification server in the file `src/server/config.hpp` by changing the value of the defined macros `DEFAULT_IP` and `DEFAULT_SERVER_PORT`.

And to init other backup replicas yu can run `./build/app_server <port> <current_leader_port>` where `<port>` is the port used by the new replica for the internal communication and `<current_leader_port>` is the port where the current leader replica is listening.
Examples:
`./build/app_server 3333 // start leader TCP server at port 3333`
`./build/app_server 4545 3333 // start leader TCP server at port 4545 and connects to the leader at port 3333`


## Persistence
In case you want to access the persistence file, you can open the file named `Users.db`.



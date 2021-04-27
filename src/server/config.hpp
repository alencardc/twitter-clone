#ifndef __config_hpp__
#define __config_hpp__

// Default IP of server and replicas
#define DEFAULT_IP "0.0.0.0"

// Internal Broadcast IP
#define BROADCAST_IP "255.255.255.255"

// Default port of main server to listen to client requests
#define DEFAULT_SERVER_PORT 8080

// Default port of main server to communicate with replicas
#define DEFAULT_INTERNAL_PORT 3333

// Duration in seconds to sleep the thread that check if server is alive
#define ALIVE_SLEEP 1

#endif // __config_hpp__
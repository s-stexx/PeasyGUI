// Minimal C version of the greeting example (without counter).  
// Uses tcp_connection.h to handle all cross-platform TCP client details.  
// Shows how easy it is to connect PeasyGUI from plain C code.

#include <stdio.h>
#include "tcp_connection.h"

int main()
{
    printf("Welcome!\n");

    int sock = connect_to_gui();

    while (1) {
        char *events = request("events?", sock);
        if (!events) {
            break; // error or disconnect
        }

        if (strstr(events, "greet_button`Clicked")) {
            char *user_name = request("get`user_input`Text`", sock);
            if (user_name) {
                char message[1024];
                snprintf(message, sizeof(message), "set`greet_text`Text`Hi, %s! 🤗`", user_name);
                request(message, sock);
                free(user_name);
            }
        }

        if (strstr(events, "Window`Closed`")) {
            request("set`Window`QuitNow`True`", sock);
            close(sock);
            free(events);
            break;
        }

        free(events);
        #ifdef _WIN32
            Sleep(100); // ms
        #else
            usleep(100000);
        #endif
    }


    return 0;
}

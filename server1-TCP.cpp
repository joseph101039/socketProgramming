#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 4096

enum {CMD_NAME, SRC_PORT};
int main(int argc, char *argv[])
{
  struct sockaddr_in server;
  struct sockaddr_in client;
  unsigned int len;
  int port;
  int s;              //收發訊息用
  int s0;           //收發連接用

printf("Please input port number\n");
scanf("%d", &port);


 if ((s0 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {          //開啟套接口
    perror("socket");
    exit(EXIT_FAILURE);
  }

  //指定伺服器IP與埠口碼
  memset(&server, 0, sizeof server);
  server.sin_family      = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port        = htons(port);

  //binding
  if (bind(s0, (struct sockaddr *) &server, sizeof server) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  //listening 開始連接
    if (listen(s0, 5) < 0) {        //backlog is 5 client queuing number)
    perror("listen");
    exit(EXIT_FAILURE);
  }

    int c = sizeof(struct sockaddr_in);

    printf("Server waiting\n");

    len = sizeof client;
    if ((s = accept(s0, (struct sockaddr *) &client, &len)) < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }

    char recv_buf[BUFSIZE];
    char send_buf[BUFSIZE];


    /*server先收後傳*/
    while(1){
         memset(recv_buf, 0, BUFSIZE);

          int i;
          for (i = 0; i < BUFSIZE - 1; i++)
          {
            if (recv(s, &recv_buf[i], 1, 0) <= 0){
                perror("receive error!\n");
                goto exit_loop;
                //break;
              }
            if (recv_buf[i] == '\n' )
              break;
            }
          printf("Server receives: %s", recv_buf);

          memset(send_buf, 0, BUFSIZE);

          int msg_len;
           msg_len = strlen(recv_buf) - 1;
          for(int i = msg_len - 1; i >= 0; i--){
            if(recv_buf[i] < 123 && recv_buf[i] > 96)
                send_buf[msg_len - 1 - i]  = recv_buf[i] - 32;
            else
                send_buf[msg_len - 1 - i] = recv_buf[i];
         }

            strcat(send_buf, "\n");

            printf("Servers send: %s", send_buf);
            if(send(s, send_buf, strlen(send_buf), 0) <= 0)
                perror("send");
    }
    exit_loop:
        close(s0);
        return 0;
}

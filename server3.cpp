#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<time.h>

#define BUFSIZE 15

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

    /*產生隨機變數*/
    int pos[10];    //1~10 數字排在第幾個位置
    for(int i = 0; i < 10; i++)
        pos[i] = 0;

    srand(time(NULL));
    int num[4];
    for(int j = 0; j < 4; j++){
           do{
                num[j] = rand() % 10;
           }while(pos[num[j]] != 0);
            pos[num[j]] = j + 1;
    }

    printf("Server generate: %d%d%d%d\n", num[0], num[1], num[2], num[3]);

    char recv_buf[BUFSIZE];
    int A;
    int B;
    while(1)
    {
        memset(recv_buf, 0, BUFSIZE);

         for (int i = 0; i < 4; i++)
          {
            if (recv(s, &recv_buf[i], 1, 0) <= 0){
                perror("receive error!\n");
                goto exit_loop;
              }
            }

            printf("Client guess: %s\n", recv_buf);
            int guess_tmp;
            guess_tmp = atoi(recv_buf);

            int dig[4];
            dig[3] = guess_tmp % 10;
            dig[2] = (guess_tmp %100) / 10;
            dig[1] = (guess_tmp % 1000) / 100;
            dig[0] = guess_tmp / 1000;


            A = 0;
            B = 0;
            for(int i = 0; i < 4; i++){
                if(pos[dig[i]] != 0){
                    if(pos[dig[i]] == i + 1)
                        A++;
                    else
                        B++;
                }
            }

            char result[4];
            sprintf(result, "%dA%dB", A, B);
            if(send(s, result, 4, 0) <= 0)
                perror("send");

            if(A== 4)
                break;
        }

    exit_loop:
        close(s0);
        return 0;
}

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

    char filename[30];
    char ext[30];       //存副檔名

    memset(filename, 0, sizeof(filename));
    memset(ext, 0, sizeof(ext));
    /*接收檔案名稱*/
    if(read(s, filename, sizeof(filename)) < 0)
            perror("read");

    int p;
    for(p = sizeof(filename); p >= 0; p--)
        if(filename[p] == '.')
            break;

int i, k = 0;
for(i = p; i <= sizeof(filename); i++){
    ext[k] = filename[i];
    k++;
}
ext[k + 1] = '\0';
filename[p] = '\0';

strcat(filename, "-b01705039");
strcat(filename, ext);

    //server read fiel and save it
    FILE* fp = fopen(filename, "wb");
    if(fp == NULL){
        perror("File open Error!\n");
        exit(-1);
    }

    char file_read[BUFSIZE];
    memset(file_read, '0', BUFSIZE);

    int byteRead;
    while(1){
        byteRead = read(s, file_read, sizeof(file_read));
        printf("Read %d bytes\n", byteRead);

        if(byteRead == 0)
           break;

        byteRead =  fwrite(file_read, sizeof(char), byteRead, fp);
        printf("Write %d bytes\n", byteRead);

    }


        fclose(fp);
        close(s0);
        return 0;
}

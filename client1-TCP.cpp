#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h> //for gethostbyname
#include <errno.h>	//for strerror()

#define DEFAULT_PORT 5900       /*設定port 為 5900*/
#define BUFSIZE 4096

int main(int argc, char *argv[])
{
    struct sockaddr_in server;  /* 伺服器位址*/
    unsigned long dst_ip;       /* 目標位址          */
    int port;                   /* 埠口碼    */
    int s;                      /* 套接口描述子      */
    int n;                      /* 輸入端byte數         */
    int len;                    /* 應用軟體資料長度 */
    char send_buf[BUFSIZE];     /*發送緩衝區    */

    printf("Please input IP address\n");
    char ip_address[16];
    scanf("%s", ip_address);

    printf("Please input port number\n");
    char port_num[6];
    scanf("%s", port_num);


/* 檢索伺服器IP位址 ,指定給dst_ip*/
  if ((dst_ip = inet_addr(ip_address)) == INADDR_NONE) {  //如果伺服端IP為255.255.255.255 ?
    struct hostent *he;  /* 主機訊息(IP) */

    if ((he = gethostbyname(ip_address)) == NULL) {           //檢索後為錯誤值終止
      fprintf(stderr, "gethostbyname error\n");
      exit(EXIT_FAILURE);
    }
    memcpy((char *) &dst_ip, (char *) he->h_addr, he->h_length);        //複製伺服端IP位置到dst_ip
  }


  /* 如果有輸入埠口碼, 指定給port*/
    if ((port = atoi(port_num)) == 0) {
      struct servent *se;
      /*服務訊息(port) */

      if ((se = getservbyname(port_num, "tcp")) != NULL)
        port = (int) ntohs((u_int16_t) se->s_port);             //(將主機big / little endian 調整成 network byte)
      else {
        fprintf(stderr, "getservbyname error\n");
        exit(EXIT_FAILURE);
      }
    }
   else
    port = DEFAULT_PORT;

  /* 使用TCP協定開啟套接口 */
  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {      //如果沒有成功 Allocate a socket
    perror("socket");                   //print error message
    exit(EXIT_FAILURE);
  }

  /* 設定伺服器位址確定連結 */
  memset(&server, 0, sizeof server);        //clean up
  server.sin_family      = AF_INET;                 //address family: Internet
  server.sin_addr.s_addr = dst_ip;
  server.sin_port        = htons(port);

  if (connect(s, (struct sockaddr *) &server, sizeof server) < 0) {     //如果連接伺服器不成功
    perror("connect");
    exit(EXIT_FAILURE);
  }
  printf("connected to '%s'\n", inet_ntoa(server.sin_addr));


/*prepare to send message*/
char send_msg[BUFSIZE] ;
 char recv_msg[BUFSIZE];
    while(1){
        memset(send_msg, 0, BUFSIZE);
        memset(recv_msg, 0, BUFSIZE);

        /*receiver 先傳後收*/

        printf("Send message to server:\n");

        setbuf(stdin, send_msg);            //flush out the previous input in buffer
        //fflush(stdin);                //fflush is no use in Linux
        //getchar();                       //getchar may be useful

        if(fgets(send_msg, BUFSIZE - 2, stdin) == NULL)
           perror("Input message");

        if(send(s, send_msg, strlen(send_msg), 0) <= 0){
            perror("send");
        }


        int i;      //接收的字數
        for(i = 0; i < BUFSIZE; i++)
        {
            recv(s, &recv_msg[i], 1, 0);
            if(recv_msg[i] == '\n' ){
               i--;
               break;
            }
        }
        printf("Client receives: %s", recv_msg);
    }
     close(s);

  return 0;
}

#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define DEBUG
#define BUFFERSIZE 0xffff

void setSocketTCP(int *sockfd, int keepalive, char *port);
void parseHTTP(char *inputData, char *outputData, ssize_t size);
void executeCGI(char *filename, char *outputData);
int main(int argc, char *argv[])
{
      int sockfd = 0, clientSockfd = 0;
      struct sockaddr_in clientInfo;
      int addrlen = sizeof(clientInfo);
      char *port;
      int keepalive;

      port = argc > 2 ? argv[2] : "8080";
      keepalive = (argc > 1 && !strncmp(argv[1], "-k", 2)) ? 1 : 0;
      setSocketTCP(&sockfd, keepalive, port);

      while (1)
      {
            char serverInput[BUFFERSIZE + 1] = {};
            char serverOutput[BUFFERSIZE + 1] = {};
            char ipstr[INET_ADDRSTRLEN];

            clientSockfd = accept(sockfd, (struct sockaddr *)&clientInfo, &addrlen);
            if (clientSockfd == -1)
            {
                  printf("clientSockfd error: %d\n", clientSockfd);
                  exit(EXIT_FAILURE);
            }
            inet_ntop(AF_INET, &clientInfo.sin_addr.s_addr, ipstr, INET_ADDRSTRLEN);
            printf("Accept socket from %s: %d\n", ipstr, ntohs(clientInfo.sin_port));

            clearenv();
            parseHTTP(serverInput, serverOutput, recv(clientSockfd, serverInput, sizeof(serverInput), 0));

            printf("----------Response---------\n");
            printf("%s\n", serverOutput);
            printf("---------------------------\n\n\n");

            send(clientSockfd, serverOutput, sizeof(serverOutput), 0);
      }

      exit(0);
}

void setSocketTCP(int *sockfd, int keepalive, char *port)
{
      struct sockaddr_in serverInfo;
      int optval;
      socklen_t optlen = sizeof(optval);

      *sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (*sockfd == -1)
      {
            printf("Fail to create a socket.\n");
            exit(EXIT_FAILURE);
      }

      if (keepalive)
      {
            if (setsockopt(*sockfd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)) == -1)
            {
                  perror("setsockopt()\n");
                  exit(EXIT_FAILURE);
            }
      }

      if (getsockopt(*sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) == -1)
      {
            perror("getsockopt()\n");
            exit(EXIT_FAILURE);
      }
      printf("SO_KEEPALIVE default is %s\n", (optval ? "ON" : "OFF"));

      bzero(&serverInfo, sizeof(serverInfo));
      serverInfo.sin_family = PF_INET;
      serverInfo.sin_addr.s_addr = INADDR_ANY;
      serverInfo.sin_port = htons(atoi(port));

      bind(*sockfd, (struct sockaddr *)&serverInfo, sizeof(serverInfo));
      listen(*sockfd, 0xf);
}

void parseHTTP(char *inputData, char *outputData, ssize_t size)
{
      printf("----------Request----------\n");
      printf("%s\n", inputData);
      printf("---------------------------\n\n");

      char *env[3] = {"REQUEST_METHOD", "REQUEST_URI", "SERVER_PROTOCOL"};
      char copy[BUFFERSIZE + 1] = {};
      char *tokens[3];

      if (size == -1)
      {
            printf("recv error\n");
            exit(EXIT_FAILURE);
      }

      if (size == 0)
            return;

      if (size > BUFFERSIZE)
            inputData[0] = '\0';
      else
            inputData[size] = '\0';

      printf("recv %ld bytes\n", size);

      strcpy(copy, inputData);
      strtok(copy, "\r\n");

      tokens[0] = strtok(copy, " ");
      for (int i = 1; i < 3; i++)
      {
            if (tokens[i - 1] != NULL)
                  tokens[i] = strtok(NULL, " ");
      }

      for (int i = 0; i < 3; i++)
      {
            setenv(env[i], tokens[i], 1);
      }

      if (!strcmp(tokens[1], "/"))
      {
            strncat(outputData, "HTTP/1.1 200 OK\r\n", BUFFERSIZE);
            strncat(outputData, "Content-Type: text/html\r\n", BUFFERSIZE);
            strncat(outputData, "Content-Length:146\r\n\r\n", BUFFERSIZE);
            strncat(outputData, "<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n<TITLE>I\'m a example</TITLE>\n<BODY>B10632021 NS HW1</BODY></HTML>\n", BUFFERSIZE);
      }
      else if (!strncmp(tokens[1], "/env.cgi", 8) && (tokens[1][8] == '\0' || tokens[1][8] == '?'))
      {
            strncat(outputData, "HTTP/1.1 200 OK\r\n", BUFFERSIZE);

            if (strchr(tokens[1], '?') != NULL)
                  setenv("QUERY_STRING", strchr(tokens[1], '?') + 1, 1);

            executeCGI("./env.cgi", outputData);
            // while (read(cgiOutput[0], &c, 1) > 0)
            //       response[i++] = c;
            // response[i] = '\0';
      }
      else if (!strncmp(tokens[1], "/mul.cgi", 8) && (tokens[1][8] == '\0' || tokens[1][8] == '?'))
      {
            strncat(outputData, "HTTP/1.1 200 OK\r\n", BUFFERSIZE);

            if (strchr(tokens[1], '?') != NULL)
                  setenv("QUERY_STRING", strchr(tokens[1], '?') + 1, 1);

            executeCGI("./mul.cgi", outputData);
            // while (read(cgiOutput[0], &c, 1) > 0)
            //       response[i++] = c;
            // response[i] = '\0';
      }
      else
      {
            int htmlfd = open("./404.html", O_RDONLY);
            char outputBuffer[BUFFERSIZE + 1] = {};
            char buf[0xf] = {};
            char c;
            if (htmlfd != -1)
            {
                  while (read(htmlfd, &c, 1) > 0)
                        strncat(outputBuffer, &c, 1);
                  sprintf(buf, "%lu", strlen(outputBuffer));
            }
            strncat(outputData, "HTTP/1.1 404 Not Found\r\n", BUFFERSIZE);
            strncat(outputData, "Content-type:text/html\r\n", BUFFERSIZE);
            strncat(outputData, "Content-Length:", BUFFERSIZE);
            strncat(outputData, buf, BUFFERSIZE);
            strncat(outputData, "\r\n\r\n", BUFFERSIZE);
            strncat(outputData, outputBuffer, BUFFERSIZE);

            close(htmlfd);
      }
}

void executeCGI(char *filename, char *outputData)
{
      int input[2];
      int output[2];

      pid_t cpid;
      int status;
      char c;
      int i = strlen("HTTP/1.1 200 OK\r\n");

      if (pipe(input) < 0 || pipe(output) < 0)
      {
            perror("pipe");
            exit(EXIT_FAILURE);
      }

      /* Creates a new process to execute cgi program */
      if ((cpid = fork()) < 0)
      {
            perror("fork");
            exit(EXIT_FAILURE);
      }

      /*child process*/
      if (cpid == 0)
      {
            printf("This is child process\n");
            printf("Execute CGI...\n");

            //close unused fd
            close(input[1]);
            close(output[0]);

            //redirect the output from stdout to cgiOutput
            dup2(output[1], STDOUT_FILENO);

            //redirect the input from stdin to cgiInput
            dup2(input[0], STDIN_FILENO);

            //after redirect we don't need the old fd
            close(input[0]);
            close(output[1]);
            //the stdout of CGI program is redirect to cgiOutput
            //the stdin  of CGI program is redirect to cgiInput
            execlp(filename, filename, NULL);
            exit(0);
      }
      else
      {
            printf("This is parent process\n");
            //close unused fd
            close(output[1]);
            close(input[0]);

            // // send the message to the CGI program
            // write(input[1], inputData, strlen(inputData));

            // receive the message from the  CGI program

            while (read(output[0], &c, 1) > 0)
            {
                  // write(STDOUT_FILENO, &c, 1);
                  outputData[i++] = c;
            }
            // send(STDOUT_FILENO, "\n", 1, 0);
            outputData[i] = '\0';

            // connection finish
            close(output[0]);
            close(input[1]);
            waitpid(cpid, &status, 0);
      }
}

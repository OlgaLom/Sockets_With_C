#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <time.h>
#include <stdlib.h>

void ConnectToServer(int arr[],int sizeOfArray,int Port );

int main(int argc , char *argv[])
{
    int sock,portToServerA=8881,portToServerB=8880;
    struct sockaddr_in server;
    int message,converted_number,RandNumForVector,response;
    char server_reply[2000];

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8883 ); 

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\nPlsease wait..\n");

    //Random number that will be the size of the vector 
    srand(time(0));
    RandNumForVector = (rand() %10 )+2;
    sleep(1);
    //check if the number is zero if it is add one {we dont want an array with size zero}
    if(RandNumForVector == 0)
        RandNumForVector ++;
    //initialize a array with this size 
    int arr[RandNumForVector];

    printf("Vector size : %d\n",RandNumForVector );

    response =htonl(RandNumForVector);
    //send the size of the vector 
    if( write(sock, &response, sizeof(response)) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Receive a reply from the server

    if( recv(sock , server_reply , 2000 , 0) < 0 )
    {
        puts("recv failed");
        return 1;
    }
    printf("Server reply :");
    puts(server_reply);
    printf("Creating the Vector with random numbers. Plsease wait.. \n");
    //fill the array with random numbers 
    for (int i = 0; i < RandNumForVector; i++) 
    {
        arr[i] = rand() %100;
        sleep(1);
    }

    // communicating with server
    for (int i = 0; i < RandNumForVector; i++) 
    {
        printf("Vector[%d] : %d\n",i,arr[i] );
        //Send some data
        converted_number =htonl(arr[i]);
        if( write(sock, &converted_number, sizeof(converted_number)) < 0)
        {
            puts("Send failed");
            return 1;
        }
    }//for END
    
    //Receive a reply from the server
    if( recv(sock , server_reply , 2000 , 0) < 0 )
    {
        puts("recv failed");
        return 1;
    }
    printf("Server reply :");
    puts(server_reply);

    int LastChar;
    //Check the last character from the reply and if it is A then connect with server A  else with B
    LastChar=  strlen(server_reply)-1;
      
    if(server_reply[LastChar] =='A')
    {

        printf("Connecting with server A ..\n");
        //close the sock from main server
        close(sock);
        //call the ConnectToServer function.
        ConnectToServer(arr,RandNumForVector,portToServerA);
        //arr : is the Vector(array) that we created 
        //RandNumForVector : the size of the Vector.
        //portToServerA : Port of server A 
    }
    else
    {
        printf("Connecting with server B ..\n");
        //close the sock from main server
        close(sock);
        //call the ConnectToServer function.
        ConnectToServer(arr,RandNumForVector,portToServerB);
    }

    return 0;
}

void ConnectToServer(int arr[],int sizeOfArray,int Port)
{
    struct sockaddr_in server;
    int converted_number,sock,response;
    char server_reply[2000];
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( Port ); 

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
       // return 1;
    }

    puts("Connected\nPlsease wait.. ");
    //send the vector size 
    response =htonl(sizeOfArray);
    write(sock, &response, sizeof(response));
    
    //Send the Vector 
    for (int i = 0; i < sizeOfArray; i++) 
    {
       // printf("Vector[%d] : %d\n",i,arr[i] );
        //Send some data
        converted_number =htonl(arr[i]);
        if( write(sock, &converted_number, sizeof(converted_number)) < 0)
        {
            puts("Send failed");
            //return 1;
        }
    }
         //Receive a reply from the server
        if( recv(sock , server_reply , 2000 , 0) < 0 )
        {
            puts("recv failed");
           // return 1;
        }
        printf("Server reply :");
        puts(server_reply);

        
    close(sock);
}

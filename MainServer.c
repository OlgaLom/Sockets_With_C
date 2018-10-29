#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

float fluAvg(char FileName[]);
float DefineTheRightServer(char FileName[], int clientArray[],int SizeOfTheVector);


int main(int argc , char *argv[])
{
	double result;
	int socket_desc , client_sock , c , read_size, SizeOfTheVector;
 	struct sockaddr_in server , client;
 	int VectorSize,client_message, client_array[100],count=0 ;
	char response[10] ="Accepted";
 
	//Create socket
  	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  	if (socket_desc == -1)
  	{
   		printf("Could not create socket");
  	}
  	puts("Socket created");

 	//Prepare the sockaddr_in structure
  	server.sin_family = AF_INET;
  	server.sin_addr.s_addr = INADDR_ANY;
  	server.sin_port = htons( 8883 );

 	//Bind
  	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
  	{
    	//print the error message
    	perror("bind failed. Error");
    	return 1;
  	}
    puts("bind done");

    //Listen
  	listen(socket_desc , 100);

  	//Accept and incoming connection
  	puts("Waiting for incoming connections...");
  	c = sizeof(struct sockaddr_in);

  	while(1)
  	{

    //accept connection from an incoming client
  	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
  	if (client_sock < 0)
 	{
 	   perror("accept failed");
 	   return 1;
	}
	puts("Connection accepted.");

	//read the message that contains the size of the vector .
	read_size = read(client_sock, &VectorSize, sizeof(VectorSize));
	//convert the message 
	SizeOfTheVector = ntohl(VectorSize);
	printf("SizeOfTheVector :%d\n", SizeOfTheVector);
	write(client_sock,response,sizeof(response));
	//sleep(5);
	 //do the loop and read the integers of the Vector
	for (int i = 0; i < SizeOfTheVector; ++i)
	{
		//read
		read_size = read(client_sock, &client_message, sizeof(client_message));
		//print the integer that the MainServer recieve
		//fprintf(stdout, "Received int = %d\n", ntohl(client_message));
		
		//insert that integer in the array
		client_array[i] = ntohl(client_message);	
	}//For END

	//initiate the possible strings 
	char messageB[100]= "Please pocced with the connection with server B ";
	char messageA[100]= "Please pocced with the connection with server A";
	 
	//call the DefineTheRightServer function.
	//if the function return true that means that the client must connect with server A else with server B.
	float resultFromA= DefineTheRightServer("AvgFromServerA",client_array,SizeOfTheVector);
	float resultsFromB = DefineTheRightServer( "AvgFromServerB",client_array,SizeOfTheVector);
	

	float fluFromA = fluAvg("FluFromServerA");
	float fluFromB = fluAvg("FluFromServerB");

	// printf("A flu %f\n", fluFromA);
	// printf("B flu %f\n", fluFromB);

	if (resultFromA < resultsFromB )
		write(client_sock , messageA , sizeof(messageA));
	else
		write(client_sock , messageB , sizeof(messageB));
	
 	if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    printf("Waiting for a client to connect..\n");
    }//While END

    return 0;
}//Main END

float fluAvg(char FileName[])
{
	int i,counter;
   	float sum = 0,pointer,avg;
   	FILE *File;
	// Openning the file with file handler as fileA
    File = fopen(FileName, "r");
   	//loop through all the array and calculate the sum.
	while (fscanf(File, "%f", &pointer)!= EOF)
	{
		sum += pointer;
		counter++;
	}
	fclose(File);
	avg = sum/counter;
	return avg;
    
}//ClientAVG END

float DefineTheRightServer(char FileName[],int clientArray[],int SizeOfTheVector)
{
	float pointer,avgA,temp=0 ,results=0;
	 
 	FILE *fileA;
	// Openning the file with file handler as fileA
    fileA = fopen(FileName, "r");
    //read the value stored in the file (the average from Server A)
	
    int counter=0,length=SizeOfTheVector;
    //loop through the file and count the quantity of the numbers
	while (fscanf(fileA, "%f", &pointer)!= EOF)
	{
		if (pointer)
		counter++;
	}
	fclose(fileA);
	//create a array with that number that will be the servers array.
	float A[counter];
	//printf("A size : %d\n",counter );
	counter=0;
	
	fileA = fopen(FileName, "r");
	//insert the every avg in the array.
	while (fscanf(fileA, "%f", &pointer)!= EOF)
	{
		A[counter]= pointer;
		//printf("Counter : %f\n",A[counter] );
		counter++;
	}
	fclose(fileA);
	//check which array is the biggest.
	if( counter > SizeOfTheVector)
		length = counter;

	//calculation for the euclidean distance
	//loop through the biggest array
	for(int x=0; x < length; x++)
	{	

		//if the client array is smaller than the serverA, that means that must proceed with 0-A[x] 

		if (x >= SizeOfTheVector)
		{
			temp += pow(fabs(0-A[x]),2.0);
			//printf("%d :x>SizeOfTheVector: math is %f\n",x,  pow(fabs(0-A[x]),2.0) );
		}
		//if the serverA array is smaller than the clients, that means that must proceed with clientArray[x]-0
		else if(x >= counter)
		{
			temp += pow(fabs(clientArray[x]-0),2.0);
			//printf("%d : x>count :math is %f\n",x,  pow(fabs(clientArray[x]),2.0) );
		}
		else
		{
			temp += pow(fabs(clientArray[x]-A[x]),2.0);
			//printf("%d :else math is %f\n",x, pow(fabs(clientArray[x]-A[x]),2.0) );
		}
	}

 	results = sqrt(temp);

	//fabs : convert a number to a positive one
	//pow : calculate the fist argument raised to the power of the second argument. 
	//sqrt : returns the square root 
	//So the resultA have the value of the Euclidean distance between the clientsAVG and the avg from the server A

	printf("Euclidean distance between client average and the average of server is : %f\n",results );
	

	return results;
}//DefineTheRightServer END
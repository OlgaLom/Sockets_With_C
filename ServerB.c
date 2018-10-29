#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <stdlib.h>
#include <math.h>

void Calculations(char *filename);

int main(int argc , char *argv[])
{
  int socket_desc , client_sock , c , read_size;
  struct sockaddr_in server , client;
  int client_message,VectorSize,sum = 0,SizeOfTheVector,client_array[30];
  char NameOfTheFile[8]="FileB";
  char response[10] ="Accepted";
  FILE * MyFile;
    
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
  server.sin_port = htons( 8880 );

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

  while(1)
  {
    //Accept and incoming connection
  
  puts("Waiting for incoming connections...");
  c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
  client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
  if (client_sock < 0)
  {
    perror("accept failed");
    return 1;
  }
  puts("Connection accepted");
	
  //read the size of the vector 
  read_size = read(client_sock, &VectorSize, sizeof(VectorSize));
  SizeOfTheVector = ntohl(VectorSize);
  //ntohl: function converts the unsigned integer netlong 
  //from network byte order to host byte order. 
  printf("SizeOfTheVector :%d\n", SizeOfTheVector);
  write(client_sock,response,sizeof(response));

  //do the loop and read the vector
  for (int i = 0; i < SizeOfTheVector; ++i)
  {
    read_size = read(client_sock, &client_message, sizeof(client_message)) ;
    //fprintf(stdout, "Received int = %d\n", ntohl(client_message));
    //save the number in the array 
    client_array[i] = ntohl(client_message);
    //open the file and store the number.
    //check if the file exists
    if( access( NameOfTheFile, F_OK ) != -1 ) 
    {
      // file exists
      MyFile = fopen(NameOfTheFile,"a");
      //check if the current integer that we are going to insert is the last one
      //if it is then add a break line (\n), else just insert the number.
      if(i == SizeOfTheVector-1)
      {
        fprintf(MyFile,"%d\n", client_array[i] );
      }else{
        fprintf(MyFile,"%d ", client_array[i] );
      }//if END

      fclose(MyFile); 

    } else {
       // file doesn't exist
      MyFile = fopen(NameOfTheFile,"w");
      fprintf(MyFile,"%d", client_array[i] );
      fclose(MyFile);
    }//If END   
  }//For END

  //call the Calculations function with one parametre the name of the file that we want to open .
  Calculations(NameOfTheFile);

  if(read_size == 0)
  {
    puts("Client disconnected");
    fflush(stdout);
  }else if(read_size == -1){
        perror("recv failed");
  }//If END
 
    }//while END
    return 0;
}//Main END

void Calculations(char *filename)
{
    char ws = ' ';
  int i=0,result,sum = 0,num = 0,counter,count,Average,AVG[100];
  float n = 0,avg=0,flu=0,temp=0;
  FILE *fi, *AvgFile, * FluFile;
  fi = fopen(filename, "r");
   
  // Keep reading in integers (which are placed into "n") until end of file (EOF)
while ( result = fscanf ( fi, "%d%c", &num, &ws)) 
{
        if ( EOF == result) {
            break;
        }
        if ( 2 == result && ' ' == ws) 
        {//scanned an int an a space
            counter=1;
            sum = num;
            while ( 2 == fscanf ( fi, "%d%c", &num, &ws)) 
            {
                counter++;
                sum += num;
                if ( '\n' == ws) {//quit when a newline is scanned
                    count++;
                    break;
                }
            }   
            //printf("Counter : %d\n",counter );    
        }
        Average = sum/counter; 
        AVG[i]=Average; 
       // printf("AVG[%d] : %d\n",i,Average );
       //  printf ( "Sum is %d\n", sum);
       // printf ( "Average is %d and i is %d\n", Average,i);

        //if i<1 means that we calculate the avg for the first row 
        //and we want to update all the file so we are overide it with "w"
        //and after that for the rest of the avgs we use the 'a' so that we dont override the 
        //new avgs that we instert.
        if(i<1)
        {
          AvgFile = fopen("AvgFromServerB","w");
          fprintf(AvgFile,"%d\n",AVG[i]);
          fclose(AvgFile);
        }
        else
        {
          AvgFile = fopen("AvgFromServerB","a");
          fprintf(AvgFile,"%d\n",AVG[i]);
          fclose(AvgFile);

        }
    
    i++;
}//while END
      
  fclose(fi); i=0;

  fi = fopen(filename, "r");
while ( result = fscanf ( fi, "%d%c", &num, &ws)) 
{
        if ( EOF == result) {
            break;
        }
        if ( 2 == result && ' ' == ws) 
        {//scanned an int an a space
            counter=1;
            temp = num-AVG[i];
            flu = pow(fabs(temp),2.0);
            while ( 2 == fscanf ( fi, "%d%c", &num, &ws)) 
            {
              // number - average  
              temp = num-AVG[i];  
              // fabs : convert a number to a positive one
              // pow: the result of the above command 
              flu += pow(fabs(temp),2.0);
              counter++;

                if ( '\n' == ws) {//quit when a newline is scanned
                  break;
                }
            }   
             
        }
        flu= flu/counter;
        // printf("The fluctuation is: %.2f Row %d\n", flu,i);

        //if i<1 means that we calculate the avg for the first row 
        //and we want to update all the file so we are overide it with "w"
        //and after that for the rest of the avgs we use the 'a' so that we dont override the 
        //new avgs that we instert.
        if(i<1)
        {
           FluFile = fopen("FluFromServerB","w");
           fprintf(FluFile,"%.2f\n",flu);
           fclose(FluFile);
        }
        else
        {
           FluFile = fopen("FluFromServerB","a");
          fprintf(FluFile,"%.2f\n",flu);
           fclose(FluFile);
        }
    
    i++;
}//while END

}//Calculations END

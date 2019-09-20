// hwclient.c
// Hello World client
// Connects REQ socket to tcp://localhost:5555 
// Sends "Hello" to server, expects "World" back 
//
#include <zhelpers.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int main () {
  void* context = zmq_ctx_new();

  // Socket to talk to server
  puts("Connecting to hello world server...");
  void* requester = zmq_socket(context, ZMQ_REQ);
  zmq_connect(requester, "tcp://localhost:5555");

  for (int request_nbr = 0; request_nbr != 10; request_nbr++) {
    char* request = "Hello";
    s_send(requester, request);
	s_send(requester, "Test");
 
    char* reply;
    reply = s_recv(requester);
    printf("Received %s %d\n", reply, request_nbr);
    free (reply);
	getchar();
  }
	FILE *file = NULL;
	unsigned char buffer[254];  // array of bytes, not pointers-to-bytes
	size_t bytesRead = 0;
	int size = 0;

	file = fopen("test.png", "rb");   

	if (file != NULL)    
	{
		fseek(file, 0, SEEK_END); // seek to end of file
		size = ftell(file); // get current file pointer
		fseek(file, 0, SEEK_SET); // seek back to beginning of file
		printf("size: %d\n",size);
		char strsize[20];
		sprintf(strsize, "%d", size);									
		s_sendmore(requester, strsize);
	  // read up to sizeof(buffer) bytes
	  while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0)
	  {
		// process bytesRead worth of data in buffer
		if(bytesRead == 254){
			printf("Sending Bytes %d %s\n", bytesRead, buffer);			
			char str[20];
			sprintf(str, "%d", bytesRead);									
			s_sendmore(requester, str);
			s_sendmore(requester, buffer);		
			//printf("Received\n");		
		}
		else{
			//printf("Sending Bytes %d %s\n", bytesRead, buffer);			
			char str[20];
			sprintf(str, "%d", bytesRead);									
			unsigned char bufferaux[bytesRead];
			strncpy(bufferaux, buffer, bytesRead);
			printf("Sending Bytes %d %s\n", bytesRead, bufferaux);			
			s_sendmore(requester, str);
			s_sendmore(requester, bufferaux);		
			//printf("Received\n");	
		}
	  }
	  //printf("Buffer: %s\n", buffer);
	  s_sendmore(requester, "0");
	  s_send(requester,"Done");
	}
	getchar();	
  zmq_close(requester);
  zmq_ctx_destroy(context);
}

// proserver.c
// Hello World server
// Binds REP socket to tcp://*:5555
// Expects "Hello" from client, replies with "World" 
//
#include <zhelpers.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h> 

int main (void) {
  void *context = zmq_ctx_new ();
  //  Socket to talk to clients
  void *responder = zmq_socket (context, ZMQ_REP); 
  zmq_bind (responder, "tcp://*:5555");
  int contador = 0;
  int flagSize = 0;
  int more;  
  size_t more_size = sizeof(more);  
  char * line;  
  int ending = 0;
  unsigned int length = 0; 
  unsigned int totalSize = 0;
  //unsigned char buffer[1024];
  while (1) {
    // Wait for next request from client
	if(contador!=10){
		char* request;
		request = s_recv(responder);
		if (request==NULL) continue;
		printf ("Received %s\n",request);
		free(request);
		
		//  Do some 'work'
		//s_sleep (1);
		
		//  Send reply back to client
		char* reply = "World";
		s_send (responder, reply);
		contador = contador +1;
	}
	else{		
		char * buffer = "";	
		char * data = "";	
		/*FILE *out;	
		out = fopen("copy.png", "wb");*/
		line = s_recv(responder);
		if(line==NULL) continue;
		printf("Total is: %s", line);
		sscanf(line,"%d",&totalSize);
		buffer = malloc(totalSize);
		free(line);
		while (1) {  			
			printf("Starting\n");
		   line = s_recv(responder);
		   printf("After\n");
		   if(line==NULL) continue;		                
		   sscanf(line,"%d",&length);
		   printf("Bytes Size %s\n",line);		   		   
		   free(line);		   		   	  		   
		   if(length > 0){
			   printf("Entrando IF validando length %d\n", length);
			   data = malloc(length);			   
			   line = s_recv(responder);				   
			   strcpy(data, line);	
			   printf("Data %s\n", data);
				//fwrite(data, 1, sizeof(data), out);
			   strcat(buffer, data);			   			  
			   free(line);  		   		   			   
		   } 
		   else{			   
			   printf("Entrando ELSE\n");			   
			   line = s_recv(responder);		   			   
			   printf("Receiving: %s\n", line);					   
			   free(line);
		   } 		   
		   zmq_getsockopt(responder, ZMQ_RCVMORE, &more, &more_size);  
		   
		   if (!more) { 
		        //fflush(out);
				//fclose(out); 		   
				printf("Creating File\n");
				FILE *fptr;
			   // use appropriate location if you are using MacOS or Linux
			   fptr = fopen("copyfile2.png","wb");
			   if(fptr == NULL)
			   {
				  printf("Error!");   
				  //exit(1);             
			   }		
				//memmove(buffer, buffer+3, strlen(buffer));
			   //fprintf(fptr,"%s",buffer);
			   fwrite(buffer, 1, totalSize, fptr);
			   fclose(fptr);
			 // Send a zero to close connection to client  			 
			 break;  
		   }		   		   		  
		}		
		free(buffer);	
		free(data);	
	}
  }
  //  We never get here but if we did, this would be how we end
  zmq_close (responder); 
  zmq_ctx_destroy (context); 
  return 0;
}

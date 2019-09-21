// hwserver.c
// Hello World server
// Binds REP socket to tcp://*:5555
// Expects "Hello" from client, replies with "World" 
//
#include <zhelpers.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

int fsize(FILE *fp){
    int prev=ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int sz=ftell(fp);
    fseek(fp,prev,SEEK_SET); //go back to where we were
    return sz;
}

static void list_dir(const char *path)
{
    struct dirent *entry;
    DIR *dir = opendir(path);

    if (dir == NULL) {
        return;
    }

    FILE *fp;
    fp = fopen ( "archivos.txt", "w" );

    while ((entry = readdir(dir)) != NULL) {

        char filename[256];
        struct tm *foo;
        struct stat attrib;

	    sprintf (filename, "%s", entry->d_name);

        if((!strcmp(filename,".")) || (!strcmp(filename,".."))){
            continue;
        }else
        {
            /* code */
            printf("File name: %s\n",filename);

            char directory[256] = "C:\\Users\\bryan\\Documents\\Sistemas Distribuidos\\ZMQ-Applications\\project\\";
            struct stat attr;
            stat(strcat(directory,filename), &attr);
            printf("Last modified time: %s\n", ctime(&attr.st_mtime));
            //para comparar tiempos: https://stackoverflow.com/questions/31633943/compare-two-times-in-c
            printf("Valor del struct: %d\n",attr.st_mtime);

            char sDateModified[15];

            // convert 123 to string [buf]
            itoa(attr.st_mtime, sDateModified, 10);

            printf("Comparando fechas:\n");
            //https://www.ibm.com/support/knowledgecenter/en/ssw_ibm_i_74/rtref/strpti.htm

/*
            char buf[21] = "MonSep0913:11:482019"; 
            struct tm *result;
    if (strptime(buf, "%a %b %d %H:%M:%S %Y",&result) == NULL)
          printf("\nstrptime failed\n");
    else
    {
          printf("tm_hour:  %d\n",result.tm_hour);
          printf("tm_min:  %d\n",result.tm_min);
          printf("tm_sec:  %d\n",result.tm_sec);
          printf("tm_mon:  %d\n",result.tm_mon);
          printf("tm_mday:  %d\n",result.tm_mday);
          printf("tm_year:  %d\n",result.tm_year);
          printf("tm_yday:  %d\n",result.tm_yday);
          printf("tm_wday:  %d\n",result.tm_wday);
    }
*/

            if(!S_ISDIR(attr.st_mode)){
                printf("Es un archivo\n");
                printf("Preparando para escribir en archivo\n");
	            
                fp = fopen ( "archivos.txt", "a" );

                char* fileDir = directory;
                printf("Leyendo desde: %s\n",fileDir);

                FILE *fpFileSize;
                fpFileSize = fopen(fileDir,"r");

                char fileSize[50];

            if(fpFileSize == NULL){
                printf("Error al leer archivo\n");
            }else{

                printf("Preparando para sacar el tamano del archivo\n");
                int size = fsize(fpFileSize);
                printf("El tamano es: %d\n",size); 
                itoa(size,fileSize,10);
            }


	            if (fp==NULL) {
                    printf("Error al crear o leer el archivo\n");
                }else{
                    printf("Escribiendo...\n\n");
                    fputs(strcat(filename, "\t"),fp);
                    fputs(strcat(fileSize, "\t"),fp);
                    fputs(strcat(sDateModified,"\n"),fp);
                }
            }

            /*
            struct stat t_stat;
            stat(filename, &t_stat);
            struct tm * timeinfo = localtime(&t_stat.st_ctime); // or gmtime() depending on what you want
            printf("File time and date: %s\n", asctime(timeinfo));*/
        }
        fclose ( fp );
    }

    closedir(dir);
}

int main(int argc, char *argv[]){
	int flag = 0;
	if(argv[1]==NULL){
		printf("Soy Server");
	}
	else{
		printf("Soy Cliente");
		flag = 1;
	}	
    printf("Hello %s!!\n",argv[1]);
    printf("Hello %s!!\n",argv[2]);
	
    list_dir("C:\\Users\\bryan\\Documents\\Sistemas Distribuidos\\ZMQ-Applications\\project");
	
	if(flag){//This is for Client Stuff
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
	else{//This is for Server Stuff
		
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
				break;
			}
		  }
		  //  We never get here but if we did, this would be how we end
		  zmq_close (responder); 
		  zmq_ctx_destroy (context); 
	}
	
	printf("Saliendo..!!");
	getchar();
	

    return 0;
}
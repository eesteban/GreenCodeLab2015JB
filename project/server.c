#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

#define BUF_SIZE 1024 

int replace_esc(char* in_file, char* out_file);

int main()
{
	const char* fileName = "/opt/gclc/gclc.log";
	const char* tmpFileName = "/opt/gclc/tmp.log";
	const char* host = "79.98.21.68";
	const char* port = "7777";
	struct addrinfo hints, *res;
	struct sockaddr_in addr;
	socklen_t addr_len;
	FILE *f;
	int sock, new_sock, ret, diff, bool_first = 1;
	char buf[BUF_SIZE];
	time_t begin, current;
  	struct tm * timeinfo;

	// Create a socket
   	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));
	addr.sin_addr.s_addr = INADDR_ANY;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;  
	hints.ai_socktype = SOCK_STREAM;


	if((getaddrinfo(NULL, port, &hints, &res)) != 0){	
		perror("Server: Getaddrinfo error\n");
		return 1;
	}

	if ((sock = socket(AF_INET, SOCK_STREAM, 0 )) < 0){
		perror("Server: Error when creating the socket");
		return 1;
	}

	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
		perror("Server: Error while binding to socket");
		close(sock);
		return 1;
	}

	if (listen(sock, 5) == -1) {
    	perror("Server: Error while listening");
		close(sock);
    	return -1;
  	}
	
	addr_len = sizeof(addr);

	if ((new_sock = accept(sock, (struct sockaddr *) &addr, &addr_len) ) < 0 )
	{
			perror("Server: Error on accept");
			close(sock);
			return 1;
	}

	while(1)
	{
		// Open file
		f = fopen(tmpFileName, "w");
		if (f == NULL)
		{
			perror("Server: Error opening the file");
			close(new_sock);
			close(sock);
			return 1;
		}
		// If first recv
		if (bool_first)
		{
			// Read the data
			if ((ret = recv(new_sock, buf, BUF_SIZE-1, 0)) == -1)
			{
				perror("Server: Error recv");
				close(sock);
				close(new_sock);
				return 1;
			}
			else if(ret != 0)
			{
				buf[ret] = '\0';

				printf("%s", buf);
				printf("***TEAM40***\n");
				fprintf(f, "%s", buf);
			}

			// Set the timeout
			struct timeval timeout;      
		    timeout.tv_sec = 20;
		    timeout.tv_usec = 0;

		    if (setsockopt (new_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
		        error("setsockopt failed\n");

			bool_first = 0;
		}

	    // Get current time 
		time (&begin);
		diff = 0;

		//The first 5 min after the first read
		while(diff < 300)
		{
			// Read the data
			if ((ret = recv(new_sock, buf, BUF_SIZE-1, 0)) == -1) {
				printf("***timeout\n");
			}
			else if(ret != 0)
			{
				buf[ret] = '\0';

				printf("%s", buf);
				printf("***DELIMITER***\n");

				fprintf(f, "%s", buf);
				//fclose(f);
			}
			// Get current time 
			time(&current);	
			diff = (int)difftime(current, begin);
			printf("DIFF: %d\n", diff);
		}
		printf("***out of the while\n");
		// Flush the content
		fflush(f);
		printf("***after flush\n");
		// Close file
		fclose(f);
		// Replace the special characters
		replace_esc(tmpFileName, fileName);
		printf("***after replacement\n");
		// Sleep untill before the 
		time ( &current );
		printf("GOING TO SLEEP\n");
		sleep(700 - (int)difftime(current, begin)); //15*60
		printf("END OF SLEEPING\n");
	}
	//fclose(f);
	
	//printf("Server exiting\n");

	return 0;
}

int replace_esc(char* in_file, char* out_file) {
	FILE* in;
	FILE* out;
	unsigned int c, c_old;

	in = fopen(in_file,"r");
	out = fopen(out_file,"w");
	printf("#\n");

	c = fgetc(in);

	while(c != EOF) {
		if (c == '\t') {
			fputs("#011",out);
		} else if (c == '\n' && c_old != '#') {
			fputs("#012",out);
		} else if (c != '#') {
			fputc(c,out);
		}	
		c_old = c;
		c = fgetc(in);			
	}
	
	fclose(in);
	fclose(out);

	return 0;
}
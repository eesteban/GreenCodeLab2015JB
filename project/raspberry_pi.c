#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>

#define MAX_BUF 1024
#define PORT_UDP 514
#define PORT_TCP 7777
#define VM_IP "51.255.62.40"

int main(int argc, char *argv[])
{
	int input_sock, output_sock, i, aux=-1, maxR = MAX_BUF-2;
	struct sockaddr_in dir_pi_udp, dir_pi_tcp;
	char buf[MAX_BUF];
	char *b = &buf[16];

	/*Create the input_socket (Servier-Pi) using UDP*/
	if((input_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Error creating the input_socket");
		return 1;
	}

	memset(&dir_pi_udp, 0, sizeof(dir_pi_udp));
	dir_pi_udp.sin_family = AF_INET;
	dir_pi_udp.sin_addr.s_addr = htonl(INADDR_ANY);
	dir_pi_udp.sin_port = htons(PORT_UDP);
	
	if(bind(input_sock, (struct sockaddr *) &dir_pi_udp, sizeof(dir_pi_udp)) < 0)
	{
		perror("Error assigning an address input_socket");
		close(input_sock);
		return 1;
	}
	/*Create the output_socket (Pi-VM) using TCP*/
	if((output_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Error creating the output_sock");
		close(output_sock);
		return 1;
	}
	
	memset(&dir_pi_tcp, 0, sizeof(dir_pi_tcp));
	dir_pi_tcp.sin_family = AF_INET;
	dir_pi_tcp.sin_port = htons(PORT_TCP);
	if(inet_aton(VM_IP,&dir_pi_tcp.sin_addr) <= 0)
	{
		fprintf(stderr,"Error in VM's IP: %s\n", argv[1]);
		close(input_sock);
		close(output_sock);
		return 1;
	}
	
	while (aux<0)
	{   
		aux=connect(output_sock, (struct sockaddr *) &dir_pi_tcp, sizeof(dir_pi_tcp));
		if(aux<0){
			perror("Error connecting with the VM");
			sleep(1);
		}
	}

	printf("Connected with the VM\n");

	while(1){
		//printf("------Waiting for packets------ \n");
		/*Get the Packets from the server*/
		if((aux = recvfrom(input_sock, b, maxR,0,NULL,NULL)) < 0)
		{
			perror("Error receiving data");
			close(input_sock);
			return 1;
		}
		
		/*Get current date and prepend it to the message*/
		time_t timer;
		struct tm* tm_info;
		time(&timer);
		char date [16];

		tm_info = localtime(&timer);
	    strftime(date, 16,"%b %e %H:%M:%S", tm_info);

	    buf[15]=' ';
		aux+=16;

	    for(i=0; i<15;i++){
	    	buf[i]=date[i];
	    }

	    /*Parse message to send it in the correct format*/
	    buf[aux-1] = '#';
	    buf[aux] = '\n';
	    buf[aux+1] = '\0';

		/*Send the packets to the VM*/
		if(write(output_sock, buf, aux+1) < aux+1)
		{
			perror("Error sending data");
			close(output_sock);
			return 1;
		}/*else{
			printf("Data sent to the server!\n");
			printf("%.*s\n\n",aux+1, buf);
		}*/
	}

	close(input_sock);
	close(output_sock);

	return 0;
}
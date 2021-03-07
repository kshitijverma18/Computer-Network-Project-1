/* f20171145@hyderabad.bits-pilani.ac.in Kshitij Verma */

/*This is a socket program written in C without the use of high level libraries like Curl to download a webpage through Proxy. The program creates a socket between the device
and the Squid Proxy server using the IP address and port number of the proxy server. The proxy requires username and password authentication in base 64 encoded format. 
The GET request is then sent to the proxy to download the webpage. The header message is read and checked for redirection. If the re-direction is required, the absolute 
location of the new redirected webpage is generated and the message to fetch webpage from the new redirected location is again sent. The code also downloads the logo of 
a particular webpage http://info.in2p3.fr/
*/

/* 
To Compile the code: gcc -o http_proxy_download http_proxy_download.c
To Run the code: ./http_proxy_download.out info.in2p3.fr 182.75.45.22 13128 csf303 csf303 index.html logo.gif
*/


#include<stdio.h>
#include<stdlib.h>
#include<string.h>	
#include<sys/socket.h>
#include<arpa/inet.h>	
#include<stdio.h>
#include<unistd.h>
#include<stdbool.h> 

char header_response[4000];
char download_http_msg[1000];
char header_message[1000];
char web[1000];
bool redirection_required=false;
char *website;
char *proxy_IP;
int port;
char *login;
char *password;
char *html_file;
char *logo;
char server_reply[4069];
char image_url[1000];
char Base64encoded[200];

void base64Encoder(char input_str[], int len_str) 
{ 
	int length_string=len_str;	
	int eight=8;
	int index, no_of_bits = 0, pad = 0, val = 0, count = 0, temp; 
	int i, j, k = 0;
	int three=3;
	char character_set[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	while(i<length_string)
	{
		int val = 0;
		int count = 0;
		int no_of_bits = 0;
		int abc=i+2;  
		for (j = i; j < length_string && j <= abc;j++) 
		{  
			count++;              
			val = val << eight;               
			val = val | input_str[j];       						             
		}  
		no_of_bits = count * eight; 
		pad = (count*eight) % three;
		while (no_of_bits != 0)  
		{ 
			if(no_of_bits<(7-1))
			{
				temp = (7-1) - no_of_bits;                                     
				index = (val << temp) & (6*10+3);  
				no_of_bits = 0; 
			}              
			else if(no_of_bits >= (7-1)) 
			{ 
				temp = no_of_bits - (7-1); 
				index = (val >> temp) & (6*10+3);  
				no_of_bits -= (7-1);          
			}			
			Base64encoded[k++] = character_set[index]; 
		}
		i=i+three; 
	}
	for (i = 0; i < length_string; i =i+ three)
	{

	}
	for (i = 1; i <= pad; i++)  
	{ 
		Base64encoded[k++] = '='; 
	}  
	Base64encoded[k] = '\0'; 	
}

void get_download_message() 
{
	memset(download_http_msg,'\0',sizeof(download_http_msg));
	char temp1[]="GET http://";
	char temp2[]=" HTTP/1.1\r\nHost: ";	 
	char temp3[]="\r\nProxy-Authorization: Basic ";
	char temp4[]="\r\nConnection: close\r\n\r\n";
	strcat(download_http_msg,temp1);
	strcat(download_http_msg,web);
	strcat(download_http_msg,temp2);
	strcat(download_http_msg,web);
	strcat(download_http_msg,temp3);
	strcat(download_http_msg,Base64encoded);
	strcat(download_http_msg,temp4);	
}

void get_header(int socket_desc)
{	
	memset(header_response,0,sizeof(header_response));	
	char server_reply[2048];
	int read_size;
	int counter=0;
	char t[] = "\r\n\r\n";
	char *pt=NULL;
	bool header_received=false;
	if (send(socket_desc , download_http_msg , strlen(download_http_msg) , 0) < 0)
	{
		puts("Failed to Connect");
		return ;
	}	
	do
	{
		read_size = read(socket_desc, server_reply, 2000);		
		if (read_size == -1)
		{
			printf("Got Disconnected from server");
		}
		else
		{	
			if(header_received==false)
			{
				pt = strstr(server_reply, t);
				if(pt!=NULL)
				{
					char *header_start = server_reply;
					while (header_start != pt + strlen(t))
					{
						header_response[counter++] = *header_start;	
						header_start++;
					}
					header_received=true;
					break;			
				}
				else
				{
					char *header_start = server_reply;
					while (header_start != server_reply + (read_size)-(2-1))
					{
						header_response[counter++] = *header_start;	
						header_start++;
					}
				}
			}
		}

	}while (read_size > 0 );
	printf("Header Message Received\n");	
}
bool check_redirection() 
{	
	char break_line='\r';
	char location[]="Location: http://";
	char *temp=strstr(header_response,location);
	if(temp==NULL)
	{
		return false;
	}
	memset(web,'\0',sizeof(web));
	char *i=temp;
	i=i+strlen(location);
	int cnt=0;
	for( i; *i!=break_line; i++)
	{
		web[cnt++]=*i;
	}
	return true;
}
void download_complete_website(int socket_desc)
{	
	
	FILE *fptr;
	fptr = fopen(html_file, "w");
	char server_reply[2048];
	int read_size;
	if (send(socket_desc , download_http_msg , strlen(download_http_msg) , 0) < 0)
	{
		puts("Send failed");
		return ;
	}
	int counter=0;
	char t[] = "\r\n\r\n";
	char *pt=NULL;
	bool header_received=false;
	bool got_image_location=false;
	do
	{
		read_size = read(socket_desc, server_reply, 2000);	
		if (read_size == -1)
		{
			printf("DISCONNECTED FROM THE SERVER");
		}
		else
		{			
			if(header_received==false)
			{
				pt = strstr(server_reply, t);
				if(pt!=NULL)
				{
					//printf("%.*s", read_size - (pt + 4 - server_reply), pt + strlen(t));
					fprintf(fptr, "%.*s", read_size - (pt + 3+1 - server_reply), pt + strlen(t));
					header_received = true;
				}
				else
				{
					//printf("%.*s", read_size, server_reply);
					fprintf(fptr, "%.*s", read_size, server_reply);
				}
			}
			else
			{
				//printf("%.*s", read_size, server_reply);
				fprintf(fptr, "%.*s", read_size, server_reply);
			}
		}

	}while (read_size > 0 );	
	fclose(fptr);
}

int download_webpage(bool checking_for_redirection)
{
	struct sockaddr_in server;
	int socket_desc=socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Failed to create a socket. Exiting.!\n");
		return 1;
	}
	else
	{
		printf("\nSocket created successfully\n");
	}		
	server.sin_addr.s_addr = inet_addr(proxy_IP);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);	
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{	
		puts("connect error");
		return 1;
	}	
	memset(header_response,0,sizeof(header_response));
	get_download_message(); 
	if(checking_for_redirection==true)
	{
		get_header(socket_desc);			
		bool redirection_required=check_redirection();
		if(redirection_required==true)
		{
			printf("\nRedirection Required\n");
			printf("New website:%s",web);
			close(socket_desc);
			return 0;
		}
		else
		{
			printf("\nNo Redirection Required. Good to go\n");
			close(socket_desc);
			return 1;
		}
	}
	else
	{	
	download_complete_website(socket_desc);
	close(socket_desc);
	return 1;
	}

}
void download_image()
{
	FILE *downloaded_file_pointer;
	char info_website_content[2000];
	downloaded_file_pointer=fopen(html_file,"r");
	if(downloaded_file_pointer==NULL)
	{
		printf("Failed to open file.!");
	}
	else
	{
		
	}
	int cnt2=0;
	while((fscanf(downloaded_file_pointer,"%c",&info_website_content[cnt2]))!=EOF) 
	{          
		cnt2++;
	}
	char extracted_image_url_from_file[100];
	char image[] = "<IMG SRC=";
	char *i = strstr(info_website_content, image);
	if (i != NULL)
	{
			
		char *last = i;
		while (*last != '>')
		{
			last++;
		}
		i = i + strlen("<IMG SRC=") +1 ;
		int j = 0;
		while (i != last)
		{
			extracted_image_url_from_file[j] = *i;
			i++;
			j++;
		}
		extracted_image_url_from_file[strlen(extracted_image_url_from_file) - 1] = '\0';		
	}	
	int read_size;
	printf("Attempting to download Image");
	FILE *image_pointer;
	struct sockaddr_in server;
	image_pointer = fopen(logo, "wb");
	int socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc != -1)
	{
		printf("\nSocket created successfully\n\n");	
	}
	else
	{
		printf("Failed to create a socket. Exiting.!\n");		
	}
	server.sin_addr.s_addr = inet_addr( proxy_IP);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		printf("connect error");	
	}
	char slash[] = "/";
	char image_download_message[1000];
	memset(image_download_message,0,sizeof(image_download_message));
	char temp1[] = "GET http://";
	strcat(image_download_message, temp1);
	strcat(image_download_message, website);
	strcat(image_download_message, slash);
	strcat(image_download_message, extracted_image_url_from_file);
	char temp2[] = " HTTP/1.1\r\nHost: ";
	strcat(image_download_message, temp2);
	strcat(image_download_message, website);
	char temp3[] = "\r\nProxy-Authorization: Basic ";
	char temp4[]="\r\nConnection: close\r\n\r\n";
	strcat(image_download_message, temp3);
	strcat(image_download_message,Base64encoded);
	strcat(image_download_message,temp4);	
	char m4[] = "\r\nConnection: close\r\n\r\n";
	strcat(image_download_message, m4);	
	char image_buffer[31325];
	int first_time = 1;
	if (send(socket_desc , image_download_message , strlen(image_download_message) , 0) < 0)
	{
		printf("Send failed");		
	}	
	int count = 0;
	do
	{		
		read_size = read(socket_desc, image_buffer, 31200);		
		if (read_size == -1)
		{
			printf("Disconnected from server");
		}
		else
		{
			if (first_time == 1)
			{				
				char t[] = "GIF";
				char *pt = strstr(image_buffer, t);				
				if (pt != NULL)
				{					
					fwrite(pt, 1, read_size - (pt - image_buffer), image_pointer);
					first_time = 0;
				}
			}
			else
			{
				fwrite(image_buffer, 1, read_size, image_pointer);
			}
		}

	} while (read_size > 0 );
	printf("Image Download Complete. Please Check\n");
	close(socket_desc);
	fclose(image_pointer);
}
int main(int argc, char *argv[])
{
	printf("Computer Networking Assignment\n");
	website=malloc(strlen(argv[1]) + 1);strcpy(website,argv[1]);
	proxy_IP=malloc(strlen(argv[2]) + 1);strcpy(proxy_IP,argv[2]);
	port=atoi(argv[3]);
	login=malloc(strlen(argv[4]) + 1);strcpy(login,argv[4]);
	password=malloc(strlen(argv[5]) + 1);strcpy(password,argv[5]);
	html_file=malloc(strlen(argv[6]) + 1);strcpy(html_file,argv[6]);
	logo=malloc(strlen(argv[7]) + 1);strcpy(logo,argv[7]);
	char loginid_password[200];
	memset(loginid_password,0,sizeof(loginid_password));	
	char colon[]=":";
	strcat(loginid_password,login);
	strcat(loginid_password,colon);
	strcat(loginid_password,password);
	base64Encoder(loginid_password,strlen(loginid_password));
	printf("Website Name:%s\n",website);
	printf("Proxy Ip:%s\n",proxy_IP);	
	printf("Proxy port:%d\n",port);	
	printf("Name of Html file:%s\n",html_file);
	printf("Name of logo:%s\n",logo);	
	memset(header_response,'\0',sizeof(header_response));
	strcpy(web,website);
	int loop=0;
	bool checking_for_redirection=true;	
	while(loop==0)
	{
		loop=download_webpage(checking_for_redirection);
	}	
	checking_for_redirection=false;
	bool download_successful=download_webpage(false);
	if(download_successful==true)
	{
		printf("%s","Website download Complete.! Please Check.\n");
	}
	if (strcmp(website, "info.in2p3.fr") == 0)
	{
		download_image();
	}	
}



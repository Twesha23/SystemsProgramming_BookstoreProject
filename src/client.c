#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include "../include/myheader.h"
#include <sys/types.h>
#include <sys/wait.h>

//include <netinet/in.h>
//include <arpa/inet.h>

#define	MAXLINE	 8192  /* Max text line length */
#define	READ	0      /* The index of the “read” end of the pipe */ 
#define	WRITE	1 

Book* bookList=NULL;
int bookCnt=0;
int (*fd)[2];

void exitHandler()
{
	freeBookList(bookList,bookCnt);
	free(fd);
	exit(0);
}

int open_clientfd(char *hostname, char *port) {

    int clientfd;
    struct addrinfo hints, *listp, *p;
	char host[MAXLINE],service[MAXLINE];
    int flags;

    /* Get a list of potential server addresses */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;  /* Open a connection */
    hints.ai_flags = AI_NUMERICSERV;  /* ... using a numeric port arg. */
    hints.ai_flags |= AI_ADDRCONFIG;  /* Recommended for connections where we get IPv4 or IPv6 addresses */
    getaddrinfo(hostname, port, &hints, &listp);
  
    /* Walk the list for one that we can successfully connect to */
    for (p = listp; p; p = p->ai_next) {
        /* Create a socket descriptor */
        if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) 
            continue; /* Socket failed, try the next */

		flags = NI_NUMERICHOST | NI_NUMERICSERV; /* Display address string instead of domain name and port number instead of service name */		
		getnameinfo(p->ai_addr, p->ai_addrlen, host, MAXLINE, service, MAXLINE, flags);
        printf("host:%s, service:%s\n", host, service);
		
        /* Connect to the server */
        if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1) 
		{
			printf("Connected to server %s at port %s\n", host, service);
            break; /* Success */
		}
        close(clientfd); /* Connect failed, try another */  //line:netp:openclientfd:closefd
    } 

    /* Clean up */
    freeaddrinfo(listp);
    if (!p) /* All connects failed */
        return -1;
    else    /* The last connect succeeded */
        return clientfd;
}

int main(int argc, char **argv)
{
	if(argc<3)
	{
		fprintf(stderr,"Enter 3 arguments (client name , server's host name and port number)\n");
		exit(0);
	}
    int clientfd;
    char *host, *port, buf[MAXLINE];
    host = argv[1];
    port = argv[2];
    clientfd = open_clientfd(host, port);
	int login_count=1;
	char username[50],password[50];
	
	while(login_count<=5)
	{
		
		printf("Enter username: ");
		scanf("%s",username);
		printf("Enter password: ");
		scanf("%s",password);
		strcpy(buf,username);
		strcat(buf," ");
		strcat(buf,password);
		write(clientfd, buf, strlen(buf));
		read(clientfd, buf, MAXLINE);
		printf("%s\n",buf);
		if(strcmp(buf,"Login successful")==0)
			break;
		if(login_count==3)
		{
			fprintf(stderr,"3 attempts failed.Try again in 10 seconds\n");
			sleep(10);
		}
		login_count++;
	}
	
	if(login_count==6)
	{
		fprintf(stderr,"5 attempts failed.Try again by restarting\n");
		close(clientfd);
		exit(0);
	}
	
	//Connection with server successful
	int choice,id,copies;
	float discount;
	double total=0;
	int cnt=0,status;
	char bid[4],num_copies[4],disc[6],bname[50],aname[50],genre[50],price[10],totalprice[10],filename[100];
	
	signal(SIGINT, exitHandler);
	strcpy(filename,"../client_data/");
	strcat(filename,username);
	strcat(filename,"_book_data.txt");
	bookList=getBookList(filename,&bookCnt);
	
	while(1)
	{
	    printf("Enter book id: (enter 0 to exit) ");
		scanf("%d",&id);
		if(id==0)
			break;			
		printf("Enter number of copies you want to send: ");
		scanf("%d",&copies);
		printf("Enter discount price  in percentage ");
		scanf("%f",&discount);
		if(cnt==0)
			fd=(int(*)[2])malloc(sizeof(int[2]));
		else
			fd=(int(*)[2])realloc(fd,sizeof(int[2])*(cnt+1));
		pipe(fd[cnt]); /* For fd1, Parent - writer, Child - reader */
		if(fork()==0)
		{
			close(fd[cnt][WRITE]); /* Close unused end */
			read( fd[cnt][READ], bid, 4 ); /* Receive */
			read( fd[cnt][READ], num_copies, 4 ); /* Receive */
			read( fd[cnt][READ], disc, 6); /* Receive */		
			id=atoi(bid);
			copies=atoi(num_copies);
			discount=atof(disc);			
			if(id>bookCnt)
			{
				printf("Invalid id of book\n");
				close(fd[cnt][READ]);
				exit(0);
			}
						
			double price_after_discount=bookList[id-1].price-(bookList[id-1].price * discount/100);
			total+=(copies*price_after_discount);	
			sprintf(bname,"%49s",bookList[id-1].bname);
			sprintf(aname,"%49s",bookList[id-1].aname);
			sprintf(genre,"%49s",bookList[id-1].genre);
			sprintf(price,"%09.2lf",bookList[id-1].price);
			sprintf(totalprice,"%09.2lf",total);
					
			write(clientfd,bid,strlen(bid)+1);
			write(clientfd,bname,strlen(bname)+1);
			write(clientfd,aname,strlen(aname)+1);
			write(clientfd,genre,strlen(genre)+1);
			write(clientfd,price,strlen(price)+1);
			write(clientfd,num_copies,strlen(num_copies)+1);
			write(clientfd,disc,strlen(disc)+1);
			write(clientfd,totalprice,strlen(totalprice)+1);
			close(fd[cnt][READ]);
			exit(0);
		}
		else{
			  close(fd[cnt][READ]); /* Close unused end */
			sprintf(bid, "%03d", id);
			sprintf(num_copies, "%03d", copies);
			sprintf(disc, "%05.2f", discount);
			write(fd[cnt][WRITE], bid, strlen(bid)+1); /* Send */
			write(fd[cnt][WRITE], num_copies, strlen(num_copies)+1); /* Send */
			write(fd[cnt][WRITE], disc, strlen(disc)+1); /* Send */
		}
				
		close(fd[cnt][WRITE]);
		cnt++;			
	}
		wait(&status);
		close(clientfd);
		kill(getpid(),SIGINT);
		exit(0);
}
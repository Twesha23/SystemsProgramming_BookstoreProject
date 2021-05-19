#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include<signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../include/myheader.h"

/* Misc constants */
#define	MAXLINE	 8192  /* Max text line length */
#define LISTENQ  1024  /* Second argument to listen() */

Publication* publicationList=NULL;
int publicationcnt=0;
Book* book_list=NULL;
int book_cnt=0;

pthread_rwlock_t rw_lock;

void exitHandler()
{
	int status;
	wait(&status);
	freePublicationList(publicationList,publicationcnt);	
	exit(0);
}

void childExitHandler()
{
	int status;
	int cid=wait(&status);
	if WIFEXITED(status)
		printf("Child with PID %d terminated normally with exit code (number of books added) = %d\n", cid,WEXITSTATUS(status));
	else if WIFSIGNALED(status)
		printf("Child with PID %d was terminated by signal %d\n", cid, WTERMSIG(status) );
	else
		printf("Child with PID %d had abnormal termination\n", cid);
}

int open_listenfd(char *port) 
{
    struct addrinfo hints, *listp, *p;
    int listenfd, optval=1;
	char host[MAXLINE],service[MAXLINE];
    int flags;

    /* Get a list of potential server addresses */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;             /* Accept connections */
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; /* ... on any IP address AI_PASSIVE - used on server for TCP passive connection, AI_ADDRCONFIG - to use both IPv4 and IPv6 addresses */
    hints.ai_flags |= AI_NUMERICSERV;            /* ... using port number instead of service name*/
    getaddrinfo(NULL, port, &hints, &listp);

    /* Walk the list for one that we can bind to */
    for (p = listp; p; p = p->ai_next) {
        /* Create a socket descriptor */
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) 
            continue;  /* Socket failed, try the next */

        /* Eliminates "Address already in use" error from bind */
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,    //line:netp:csapp:setsockopt
                   (const void *)&optval , sizeof(int));

		flags = NI_NUMERICHOST | NI_NUMERICSERV; /* Display address string instead of domain name and port number instead of service name */
		getnameinfo(p->ai_addr, p->ai_addrlen, host, MAXLINE, service, MAXLINE, flags);
        printf("host:%s, service:%s\n", host, service);

        /* Bind the descriptor to the address */
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break; /* Success */
        close(listenfd); /* Bind failed, try the next */
    }

    /* Clean up */
    freeaddrinfo(listp);
    if (!p) /* No address worked */
        return -1;

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, LISTENQ) < 0) {
        close(listenfd);
	return -1;
    }
    return listenfd;
}

int login_info(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    if((n = read(connfd, buf, MAXLINE)) != 0) 
    buf[n] = '\0';
	const char s[2] = " ";
    char *token;
    char username[50],password[50];
    int count=0;
   /* get the first token */
   token = strtok(buf, s);
   
   /* walk through other tokens */
   while( token != NULL ) {
	   if(count==0)
	   {
		   strcpy(username,token);
	   }
	   else if(count==1)
	   {
		   strcpy(password,token);
	   }
    
	 count++;
     token = strtok(NULL, s);
   }
   int login=checkLogin(publicationList,publicationcnt,username,password);
   if(login==0)
	   strcpy(buf,"Login unsuccessful");
   else
		strcpy(buf,"Login successful");
		write(connfd,buf,strlen(buf)+1);
	
	return login;
}


void* write_bookList(void* arg)
{
	int* login_id=arg;
	pthread_rwlock_wrlock(&rw_lock);
	write_to_file("../server_data/bookstore_book_details.txt",book_list,book_cnt,publicationList[(*login_id)-1].username);
	pthread_rwlock_unlock(&rw_lock);
	return NULL;
}

int main(int argc, char **argv)
{
	if(argc<2)
	{
		fprintf(stderr,"Enter 2 arguments (server name and port number)\n");
		exit(0);
	}
	signal( SIGINT, exitHandler );
	signal(SIGCHLD, childExitHandler);
    int listenfd, connfd,login_count=1;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr; /* Enough room for any addr */
    char client_hostname[MAXLINE], client_port[MAXLINE];
    listenfd = open_listenfd(argv[1]);
	
	publicationList=getPublicationData("../server_data/publication_login.txt",&publicationcnt);
	if(publicationList==NULL)
	{
		fprintf(stderr,"File not found or file is empty\n");
		exit(1);
	}
	printf("Press CTRL+C to exit\n");
    while (1) {
	printf("Waiting for a new Client to connect\n");
        clientlen = sizeof(struct sockaddr_storage); /* Important! */
        connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
		int cpid=fork();
		if(cpid==0)
		{
				getnameinfo((struct sockaddr *) &clientaddr, clientlen,
				client_hostname, MAXLINE, client_port, MAXLINE, 0);
				printf("Connected to (%s, %s) via child process %d\n", client_hostname, client_port,getpid());
				printf("Started Communication with Client %s\n",client_hostname);
				int login_id=0;
				while((login_id=login_info(connfd))==0)
				{
					if(login_count==5)
					{
						printf("End Communication with Client %s by child process %d\n",client_hostname,getpid());
						close(connfd);
						exit(0);
					}
					login_count++;
				}
				
				int id,copies;
				float discount;
				double disc_price=0,mrp;
				char buf[2],bid[4],num_copies[4],disc[6],bname[50],aname[50],genre[50],price[10],totalprice[10];
				char *book_name,*auth_name,*book_genre;
				size_t n;
				while((n = read(connfd, bid, 4)) != 0) 
				{
					
					read( connfd, bname, 50 ); /* Receive */
					read( connfd, aname, 50 ); /* Receive */
					read( connfd, genre, 50 ); /* Receive */
					read( connfd, price, 10); /* Receive */
					read( connfd, num_copies, 4 ); /* Receive */
					read( connfd, disc, 6); /* Receive */
					read( connfd, totalprice, 10 ); /* Receive */
					
					trim(bname);
					trim(aname);
					trim(genre);
					id=atoi(bid);
					mrp=atod(price);
					copies=atoi(num_copies);
					discount=atof(disc);
					disc_price=atod(totalprice);
					printf("Server received Book Details from publication %s\n %d %s %s %s %lf %d %f %lf\n",publicationList[login_id-1].username,id,bname,aname,genre,mrp,copies,discount,disc_price);
					
					if(book_cnt==0)
						book_list = (Book *)malloc(sizeof(Book));
					else
						book_list = (Book *)realloc(book_list,(book_cnt+1)*sizeof(Book));
					book_list[book_cnt]=setBookData(id,bname,aname,genre,mrp,copies,discount,disc_price); 
					book_cnt++;
					
				}
				close(connfd);
				//thread
				pthread_t thread1;
				int ret=pthread_rwlock_init(&rw_lock, NULL);
				ret=pthread_create (&thread1, NULL, write_bookList, (void*)&login_id);
				if (ret != 0)
				printf("cant create thread: %s\n", strerror(ret));
				pthread_join(thread1, NULL);
				ret = pthread_rwlock_destroy(&rw_lock);
				printf("End Communication with Client %s by child process %d and connection closed\n",client_hostname,getpid());
				freeBookList(book_list,book_cnt);
				exit(book_cnt);
		}
    }
}

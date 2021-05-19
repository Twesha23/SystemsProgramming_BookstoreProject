#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include "../include/myheader.h"

Book* bookList=NULL;
int bookCnt=0;

void exitHandler()
{
	freeBookList(bookList,bookCnt);
	exit(0);
}

int main()
{
	char filename[100],pname[50];
	signal( SIGINT, exitHandler );
	printf("Enter your publication company's user name:");
	scanf("%s",pname);
	strcpy(filename,"../client_data/");
	strcat(filename,pname);
	strcat(filename,"_book_data.txt");
	bookList=getBookList(filename,&bookCnt);
	char bname[50],aname[50],genre[100];
	int choice;
	double price;
	while(1)
	{
		printf("1: Add new book details to your local file\n");
		printf("2: View all book details from your local file\n");
		printf("0: Exit\n");
		printf("Enter choice: ");
		scanf("%d",&choice);
		switch(choice)
		{
			case 2:
				if(bookList==NULL)
				{
					printf("File not found or file is empty\n");
				}
				else
					viewBookList(bookList,bookCnt);
				break;
			case 1:
				printf("Enter book name: ");
				scanf("%s",bname);
				printf("Enter author name: ");
				scanf("%s",aname);
				printf("Enter genre: ");
				scanf("%s",genre);
				printf("Enter price: ");
				scanf("%lf",&price);
				bookList=addBookToList(filename,bookList,&bookCnt,bname,aname,genre,price);
				break;
			case 0:
				kill(getpid(),SIGINT);
				break;
			default:
				printf("Invalid choice\n");
				break;
		}
	}
}
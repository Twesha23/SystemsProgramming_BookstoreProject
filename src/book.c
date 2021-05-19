#include<stdio.h>
#include<stdlib.h>
#include<ctype.h> 
#include<string.h>
#include "../include/myheader.h"
Book setBookData(int id,char* bname,char* aname,char* genre,double price,int copies,float discount,double total_amt)
{
	Book* newBook = (Book *)malloc(sizeof(Book));
		newBook->id=id;
		newBook->bname=(char *)malloc(sizeof(char) * strlen(bname+1));
		strcpy(newBook->bname,bname);
		newBook->aname=(char *)malloc(sizeof(char) * strlen(aname+1));
		strcpy(newBook->aname,aname);
		newBook->genre=(char *)malloc(sizeof(char) * strlen(genre+1));
		strcpy(newBook->genre,genre);		
		newBook->price=price;
		newBook->copies=copies;
		newBook->discount=discount;
		newBook->total_amt=total_amt;
	return *newBook;
}

Book* getBookList(char* filename,int *book_cnt)
{
	FILE *fp; 
	int id;
	double mrp;	
	Book* book_list=NULL;
	char name[20],author[50],genre[50]; //creating char array to store data of file  
	fp = fopen(filename, "r"); 
	if(fp==NULL)
	{
		printf("Error.. could not open file %s\n",filename);
		return NULL;
	}
	while(fscanf(fp, "%d %s %s %s %lf", &id,name,author,genre,&mrp)!=EOF)
	{  
		
		if(*book_cnt==0)
			book_list = (Book *)malloc(sizeof(Book));
		else
			book_list = (Book *)realloc(book_list,(*book_cnt+1)*sizeof(Book));
		book_list[*book_cnt]=setBookData(id,name,author,genre,mrp,0,0,0); 
		(*book_cnt)++;
	} 
	fclose(fp);	
	return book_list;
	
}
Book* addBookToList(char* filename,Book* book_list,int *book_cnt,char* name,char* author,char* genre,double mrp)
{
	FILE *fp; 
	fp = fopen(filename, "a"); 
	if(fp==NULL)
	{
		printf("Error.. could not open file %s\n",filename);
		return NULL;
	}
	if(book_cnt==0)
		book_list = (Book *)malloc(sizeof(Book));
	else
		book_list = (Book *)realloc(book_list,(*book_cnt+1)*sizeof(Book));
	book_list[*book_cnt]=setBookData(*book_cnt+1,name,author,genre,mrp,0,0,0); 
	fprintf(fp, "%d %s %s %s %lf\n",*book_cnt+1,name,author,genre,mrp);
	printf(" Book with id:%d added to your local file\n",book_list[*book_cnt].id);
	(*book_cnt)++;
	fclose(fp);	
	return book_list;	
}
void viewBookList(Book* book_list,int count)
{
	printf("Your local Book List\n");
	printf("--------------------------------------------------------------------------------------------|\n");
	printf("| Book Id | Book Name            | Author               | Genre                | Price      |\n");
	for(int i=0;i<count;i++)
	{
		printf("--------------------------------------------------------------------------------------------|\n");
		printf("| %-7d | %-20s | %-20s | %-20s | %-9.2lf  |\n", book_list[i].id,book_list[i].bname,book_list[i].aname,book_list[i].genre,book_list[i].price);
	}
	printf("--------------------------------------------------------------------------------------------|\n");
}
void freeBookList(Book* book_list,int count)
{
	printf("Clearing Book List memory\n");
	for(int i=0;i<count;i++)
	{
		free(book_list[i].bname);
		free(book_list[i].aname);
		free(book_list[i].genre);
	}
	free(book_list);
}

void write_to_file(char* filename,Book* book_list,int count,char* publicationName)
{
	FILE *fp; 
	fp = fopen(filename, "a"); 
	if(fp==NULL)
	{
		printf("Error.. could not open file %s\n",filename);
		return;
	}
	fprintf(fp,"\n---Book details of %s---\n",publicationName);
	fprintf(fp,"-------------------------------------------------------------------------------------------------------------------------------|\n");
	fprintf(fp,"| Book Id | Book Name            | Author               | Genre                | Price      | Copies | Discount  | Total Price |\n");
	for(int i=0;i<count;i++)
	{
		
		fprintf(fp, "| %-7d | %-20s | %-20s | %-20s | %-9.2lf  | %-6d | %-9.2lf | %-11.2f |\n",book_list[i].id,book_list[i].bname,book_list[i].aname,book_list[i].genre,book_list[i].price,book_list[i].copies,book_list[i].discount,book_list[i].total_amt);
	}
	fprintf(fp,"-------------------------------------------------------------------------------------------------------------------------------|\n");
	fclose(fp);	
	
}

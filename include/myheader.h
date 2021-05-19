#include<stdbool.h>  
#include<string.h>
#include<ctype.h>
struct st_book
{
	int id;
	char* bname;
	char* aname;
	char* genre;
	double price;
	int copies;
	float discount;
	double total_amt;
	
	
};
typedef struct st_book Book;
struct st_publication
{
	int id;
	char* username;
	char* password;
};
typedef struct st_publication Publication;

Publication* getPublicationData(char* filename,int* publication_cnt);
int checkLogin(Publication* publication,int cnt,char* name,char* password);
Publication setPublicationData(int id,char* name,char* password);
Book setBookData(int id,char* bname,char* aname,char* genre,double price,int copies,float discount,double total_amt);
Book* getBookList(char* filename,int *book_cnt);
void viewBookList(Book* book_list,int count);
Book* addBookToList(char* filename,Book* book_list,int *book_cnt,char* name,char* author,char* genre,double mrp);
void freeBookList(Book* book_list,int count);
double atod(char s[]);
void trim(char *s);
void write_to_file(char* filename,Book* book_list,int count,char* publicationName);
void freePublicationList(Publication* publicationList,int publicationcnt);
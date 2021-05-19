#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>  
#include "../include/myheader.h"

Publication setPublicationData(int id,char* name,char* password)
{
	Publication* newPublication = (Publication *)malloc(sizeof(Publication));
	newPublication->id=id;
	newPublication->username=(char *)malloc(sizeof(char) * strlen(name+1));
	strcpy(newPublication->username,name);
	newPublication->password=(char *)malloc(sizeof(char) * strlen(password+1));
	strcpy(newPublication->password,password);	
	return *newPublication;
}
Publication* getPublicationData(char* filename,int* publication_cnt)
{
	FILE *fp; 
	int id;
	Publication* publication_list=NULL;
	char name[50],password[50];//creating char array to store data of file  
	fp = fopen(filename, "r"); 
	if(fp==NULL)
	{
		printf("Error.. could not open file %s\n",filename);
		return NULL;
	}
	while(fscanf(fp, "%d %s %s", &id,name,password)!=EOF)
	{  
		//printf("%d %s %s \n", id,name,password ); 
		if(*publication_cnt==0)
			publication_list = (Publication *)malloc(sizeof(Publication));
		else
			publication_list = (Publication *)realloc(publication_list,(*publication_cnt+1)*sizeof(Publication));
		publication_list[*publication_cnt]=setPublicationData(id,name,password);
		(*publication_cnt)++;
	} 
	fclose(fp);	
	return publication_list;
}
int checkLogin(Publication* publication,int cnt,char* name,char* password)
{
	int i=0;
	while(i<cnt)
	{
		if(strcmp(name,publication[i].username)==0 && strcmp(password,publication[i].password)==0)
		return publication[i].id;
		i++;
	}
	return 0;	
}
void freePublicationList(Publication* publicationList,int publicationcnt)
{
	printf("Clearing Publication List memory\n");
	for(int i=0;i<publicationcnt;i++)
	{
		free(publicationList[i].username);
		free(publicationList[i].password);
		
	}
	free(publicationList);
}

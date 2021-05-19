#include<stdio.h>
#include<stdlib.h>
#include<ctype.h> 
#include<string.h>
#include "../include/myheader.h"
double atod(char s[])
{
    double val, power;
    int i, sign;
for (i = 0; isspace(s[i]); i++) /* skip white space */
    ;
    sign = (s[i] == '-') ? -1 : 1;
    if (s[i] == '+' || s[i] == '-')
        i++;
    for (val = 0.0; isdigit(s[i]); i++)
        val = 10.0 * val + (s[i] - '0');
    if (s[i] == '.')
        i++;
    for (power = 1.0; isdigit(s[i]); i++) {
        val = 10.0 * val + (s[i] - '0');
        power *= 10;
    }
    return sign * val / power;
}
void trim(char *s)
{
	int i,j;
	for(i=0;s[i]==' '||s[i]=='\t';i++);
 
	for(j=0;s[i];i++)
	{
		s[j++]=s[i];
	}
	s[j]='\0';  
}
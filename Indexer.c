#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
struct pages
{
	char *p;
	int count;
	struct pages *next;
};
struct  words
{
	char *w;
	struct words *nextw;
	struct pages *nextp;
};
struct Hash
{
	int val;
	struct words *fir,*las;
};
int kbhit(void)
{
	struct termios oldt, newt;
	int ch;
  	int oldf;
	tcgetattr(STDIN_FILENO, &oldt);
 	newt = oldt;
  	newt.c_lflag &= ~(ICANON | ECHO);
  	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
 	fcntl(STDIN_FILENO, F_SETFL, oldf);
	if(ch != EOF)
  	{
		ungetc(ch, stdin);
		return 1;
  	}
	return 0;
}
void extractWords(char *html)
{
	int i,j=0,flag=0;
	for(i=0;i<strlen(html);)
	{
		if(html[i]=='<'&&(html[i+1]=='h'||html[i+1]=='H')&&(html[i+2]>='1'&&html[i+2]<='6'))
		{
			while(html[i++]!='>');
			while(html[i]!='<'||html[i+1]!='/'||html[i+4]!='>')
			{
				if(html[i]=='<')
				{
					while(html[i++]!='>');
					html[j++]=' ';
					continue;
				}
				if((html[i]>='a'&&html[i]<='z')||(html[i]>='A'&&html[i]<='Z')||html[i]==' '||(html[i]>='0'&&html[i]<='9'))
					html[j++]=html[i++];
				else
				{
					i++;
					html[j++]=' ';
				}
			}
			html[j++]=' ';	
		}
		else if(strncmp(html+i,"<title",6)==0)
		{
			while(html[i++]!='>');
			while(html[i]!='<')
			{
				if((html[i]>='a'&&html[i]<='z')||(html[i]>='A'&&html[i]<='Z')||html[i]==' '||(html[i]>='0'&&html[i]<='9'))
					html[j++]=html[i++];
				else
				{
					i++;
					html[j++]=' ';
				}
			}
			html[j++]=' ';
			flag=1;
		}
		else if(flag==1&&strncmp(html+i,"<meta",5)==0)
		{
			i+=5;
			while(html[i]!='>')
			{
				if(strncmp(html+i,"name",4)==0||strncmp(html+i,"content",7)==0)
				{
					while(html[i++]!='\"');
					if(strncmp(html+i,"http",4)==0)
					{
						while(html[i]!='\"')
							++i;
					}
					while(html[i]!='\"')
					{
						if((html[i]>='a'&&html[i]<='z')||(html[i]>='A'&&html[i]<='Z')||html[i]==' '||(html[i]>='0'&&html[i]<='9'))
							html[j++]=html[i++];
						else
						{
							i++;
							html[j++]=' ';
						}
					}
					html[j++]=' ';
				}
				++i;
			}
		}
		else
			++i;
	}
	html[j]='\0';
}
void retreive_file(char *html,char filename[])
{
	int i=-1;
	FILE *fp;
	fp=fopen(filename,"r");
	while(!feof(fp))
	{
		html[++i]=fgetc(fp);
		if(html[i]=='\t'||(html[i]==' '&&html[i-1]==' '))
			--i;
	}
	html[++i]='\0';
	fclose(fp);
}
long int get_file_size(char filename[])
{
	FILE *fp;
	fp=fopen(filename,"r");
	fseek(fp,0L,SEEK_END);
	long int res=ftell(fp);
	fclose(fp);
	return res;
}
int retkey(char *token)
{
	int val=0;
	for(int i=0;token[i];++i)
		val=val+token[i];
	val=val%50;
	return val;
}
void separateWords(char *html,struct words *wordsarray,char url[],struct Hash h[],int opt)
{
	static int f=0;
	int flag=0,key;
	struct words *b=wordsarray;
	char *token=strtok(html," ");
	while(token!=NULL)
	{
		if(strlen(token)>2)
		{
			key=retkey(token);
			if(f==0)
			{
				struct pages *pagearray=(struct pages *)malloc(sizeof(struct pages));
				b=wordsarray;
				b->w=(char *)malloc((strlen(token)+1)*sizeof(char));
				strcpy(b->w,token);
				b->nextw=NULL;
				f=1;
				pagearray->p=(char *)malloc((strlen(url)+1)*sizeof(char));
				strcpy(pagearray->p,url);
				pagearray->count=opt;
				pagearray->next=NULL;
				b->nextp=pagearray;
				h[key].val=1;
				h[key].fir=b;
				h[key].las=b;
			}
			else if(h[key].val!=0)
			{
				struct words *x=h[key].fir,*y=h[key].las;
				flag=0;
				while(flag==0)
				{
					if(strcmp(x->w,token)==0)
					{
						flag=1;
						struct pages *pa=x->nextp;
						while(pa->next!=NULL)
							pa=pa->next;
						if(strcmp(pa->p,url)!=0)
						{
							struct pages *pagearray=(struct pages *)malloc(sizeof(struct pages));
							pagearray->p=(char *)malloc((strlen(url)+1)*sizeof(char));
							strcpy(pagearray->p,url);
							pagearray->count=opt;
							pagearray->next=NULL;
							pa->next=pagearray;
						}
						else
							pa->count+=1;
						break;
					}
					if(x==y)
						break;
					x=x->nextw;
				}
				if(flag==0)
				{
					struct words *temp=(struct words *)malloc(sizeof(struct words)),*temp1;
					temp->w=(char *)malloc((strlen(token)+1)*sizeof(char));
					strcpy(temp->w,token);
					temp1=y->nextw;
					y->nextw=temp;
					temp->nextw=temp1;
					y=y->nextw;
					struct pages *pagearray=(struct pages *)malloc(sizeof(struct pages));
					pagearray->p=(char *)malloc((strlen(url)+1)*sizeof(char));
					strcpy(pagearray->p,url);
					pagearray->count=opt;
					pagearray->next=NULL;
					y->nextp=pagearray;
					h[key].las=y;
				}
			}
			else if(h[key].val==0)
			{
				while(b->nextw!=NULL)
					b=b->nextw;
				struct words *temp=(struct words *)malloc(sizeof(struct words));
				temp->w=(char *)malloc((strlen(token)+1)*sizeof(char));
				strcpy(temp->w,token);
				temp->nextw=NULL;
				b->nextw=temp;
				b=b->nextw;
				struct pages *pagearray=(struct pages *)malloc(sizeof(struct pages));
				pagearray->p=(char *)malloc((strlen(url)+1)*sizeof(char));
				strcpy(pagearray->p,url);
				pagearray->count=opt;
				pagearray->next=NULL;
				b->nextp=pagearray;
				h[key].val=1;
				h[key].fir=b;
				h[key].las=b;
			}
		}
		token=strtok(NULL," ");
	}
}
void getFileUrl(char *html,char url[])
{
	int i=0,j=0;
	while(html[i]!=' ')
		url[j++]=html[i++];
	url[j]='\0';
}
int main(int argc,char *argv[])
{
	int lol=0;
	struct Hash hash[50];
	for(int i=0;i<50;++i)
		hash[i].val=0;
	struct words *wordsarray=(struct words *)malloc(sizeof(struct words));
	char filename[20],*html,url[100],ch=' ';
	FILE *fp;
	if(fp=fopen("continue.txt","r"))
	{
		fscanf(fp,"%d\n",&lol);
		char tem[100];
		html=(char *)malloc(100*sizeof(int));
		int te;
		while(!feof(fp))
		{
			fscanf(fp,"%s %d\n",tem,&te);
			if(strncmp(tem,"http",4)==0||strncmp(tem,"www.",4)==0)
			{
				separateWords(html,wordsarray,tem,hash,te);
			}
			else
			{
				strcpy(html,tem);
			}
		}
		fclose(fp);
	}
	while(1)
	{
		if(kbhit())
		{
			ch=getchar();
		}
		if(ch=='e')
			break;
		++lol;
		sprintf(filename,"index%d.txt",lol);
		if(fp=fopen(filename,"r"))
		{
			fclose(fp);
			long long size=get_file_size(filename);
			++size;
			html=(char *)malloc(size*sizeof(char));
			retreive_file(html,filename);
			getFileUrl(html,url);
			extractWords(html);
			separateWords(html,wordsarray,url,hash,1);
		}
		else
		{
			--lol;
			break;
		}
	}
	fp=fopen("continue.txt","w");
	fprintf(fp,"%d\n",lol);
	while(wordsarray!=NULL)
	{
		fprintf(fp,"%s %d\n",wordsarray->w,retkey(wordsarray->w));
		struct pages *pa=wordsarray->nextp;
		while(pa!=NULL)
		{
			fprintf(fp,"%s %d\n",pa->p,pa->count);
			pa=pa->next;
		}
		wordsarray=wordsarray->nextw;
	}
	fclose(fp);
	return 0;
}

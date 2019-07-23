#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<math.h>
#define BASE_URL "https://www.chitkara.edu.in"
#define MAX 2
int lol=0;
struct crawl
{
	char *k;
	int level,done;
	int key;
	struct crawl *prev,*next;
};
struct Hash
{
	struct crawl *start;
	struct crawl *end;
	int opt;
};
void validate(char *str2)
{
	int i,j=0;
	for(i=0;str2[i]&&j<1;++i)
	{
		if(BASE_URL[i+8]!=str2[i]&&BASE_URL[i+8])
			break;
		if(str2[i]=='/')
			++j;
	}
	if(j!=1&&str2[i]!='\0')
	{
		printf("Not same as BASE URL!!!\n");
		exit(0);
	}
}
void checkPath(char *str1)
{
	struct stat sb;
	if(stat(str1,&sb)==0 && S_ISDIR(sb.st_mode))
	{
		if((sb.st_mode & S_IWUSR)!=S_IWUSR)
		{
			printf("Invalid directory!!! It isn't writeable!! Exiting now...\n");
			exit(0);
		}
	}
	else
	{
		printf("No such path exists!!! Exiting now...\n");
		exit(0);
	}
}
int NormalizeURL(char* URL) 
{
  	int len=strlen(URL);
  	if (len<=1)
    		return 0;
  	if (URL[len-1]=='/') 
  	{
    		URL[len-1]=0;
    		len--;
  	}
  	int i,j;
  	len=strlen(URL);
  	if(len<2)
    		return 0;
  	for(i=len-1;i>=0;i--)
    		if(URL[i]=='.')
      			break;
  	for(j=len-1;j>=0;j--)
    		if(URL[j]=='/')
      			break;
	if(j==-1)
		return 0;
  	if((j>=7)&&(i>j)&&((i+3)>=len-1)) 
  	{
    		if((!strncmp((URL+i),".htm",4))||(!strncmp((URL+i),".HTM",4))||(!strncmp((URL+i),".php",4))||(!strncmp((URL+i),".jsp",4))||(!strncmp((URL+i),".in",3))) 
    		{
      			len=len;
    		}	 
    		else 
    		{
      			return 0;
    		}
  	}
  	return 1;
}
void insert_linked(char **a,int i,struct crawl *res,int co,struct Hash h[],int d)
{
	struct crawl *s,*temp;
	int key,k;
	if(co==1)
	{
		res->k=(char *)malloc((strlen(a[0])+1)*sizeof(char));
		strcpy(res->k,a[0]);
		res->level=co;
		res->done=d;
		res->prev=NULL;
		res->next=NULL;
	}
	for(int j=0;j<i&&co!=1;++j)
	{
		key=0;
		for(k=0;a[j][k];++k)
		{
			if(k%2)
				key=key+a[j][k];
			else
				key=key-a[j][k];
		}
		key=key/10;
		key=abs(key);
		temp=(struct crawl *)malloc(sizeof(struct crawl));
		temp->k=(char *)malloc((strlen(a[j])+1)*sizeof(char));
		strcpy(temp->k,a[j]);
		temp->level=co;
		temp->done=d;
		if(h[key].opt==0)
		{
			h[key].opt=1;
			h[key].start=temp;
			h[key].end=temp;
			s=res;
			while(s->next!=NULL)
			{
				s=s->next;
			}
			s->next=temp;
			temp->prev=s;
			temp->next=NULL;
		}
		else if(h[key].opt==1)
		{
			s=h[key].end;
			temp->prev=s;
			if(s->next!=NULL)
			{
				s=s->next;
				temp->next=s;
				s->prev=temp;
				s=s->prev->prev;
				s->next=temp;
				h[key].end=temp;
			}
			else
			{
				temp->next=NULL;
				s->next=temp;
				h[key].end=temp;
			}
		}
	}
}
void remove_nq(char *html,struct crawl *res,int co,struct Hash h[],struct crawl *fin)
{
	struct crawl *temp;
	struct crawl *s;
	int i=0,j=0,flag,key,k;
	char **a=(char **)malloc(100*sizeof(char *));
	char *token=strtok(html," ");
	for(;i<MAX&&token!=NULL;)
	{
		if(NormalizeURL(token))
		{
			flag=1;
			s=res;
			for(j=0;j<i&&flag!=0;++j)
			{
				if(strcmp(a[j],token)==0)
					flag=0;
			}
			if(flag==1)
			{
				key=0;
				for(k=0;token[k];++k)
				{
					if(k%2)
						key=key+token[k];
					else
						key=key-token[k];
				}
				key=key/10;
				key=abs(key);
				if(h[key].opt==1)
				{
					for(s=h[key].start;;s=s->next)
					{
						if(strcmp(s->k,token)==0)
							flag=0;
						if(s==h[key].end||flag==0)
							break;
					}
				}
			}
			if(flag==1)
			{
				a[i]=(char *)malloc((strlen(token)+1)*sizeof(char));
				strcpy(a[i],token);
				i++;
			}
		}
		token=strtok(NULL," ");
	}
	insert_linked(a,i,res,co,h,0);
	if(i>0)
		fin->done=1;
	free (a);
}
void extract_links(char *html)
{
	int i,j=0;
	for(i=0;i<strlen(html);++i)
	{
		if((i+3)<strlen(html)&&html[i]=='h'&&html[i+1]=='r'&&html[i+2]=='e'&&html[i+3]=='f')
		{
			++i;
			while(html[i]!='\"')
				++i;
			++i;
			while(html[i]!='\"')
			{
				html[j++]=html[i];
				i++;
			}
			html[j++]=' ';
		}
	}
	html[j]='\0';
}
void extract_anchor(char *html)
{
	int i,j=0,counter=0;
	for(i=0;html[i]!='\0';++i)
	{
		if(html[i]=='<'&&(html[i+1]=='A'||html[i+1]=='a'))
		{
			while(html[i]!='>')
			{
				html[j++]=html[i];
				i++;
			}
			html[j++]=html[i];
		}
	}
	html[j]='\0';
	extract_links(html);
}
void get_copy_data(char index_file[],char str1[])
{
	int i=0;
	FILE *fp;
	fp=fopen(str1,"r");
	while((index_file[i++]=fgetc(fp))!=EOF);
	fclose(fp);
}
long int get_file_size(char str1[])
{
	FILE *fp;
	fp=fopen(str1,"r");
	fseek(fp,0L,SEEK_END);
	long int res=ftell(fp);
	fclose(fp);
	return res;
}
void retreive(char str1[],char str2[])
{
	long int i=0;
	char start[200]="wget --convert-links ";
	strcat(start,str2);
	strcat(start," -O ");
	strcat(start,str1);
	system(start);
	FILE *fp;
	long int index=get_file_size(str1);
	char index_file[index+1];
	get_copy_data(index_file,str1);
	fp=fopen(str1,"w");
	fprintf(fp,"%s\n",str2);
	fprintf(fp,"%s",index_file);
	fclose(fp);
}
void get_file_data(char *html,char str1[],char str2[])
{
	int i=0;
	FILE *fp;
	fp=fopen(str1,"r");
	while((html[i++]=fgetc(fp))!=EOF)
	{
		if(html[i-1]==' ')
			--i;
	}
	fclose(fp);
}
int main(int argc,char *argv[])
{
	struct crawl *s=(struct crawl *)malloc(sizeof(struct crawl)),*temp;
	struct Hash h[250];
	char ch,**filearr,fname[15];
	FILE *fp;
	int i,j=1,depth,pdepth=0,counter=1,x,k,key,ext=1,lv,dn;
	for(i=0;i<250;++i)
	{
		h[i].start=NULL;
		h[i].end=NULL;
		h[i].opt=0;
	}
	for(i=0;argv[1][i];++i);
	char *str1=(char *)malloc((i+15)*sizeof(char));
	strcpy(str1,argv[1]);
	checkPath(str1);
	sscanf(argv[2],"%d",&depth);
	for(i=0;argv[3][i];++i);
	char *str2=(char *)malloc((i+1)*sizeof(char));
	strcpy(str2,argv[3]);
	validate(str2);
	fp=fopen("cont.txt","r");
	fseek(fp,0,SEEK_END);
	if(ftell(fp)!=0)
	{
		fseek(fp,0,SEEK_SET);
		fscanf(fp,"%d %d %d",&ext,&pdepth,&lol);
		filearr=(char **)malloc(1*sizeof(char *));
		*filearr=(char *)malloc(100*sizeof(char));
		while(!feof(fp))
		{
			fscanf(fp,"%s %d %d\n",filearr[0],&lv,&dn);
			if(dn==0&&ext>lv)
				ext=lv;
			if(ext==lv&&lv==pdepth)
			{
				dn=0;
				--lol;
			}
			insert_linked(filearr,1,s,lv,h,dn);
		}
		fclose(fp);
		goto x;
	}
	s->k=(char *)malloc((i+1)*sizeof(char));
	strcpy(s->k,str2);
	s->level=1;
	s->done=0;
	s->prev=NULL;
	s->next=NULL;
	for(i=0;s->k[i];++i)
	{
		if(i%2)
			key=key+s->k[i];
		else
			key=key-s->k[i];
	}
	key=key/10;
	key=abs(key);
	h[key].opt=1;
	h[key].start=s;
	h[key].end=s;
	x:
	for(i=ext;i<=depth;++i)
	{
		temp=s;
		while(temp!=NULL)
		{
			if(temp->level==i&&temp->done!=1&&temp->level<depth)
			{
				if((ch=getchar())=='e')
					break;
				if(temp->level!=1)
				{
					str2=(char *)malloc((strlen(temp->k)+1)*sizeof(char));
					strcpy(str2,temp->k);
				}
				strcpy(str1,argv[1]);
				++lol;
				sprintf(fname,"/index%d.txt",lol);
				strcat(str1,fname);
				retreive(str1,str2);	
				long int res=get_file_size(str1);
				++res;
				char *html=(char *)malloc(res*sizeof(char));
				get_file_data(html,str1,str2);
				extract_anchor(html);
				remove_nq(html,s,temp->level+1,h,temp);
			}
			else if(temp->level==depth&&temp->level==i&&temp->done==0)
			{
				if((ch=getchar())=='e')
					break;
				str2=(char *)malloc((strlen(temp->k)+1)*sizeof(char));
				strcpy(str2,temp->k);
				strcpy(str1,argv[1]);
				++lol;
				sprintf(fname,"/index%d.txt",lol);
				strcat(str1,fname);
				retreive(str1,str2);
				temp->done=1;
			}
			temp=temp->next;
		}
		if(ch=='e')
			break;
	}
	fp=fopen("cont.txt","w");
	if(i>depth)
		i=depth;
	fprintf(fp,"%d %d %d\n",i,depth,lol);
	while(s!=NULL)
	{
		fprintf(fp,"%s %d %d",s->k,s->level,s->done);
		s=s->next;
		if(s!=NULL)
			fprintf(fp,"\n");
	}
	fclose(fp);
	free(s);
	free(temp);
	free(str1);
	free(str2);
	free(filearr);
	return 0;
}

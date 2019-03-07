#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
//#include<iostream>
#define base_url "www.chitkara.edu.in"
//using namespace std;
void extract_anchor_tags(char *html)
{
	int i,j=0;
	for(i=0;html[i]!='\0';i++)
	{
		if(html[i]=='<'&&(html[i+1]=='a'||html[i+1]=='A'))
		{
			while(html[i]!='>')
			{
				html[j]=html[i];
				i++;
				j++;
			}
			html[j]=html[i];
			j++;
		}
	}
	html[j]='\0';
	printf("%s",html);
}
void validate_url(char *seed_url)
{
	printf("%s\n",seed_url);
	int i,j=0;
	for(i=0;base_url[i];i++)
	{
		if(base_url[i]!=seed_url[i])
		{
			j=1;
			break;
		}
		//if(seed_url[i]=='/')
	}
	if(j)
	{
		printf("Invalid Seed\n");
		exit(0);
	}
	else
		printf("Seed Verified\n");
}
void validate_target_dir(char *target_dir)
{
	struct stat statbuf;
	if(stat(target_dir,&statbuf)==-1)
	{
		fprintf(stderr,"----------\n");
		fprintf(stderr,"Invalid Directory\n");
		fprintf(stderr,"----------\n");
		exit(1);
	}
	if(!S_ISDIR(statbuf.st_mode))
	{
		fprintf(stderr,"-------------------------------\n");
		fprintf(stderr,"Invalid Directory entry.Your input isn't a directory\n");
		fprintf(stderr,"-------------------------------\n");
		exit(1);
	}
	if((statbuf.st_mode&S_IWUSR)!=S_IWUSR)
	{
		fprintf(stderr,"-------------------------------\n");
		fprintf(stderr,"Invalid directory entry.It isn't writable\n");
		fprintf(stderr,"-------------------------------\n");
	}
	printf("Directory Verified");
}
void downfile(char *target_dir,char *seed_url)
{
	static int page_counter=1;
	char wget[]="wget --convert-links -O index.txt -P ";
	char down_url[200],file_name[30];
	sprintf(file_name,"file%d.txt",page_counter);
	strcpy(down_url,wget);
	strcat(down_url,target_dir);
	strcat(down_url," ");
	strcat(down_url,seed_url);
	//char create_file[500]="cat index.txt | grep -Eo \"(http|https)://[a-zA-Z0-9./?=_-]*\" | grep -Ev '(*.jpg|*.css|*.js|*.png|*.php|*.gif|*.xml|*.ico|*noscript*|*.pdf)' | sort | uniq > ";
	//strcat(create_file,file_name);
	system(down_url);
	//system(create_file);
	//system("rm index.txt");
	//printf("%s",html);
}
long int get_index_size()
{
	FILE *fp;
	fp=fopen("index.txt","r");
	fseek(fp,0L,SEEK_END);
	long int html_size;
	html_size=ftell(fp);
	html_size++;
	return html_size;
	fclose(fp);
}
void Get_HTML_from_index(char *html)
{
	FILE *fp;
	fp=fopen("index.txt","r");
	long int i=0;
	while((html[i++]=fgetc(fp))!=EOF)
	{
		if(html[i-1]=='\n'||html[i-1]==' '||html[i-1]=='\t')
			i--;
	}
	html[i]='\0';
	fclose(fp);
	//printf("%s",html);
	//return html_size;
}
main(int argc,char *argv[])
{
	char seed_url[strlen(argv[1])+1],target_dir[strlen(argv[3])+1];
	int depth;
	strcpy(seed_url,argv[1]);
	strcpy(target_dir,argv[3]);
	sscanf(argv[2],"%d",&depth);
	printf("%d\n%s\n%s\n",depth,seed_url,target_dir);
	printf("%d\n%s\n%s\n",depth,argv[1],target_dir);
	validate_url(seed_url);
	validate_target_dir(target_dir);
	downfile(target_dir,seed_url);
	long int html_size=get_index_size();
	char *html=(char *)malloc(html_size*sizeof(char));;
	Get_HTML_from_index(html);
//	printf("%s",html);
	extract_anchor_tags(html);
	printf("\n");
}

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

int main(int argc, char* argv[])
{
    if(argc == 1)
    {
        DIR* dir = opendir("./");
        if(dir == NULL)
		{
			perror("Can not open directory\n");
			return -1;
		}
		struct dirent *dirent_v;
		while((dirent_v = readdir(dir)) != NULL)
		{
			if(dirent_v->d_name[0] != '.')
			{ 
				printf("%s\t",dirent_v->d_name);
			}
		}
		puts("");
        closedir(dir);
    }
    else if( argc == 2)
    {
		if(!strcmp(argv[1],"-a"))
		{
			DIR* dir = opendir("./");
			if(dir == NULL)
			{
				perror("Can not open directory\n");
				return -1;
			}
			struct dirent *dirent_v;
			while((dirent_v = readdir(dir)) != NULL)
			{
				printf("%s\t",dirent_v->d_name);
			}
			puts("\n");
            closedir(dir);	 
		}   
		else if(!strcmp(argv[1],"-l"))
		{
			DIR* dir = opendir("./");
			int fd = -1;

			if(dir == NULL)
			{
					perror("Can not open directory\n");
					return -1;
			}
			struct dirent *dirent_v;
			struct stat stat_v;
			while((dirent_v = readdir(dir)) != NULL)
			{
				if(dirent_v->d_name[0] != '.')
				{
					fd = open(dirent_v->d_name,O_RDONLY);
					if(fd < 0)
					{
						perror("Can not open file\n");
					}
					fstat(fd,&stat_v);
					printf("%d  ",stat_v.st_uid);
					printf("%d  ",stat_v.st_gid);
					printf("%d  ",stat_v.st_size);
					printf("%d  ",stat_v.st_mtim);
					printf("%s\n",dirent_v->d_name);

					close(fd);
				}
			}
				closedir(dir);
		}
		else if(!strcmp(argv[1],"-la"))
		{
			DIR* dir = opendir("./");
			int fd = -1;
			if(dir == NULL)
			{
					perror("Can not open directory\n");
					return -1;
			}
			struct dirent *dirent_v;
			struct stat stat_v;

			fd = open(dirent_v->d_name,O_RDONLY);
			
			if(fd < 0)
			{
				perror("Can not open file\n");
			}
			
			while((dirent_v = readdir(dir)) != NULL)
			{
				fd = open(dirent_v->d_name,O_RDONLY);
				if(fd < 0)
				{
					perror("Can not open file\n");
				}
				fstat(fd,&stat_v);
				printf("%d  ",stat_v.st_uid);
				printf("%d  ",stat_v.st_gid);
				printf("%d  ",stat_v.st_size);
				printf("%d  ",stat_v.st_mtim);
				printf("%s\n",dirent_v->d_name);

				close(fd);
			}
			closedir(dir);
		} 
    }
    return 0;
}

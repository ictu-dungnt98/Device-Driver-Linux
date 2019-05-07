#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

int main()
{
    DIR *dir = opendir("./");
    if(dir == NULL)
    {
        perror("Can not open directory");
        return -1;
    }
    struct dirent *dirent_v;
    struct stat stat_buf;
    dirent_v = readdir(dir);
    do
    {
        printf("%s\n",dirent_v->d_name);
        if(-1 == stat(dirent_v->d_name,&stat_buf))
	{
	    perror("Can not open file\n");
            return -1;
	}
        printf("size = %d\n",stat_buf.st_size);
        printf("modify time = %d\n",stat_buf.st_mtim);
    	puts("");
	dirent_v = readdir(dir);
    }while(dirent_v != NULL);
    return 0;
}

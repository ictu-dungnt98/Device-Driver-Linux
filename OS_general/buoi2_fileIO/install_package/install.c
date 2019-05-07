#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

int main(int argc, char* argv[])
{
    if(argc == 2)
    {
	    if(!strcmp(argv[1],"install"))
	    {
	        system("make -C ./shared_lib/ all");
	        int foo_fd = -1;
	        foo_fd = open("/etc/foo.conf", O_RDWR|O_CREAT|O_APPEND,S_IRWXU);
	        if(foo_fd < 0)
	        {
	            perror("can not create file /etc/foo.conf\n");
	            exit(-1);
	        }
	        close(foo_fd);
	        int libfoo_fd = -1;
	        libfoo_fd = open("/usr/bin/libfoo.so",O_RDWR|O_CREAT|O_APPEND,S_IRWXU);
	        if(libfoo_fd < 0)
	        {
	            perror("can not create file /usr/bin/libfoo.so\n");
	            exit(-1);
	        }
	        int gen_fd = -1;
	        gen_fd = open("/bin/gen",O_RDWR|O_CREAT|O_APPEND,S_IRWXU);
	        if(gen_fd < 0)
	        {
	            perror("can not create file /bin/gen\n");
	            exit(-1);
	        }

	        DIR* dir = opendir("./shared_lib/");
	        if(dir == NULL)
	        {
	            perror("Directory shared_lib not found\n");
		    return -1;
	        }
	        struct dirent *dir_var;
	        int fd = -1;
	        char file_name[256]= "";
	        char buff[1000] = "";
	        int len_read = 0;
	        int len_write = 0;
	        while((dir_var = readdir(dir)) != NULL)
	        {
		    memset(file_name,0,strlen(file_name));
		    strcpy(file_name,"./shared_lib/");
		    strcat(file_name,dir_var->d_name);

	            if(0 == strcmp(dir_var->d_name,"libfoo.so"))
		    {
		        fd = open(file_name,O_RDONLY);
	                if(fd < 0)
	                {
	                    perror("Can not open file libfoo.so\n");
	                    exit(-1);
	                }
	                while(0 != (len_read = read(fd,buff,sizeof(buff))))
		        {
	                    if(len_read < 0)
	                    {
	                        perror("can not read file\n");
	                        exit(-1);
		            }
			    len_write = write(libfoo_fd,buff,strlen(buff));
		            if(len_write < 0)
			    {
			        perror("Error occur when copy file file\n");
			        exit(-1);
			    }
		        }
	                close(fd);
		        close(libfoo_fd);
	            }
		    else if (0 == strcmp(dir_var->d_name,"gen"))
	            {
		        fd = open(file_name,O_RDONLY);
	                if(fd < 0)
	                {
	                    perror("Can not open file\n");
	                    exit(-1);
	                }
	                while(0 != (len_read = read(fd,buff,sizeof(buff))))
		        {
	                    if(len_read < 0)
	                    {
	                        perror("can not read file\n");
	                        exit(-1);
		            }
			    len_write = write(gen_fd,buff,strlen(buff));
		            if(len_write < 0)
			    {
			        perror("Error occur when copy file file\n");
			        exit(-1);
			    }
		        }
	                close(fd);
		        close(gen_fd);
	            }
		    else
		    {
		        continue;
		    }
	        }
	    }
	    else if(!strcmp(argv[1],"uninstall"))
	    {
	        if(-1 == remove("/etc/foo.conf"))
		{
	            perror("Not find /etc/foo.conf\n");
		}
	        if( -1 == remove("/usr/bin/libfoo.so"))
		{
	            perror("Not find /etc/foo.conf\n");
		}
	        if(-1 == remove("/bin/gen"))
		{
	            perror("Not find /etc/foo.conf\n");
		}
	    }
    }
    return 0;
}

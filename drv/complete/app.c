#include <stdio.h>
#include <fcntl.h>

char buf[20]="123456789";
char buf2[20]={0};
int main()
{
	int ret=0;
	int fd=0;
	fd=open("/dev/misc_complete",O_RDWR);
	if(fd<0)
		printf("open error\n");

	ret=write(fd,buf,sizeof(buf));
	if(ret<0)
		printf("write error\n");

	ret=read(fd,buf2,1);
	buf2[ret]='\0';
	if(ret<0)
		printf("read error\n");
	printf("buf2=%s\n",buf2);

	ret=lseek(fd,0,SEEK_CUR);
	printf("lseek,ret=%d\n",ret);
	ret=read(fd,buf2,1);
	buf2[ret]='\0';
	printf("read,ret=%d\n",ret);
	if(ret<0)
		printf("read error\n");
	printf("buf2=%s\n",buf2);

	ret=lseek(fd,0,SEEK_CUR);
	printf("lseek,ret=%d\n",ret);
	close(fd);
return 0;

}

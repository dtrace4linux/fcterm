# include <stdio.h>
# include <fcntl.h>

int main()
{	char	buf[256];
	int	i, ret;

	sprintf(buf, "ls -l /proc/%d/fd", getpid());
	system(buf);

	i = open("/dev/null", O_RDONLY); 
	ret = fcntl(i, F_SETFD, fcntl(i, F_GETFD, 0) | 1);
	printf("i=%d ret=%d\n", i, ret);
	if (fork() == 0)
		execl("/bin/bash", "/bin/bash", 0);
	sleep(100);
}

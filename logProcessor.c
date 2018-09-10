#include<stdio.h>
#include<stdarg.h>
#include <fcntl.h>

static int fd_log;
static int fd_error;

static int vfpf(char *fmt, ...)
{
	va_list argptr;
	int cnt;
	va_start(argptr, fmt);
	cnt = vfprintf(fp, fmt, argptr);
	va_end(argptr);
	return(cnt);
}

int log_record(char *fmt, ...)
{
	
}

int error_record(char *fmt, ..)
{

}

bool log_init(void)
{
	fd_log = open("a.txt", O_CREAT | O_TRUNC | O_RDWR, 0600);;
	if(fp < 0)
	{
		return false;		
	}
	return true;
	
}

int main(void)
{
	int inumber = 30;
	float fnumber = 90.0;
	char string[4] = "abc";
	if (fp < 0)
	{
		exit(1);
	}
	vfpf("%d %f %s", inumber, fnumber, string);
	rewind(fp);
	fscanf(fp,"%d %f %s", &inumber, &fnumber, string);
	printf("%d %f %s\n", inumber, fnumber, string);
	fclose(fp);
	return 0;
}

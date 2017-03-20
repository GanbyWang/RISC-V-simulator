#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "riscv.h"

void ecall()
{
	long long sys_num = regs.get_64_reg(17);
	long long arg_0 = regs.get_64_reg(10);
	long long arg_1 = regs.get_64_reg(11);
	long long arg_2 = regs.get_64_reg(12);
	long long arg_3 = regs.get_64_reg(13);

	int file;
	void *io_buf;
	int nbytes;
	struct tms *tms_buf;
	struct stat *fstat_buf;
	void *addr;
	struct timeval *tv;
	struct timezone *tz;
	off_t offset;
	int whence;

	if (sys_num == 63)		//read
	{			
		file = arg_0;
		io_buf = (void *)mem.get_address(arg_1);
		nbytes = arg_2;
		// printf("\n%d %llx %d\n", file, arg_1, nbytes);
		regs.write_32_reg( 10, read(file, io_buf, nbytes) );
		// printf("Read\r\n");
	}
	else if (sys_num == 64)		//write
	{
		file = arg_0;
		io_buf = (void *)mem.get_address(arg_1);
		nbytes = arg_2;
		regs.write_32_reg( 10, write(file, io_buf, nbytes) );
		// printf("Write\r\n");
	}
	/* else if (sys_num == 153)	//times
	{
		tms_buf = (struct tms *)mem.get_address(arg_0);
		regs.write_32_reg( 10, times(tms_buf) );
		// printf("Times\r\n");
	} */
	else if (sys_num == 93)		//exit
	{
		END = 1;
		// printf("End\r\n");
	}
	else if (sys_num == 80)		//fstat
	{
		/* file = arg_0;
		fstat_buf = (struct stat *)mem.get_address(arg_1);
		regs.write_32_reg( 10, fstat(file, fstat_buf) ); */
		// printf("Fstat %d\r\n",regs.get_64_reg(10));
	}
	else if (sys_num == 214)	//brk
	{
		// addr = (void *)mem.get_address(arg_0);
		// regs.write_32_reg( 10, brk(addr) );
		// regs.write_64_reg( 10, 0 );
		// printf("Brk: %llx\r\n", regs.get_64_reg(10));
	}
	else if (sys_num == 169)	//gettimeofday
	{
		tv = (struct timeval *)mem.get_address(arg_0);
		tz = (struct timezone *)mem.get_address(arg_1);
		regs.write_32_reg( 10, gettimeofday(tv, NULL) );
		// printf("Gettimeofday\r\n");
	}
	else if (sys_num == 62)		//lseek
	{
		file = arg_0;
		offset = (off_t)arg_1;
		whence = arg_2;
		regs.write_32_reg( 10, lseek(file, offset, whence) );
		// printf("Lseek\r\n");
	}
	else if (sys_num == 57)		//close
	{
		// file = arg_0;
		// regs.write_32_reg( 10, close(file) );
		// printf("Close\r\n"); 
	}
	else
	{
		printf("Undefined ecall %lld\r\n", sys_num);
		exit(0);
	}
}

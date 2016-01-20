#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 

#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 

#include <sys/mman.h> 

// TODO: define the led control register
//#define GPIO_BASE_ADDR		0xE0300000	//-- GPIO BASE ADDRESS
#define GPIO_BASE_ADDR		0x56000000	//-- GPIO BASE ADDRESS
#define GPIO_SIZE		    0x1000		//-- ���� �Ҵ� �޸��� Size
//#define OFFSET_GPGCON		0x280
#define OFFSET_GPGCON		0x60
//#define OFFSET_GPGDAT		0x284
#define OFFSET_GPGDAT		0x64


void led_test (void *addr)
{
	int i=0; 
	
	unsigned int *pgpgcon;
	unsigned int *pgpgdat;
	
	pgpgcon = (unsigned int *)(addr + OFFSET_GPGCON);
	pgpgdat = (unsigned int *)(addr + OFFSET_GPGDAT);
	
	//*pgpgcon = ((*pgpgcon & ~(0xffff<<0))| (0x1111<<0)); // outmode setting
	*pgpgcon = ((*pgpgcon & ~(0xff<<8))| (0x55<<8)); // outmode setting

	for( i=0; i<10; i++)
	{
		*pgpgdat = (*pgpgdat|(0xf<<4)); // led off
		sleep(1);
		*pgpgdat = (*pgpgdat & ~(0xf<<4)); // led on
		sleep(1);
	}
}

int main(void)
{ 
	int fd;
	void *addr;

	fd = open( "/dev/mem", O_RDWR|O_SYNC ); 

	if( fd < 0 ) 
	{ 
		perror( "/dev/mem open error" ); 
		exit(1); 
	} 

	// TODO: mmap�� ����� ���� �ּҸ� addr�� �Ҵ� �Ѵ�. 
	// ��� ���μ������� ���� �� �� �ֵ��� �ϸ�, �б�/���� �����ϰ� �Ѵ�.
	// ���� �ּҴ� GPIO_BASE_ADDR�� �����Ѵ�.`
	addr = mmap(0, //-- auto allocation(user addr)
			GPIO_SIZE, //--size
			PROT_READ|PROT_WRITE, //-- read, write
			MAP_SHARED, // --other process
			fd, //--file dscriptor
			GPIO_BASE_ADDR); //--

	if( addr == NULL )
	{
		printf("mmap fail ==> exit()");
		exit(1);
	}

	printf( "fd value %d\n", fd );
	printf( "IO ADDR %p\n", addr ); //-- ���� ��(Virtual) Base Address
	
	led_test( addr );
	
	munmap(addr, GPIO_SIZE ); // ���εȰ� ���� 
	close (fd);
	

	return 0; 
}


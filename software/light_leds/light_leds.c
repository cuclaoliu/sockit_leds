#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>

#define soc_cv_av

#include "hwlib.h"
#include "soc_cv_av/socal/socal.h"
#include "soc_cv_av/socal/hps.h"
#include "soc_cv_av/socal/alt_gpio.h"

#define HW_LWFPGA_BASE ( ALT_LWFPGASLVS_OFST )
#define HW_LEDS_OFST ( 0x10 )
#define HW_REGS_SPAN ( 0x01000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )


int main(int argc, char **argv) {

	void *virtual_base;
	int fd;
	unsigned char value = 0, *param;
	
	if (argc < 2) {
		printf("Usage: %s number\n", argv[0]);
		return( EXIT_FAILURE );
	}
	
	param = (unsigned char *)argv[1];
	while ((*param)!='\0'){
		if (*param >= '0' && *param <= '9'){
			value *= 10;
			value += (*param) - '0';
		} 
		else{
			printf("Invalid parameter. Must be between 0 and 15, inclusive.\n");
			return( EXIT_FAILURE );
		}
		param++;
	};
	
	value %= 16;
	
	// map the address space for the LED registers into user space so we can interact with them.
	// we'll actually map in the entire CSR span of the HPS since we want to access various registers within that span
	if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( EXIT_FAILURE );
	}

	virtual_base = mmap( NULL, HW_REGS_SPAN, PROT_WRITE , MAP_SHARED, fd, HW_LWFPGA_BASE );

	if( virtual_base == MAP_FAILED ) {
		printf( "ERROR: mmap() failed...\n" );
		close( fd );
		return( EXIT_FAILURE );
	}

	printf("leds demo with number %d\n", value);
			
	// set led
	alt_write_word( ( virtual_base + ( ( uint32_t )( HW_LEDS_OFST ) & ( uint32_t )( HW_REGS_MASK ) ) ), value );

	// clean up our memory mapping and exit
	if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( EXIT_FAILURE );
	}

	close( fd );

	return( EXIT_SUCCESS );
}

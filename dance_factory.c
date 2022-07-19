#include <kernel.h>
#include <stdio.h>
#include <debug.h>
#include <graph.h>
#include <libcdvd.h>
#include <sifrpc.h>
#include <iopcontrol.h>
#include <loadfile.h>
#include <sbv_patches.h>

#define double_printf(...) printf(__VA_ARGS__); scr_printf(__VA_ARGS__);

extern unsigned int size_bdm_irx;
extern unsigned char bdm_irx[];

extern unsigned int size_bdmfs_vfat_irx;
extern unsigned char bdmfs_vfat_irx[];

extern unsigned int size_usbmass_bd_irx;
extern unsigned char usbmass_bd_irx[];

extern unsigned int size_usbd_irx;
extern unsigned char usbd_irx[];

void init_usb()
{
	sbv_patch_enable_lmb();

	int usbd_irx_id = SifExecModuleBuffer(&usbd_irx, size_usbd_irx, 0, NULL, NULL);
	double_printf("USBD ID is %d\n", usbd_irx_id);

	int bdm_irx_id = SifExecModuleBuffer(&bdm_irx, size_bdm_irx, 0, NULL, NULL);
	double_printf("BDM ID: %d\n", bdm_irx_id);

	int bdmfs_vfat_irx_id = SifExecModuleBuffer(&bdmfs_vfat_irx, size_bdmfs_vfat_irx, 0, NULL, NULL);
	double_printf("BDMFS VFAT ID: %d\n", bdmfs_vfat_irx_id);

	int usbmass_irx_id = SifExecModuleBuffer(&usbmass_bd_irx, size_usbmass_bd_irx, 0, NULL, NULL);
	double_printf("USB Mass ID is %d\n", usbmass_irx_id);
}

int main()
{
	printf("Resetting IOP\n");
	SifInitRpc(0);
	// Reset IOP
	while (!SifIopReset("", 0x0))
		;
	while (!SifIopSync())
		;
	SifInitRpc(0);
	printf("Finished resetting IOP\n");

	init_usb();

	// Wait, because the USB driver is slow
	nopdelay();
	nopdelay();
	nopdelay();

	init_scr();

	graph_wait_vsync(); // Waiting for a vsync so the hardware renderer works
	 					// without preload frame


	double_printf("Hello, world!\n");

	sceCdInit(0);
	double_printf("Waiting for disc to become ready\n");

	while(sceCdDiskReady(0) == SCECdNotReady)
	{
		printf("Waiting...\n");
	}

	double_printf("Disc is ready, seeking to sector 38158\n");

	if(sceCdSeek(38158))
	{
		double_printf("Seeked to sector 38158\n");
	}

	u32 buff = 0;
	u32 status = 0;

	double_printf("Going to read SUBQ from sector 38158\n");
	sceCdReadSUBQ(&buff, &status);
	double_printf("Waiting for ReadSUBQ command\n");
	sceCdSync(0);
	double_printf("Finished!\n");
	double_printf("SUBQ is 0x%08x\nStatus is %x", buff, status);

	u8 toc[1024];
	sceCdGetToc(toc);

	sceCdSync(0);
	FlushCache(0);

	FILE* f = fopen("mass:TOC.bin", "wb");
	fwrite(toc, 1, 1024, f);
	FlushCache(0);
	fclose(f);
	
	double_printf("Finished\n");
	SleepThread();
}

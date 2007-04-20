#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include "ftd2xx.h"
#include "FTChipID.h"
#include "FTDI_HD44780.h"


int main(int argc, char* argv[])
{
	char Version[100];
	unsigned long NumDevices = 0, LocID = 0, ChipID = 0;
	char SerialNumber[256], Description[256], ErrorMessage[256];
	FTID_STATUS dStatus;
	FT_HANDLE ftHandle;
	FT_STATUS ftStatus;
	DWORD iNumDevs;
	unsigned char init_data[9] = {0x02, 0x02, 0x08, 0x00, 0x0E, 0x00, 0x06, 0x24, 0x21};
	
	FTID_GetDllVersion(Version, 100);

	printf("Dll Version = %s\n\n", Version);
	
	dStatus = FTID_GetNumDevices(&NumDevices);

	if((dStatus == FTID_SUCCESS) && NumDevices) {

		printf("Number of 232R devices = %ld\n\n", NumDevices);

		for(int i = 0; i < (int)NumDevices; i++) {

			printf("Device %d\n", i);

			dStatus = FTID_GetDeviceSerialNumber(i, SerialNumber, 256);
			if(dStatus == FTID_SUCCESS) {
				printf("\tSerial Number: %s\n", SerialNumber);
			}

			dStatus = FTID_GetDeviceDescription(i, Description, 256);
			if(dStatus == FTID_SUCCESS) {
				printf("\tDescription: %s\n", Description);
			}

			dStatus = FTID_GetDeviceLocationID(i, &LocID);
			if(dStatus == FTID_SUCCESS) {
				printf("\tLocation ID: 0x%08lX\n", LocID);
			}

			dStatus = FTID_GetDeviceChipID(i, &ChipID);
			if(dStatus == FTID_SUCCESS) {
				printf("\tChip ID: 0x%08lX\n", ChipID);
			}
			printf("\n");
		}
	}

	if(dStatus != FTID_SUCCESS) {
		FTID_GetErrorCodeString("EN", dStatus, ErrorMessage, 256);
		printf(ErrorMessage);
	}

	printf("Get Chip IDs using handle\n");

	FT_DEVICE_LIST_INFO_NODE *devInfo;

	//
	// create the device information list
	//
	ftStatus = FT_CreateDeviceInfoList(&iNumDevs);
	if (ftStatus == FT_OK) {
	   printf("%d FTDI devices connected\n",iNumDevs);
	}

	//
	// allocate storage for list based on numDevs
	//
	devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*iNumDevs);

	//
	// get the device information list
	//
	ftStatus = FT_GetDeviceInfoList(devInfo, &iNumDevs);
	if (ftStatus == FT_OK) {
		for (unsigned int i = 0; i < iNumDevs; i++) { 
			printf("Device type: %d (232R = %d)\n", devInfo[i].Type, FT_DEVICE_232R);
			if(devInfo[i].Type == FT_DEVICE_232R) {
				FT_Open(i, &ftHandle);
				dStatus = FTID_GetChipIDFromHandle(ftHandle, &ChipID);
				if(dStatus == FTID_SUCCESS) {
					printf("\tChip ID: 0x%08lX\n", ChipID);
				}
				
				ftStatus = FT_SetBaudRate(ftHandle, 9600);
				if (ftStatus == FT_OK) {
					printf("Baudrate set to 9600\n");
				}
				else {
					printf("FT_SetBaudRate failed\n");
				}

				ftStatus = FT_SetBitMode(ftHandle, 0xFF, 0x01); // All outputs, async bit-bang
				if (ftStatus == FT_OK) {
					printf("Bit mode set to async\n");
				}
				else {
					printf("FT_SetBitMode failed\n");
				}

				ftdi_hd44780 *display = new ftdi_hd44780(ftHandle);

				display->outCMD( (Set_Function | Data_Length_4) >> 4 );
				display->outCMD( Set_Function | Data_Length_4 | Two_Display_Lines | Font_5x7);
				display->outCMD( Set_Display | Display_On | Cursor_On | Blink_On );
				display->outCMD( Set_Entry_Mode | Increment_Address | Shift_Display_Off );
				display->outCMD( Clear_Display );
				display->outCMD( Return_Home );
				display->outString( "Hello World!" );

				ftStatus = FT_SetBitMode(ftHandle, 0xFF, 0x00); // Reset bit mode
				if (ftStatus == FT_OK) {
					printf("Bit mode reset\n");
				}
				else {
					printf("FT_SetBitMode failed\n");
				}

				FT_Close(ftHandle);
			}
		}
	} 	

	//printf("Press return to exit\n");
	//getchar();

	return 0;
}

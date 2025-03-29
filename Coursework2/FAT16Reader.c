#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include<stdbool.h>
#include <string.h>

typedef struct __attribute__((__packed__)) {
   uint8_t BS_jmpBoot[ 3 ]; // x86 jump instr. to boot code
   uint8_t BS_OEMName[ 8 ]; // What created the filesystem
   uint16_t BPB_BytsPerSec; // Bytes per Sector   
   uint8_t BPB_SecPerClus; // Sectors per Cluster
   uint16_t BPB_RsvdSecCnt; // Reserved Sector Count
   uint8_t BPB_NumFATs; // Number of copies of FAT
   uint16_t BPB_RootEntCnt; // FAT12/FAT16: size of root DIR
   uint16_t BPB_TotSec16; // Sectors, may be 0, see below
   uint8_t BPB_Media; // Media type, e.g. fixed
   uint16_t BPB_FATSz16; // Sectors in FAT (FAT12 or FAT16)
   uint16_t BPB_SecPerTrk; // Sectors per Track
   uint16_t BPB_NumHeads; // Number of heads in disk
   uint32_t BPB_HiddSec; // Hidden Sector count
   uint32_t BPB_TotSec32; // Sectors if BPB_TotSec16 == 0
   uint8_t BS_DrvNum; // 0 = floppy, 0x80 = hard disk
   uint8_t BS_Reserved1; //
   uint8_t BS_BootSig; // Should = 0x29
   uint32_t BS_VolID; // 'Unique' ID for volume
   uint8_t BS_VolLab[ 11 ]; // Non zero terminated string
   uint8_t BS_FilSysType[ 8 ]; // e.g. 'FAT16 ' (Not 0 term.)
} BootSector;

typedef struct __attribute__((__packed__))
{
    uint8_t	DIR_Name[ 11 ];		// Non zero terminated string
	uint8_t	DIR_Attr;		// File attributes
	uint8_t	DIR_NTRes;		// Used by Windows NT, ignore
	uint8_t	DIR_CrtTimeTenth;	// Tenths of sec. 0..199
	uint16_t	DIR_CrtTime;		// Creation Time in 2s intervals
	uint16_t	DIR_CrtDate;
	uint16_t	DIR_LstAccDate;		// Date of last read or write
	uint16_t	DIR_FstClusHI;		// Top bits file's 1st cluster
	uint16_t	DIR_WrtTime;		// Time of last write
	uint16_t	DIR_WrtDate;		// Date of last write
	uint16_t	DIR_FstClusLO;		// Lower bits file's 1st cluster
	uint32_t	DIR_FileSize;		// File size in bytes
}NormalFile;

typedef struct __attribute__((__packed__))
{
    uint8_t		LDIR_Ord;		// Order/ position in sequence
	uint8_t		LDIR_Name1[ 10 ];	// First 5 UNICODE characters
	uint8_t		LDIR_Attr;		// = ATTR_LONG_NAME
	uint8_t		LDIR_Type;		// Should = 0
	uint8_t		LDIR_Chksum;		// Checksum of short name
	uint8_t		LDIR_Name2[ 12 ];	// Middle 6 UNICODE characters
	uint16_t	LDIR_FstClusLO;		// MUST be zero
	uint8_t		LDIR_Name3[ 4 ];	// Last 2 UNICODE characters
}LongFile;

// this function takes in file descriptor, the number of directories in the curret Folder, where to start reading, the size of a cluster
// the number of bytes to reach the end of Root Directory, the size of the FAT, The FAT file, and the boolean if you want to read the files or not
// The function creates an array of the struct NormalFile called nFile to the size of number of directories in the curret Folder. Then it will seek to the number in startread + (sizeof(NormalFile)*i)
// After it will read sizeof(NormalFile) number of bytes into the Array nfile. Then it will read the Attributes of nFile. If the first letter of the name of nFile is the null character it will return and leave the function
// If the first or first and second character is '.' it will ignore that RootDirectory. If nFile has a attribute of 15 it will ignore that nFile. If nFile is not 15 it will convert the dates and times to there proper values.
// If readData is false it will print all the variables in the struct NormalFile. If the directory bit and volumeName bit is equal to zero it is a txt file.  
// Then it will allocate memory based on how big the fileSize is to a variable called nFileRead. Then it will jump to it (FAT entry -2 *clusterSize)+ the end of the RootDirectory read the data then it will go to the next FAT entry
// until the FAT entry is equal to 0xFFF8. Then it will free nFileRead. If the Directory bit is set and volumeName bit is 0 it is a Folder. If it is a Folder it will recursively call the function
// with the new values for numDIR which is now equal to  clusterSize/sizeof(NormalFile), the new value for startRead is ((tempClusNumber-2)*clusterSize) where tempClusNumber is the FAT entry for the Folder until the FAT entries
// of the Folder is equal to 0xFFF8
//
void directoryRead(int f, int numDIR, int startRead, int clusterSize, int directoryEnd,int FATSize ,uint16_t FAT[FATSize],bool readData)
{
    NormalFile nFile[numDIR];
    for(int i=0;i<numDIR;i++)
    {
        lseek(f,startRead+(sizeof(NormalFile)*(i)),SEEK_SET);
        read(f,&nFile[i],sizeof(NormalFile));
        int archive = (nFile[i].DIR_Attr>>5)&1;
        int directory = (nFile[i].DIR_Attr>>4)&1;
        int volumeName = (nFile[i].DIR_Attr>>3)&1;
        int system = (nFile[i].DIR_Attr>>2)&1;
        int hidden = (nFile[i].DIR_Attr>>1)&1;
        int readOnly = nFile[i].DIR_Attr&1;
        if(nFile[i].DIR_Name[0] == '\0')
        {
            return;
        }
        else if(nFile[i].DIR_Name[0]== '.'&&nFile[i].DIR_Name[1]=='.')
        {
        }
        else if(nFile[i].DIR_Name[0]== '.'&&nFile[i].DIR_Name[1]==' ')
        {
        }
        else if(nFile[i].DIR_Attr == 15)
        {
            //readLongFile(f,ret,startRead,i);
        }
        else if(nFile[i].DIR_Attr != 15)
        {
            int Years = 1980 + ((nFile[i].DIR_WrtDate)>>9&0x7F);
            int Months = (nFile[i].DIR_WrtDate)>>5&0x00f;
            int Days = nFile[i].DIR_WrtDate&0x001f;
            int Hours = (nFile[i].DIR_WrtTime)>>11&0x1F;
            int Minutes = (nFile[i].DIR_WrtTime)>>5&0x03F;
            int Seconds = (nFile[i].DIR_WrtTime&0x001f)*2;
            int tempClusNumber = nFile[i].DIR_FstClusLO;
            int fileSize = nFile[i].DIR_FileSize;
            char* nFileRead;
            char Name[11];
            for(int j =0;j<11;j++)
            {
                if(nFile[i].DIR_Name[j] != '\0')
                {
                    Name[j] = nFile[i].DIR_Name[j];
                }
            }
            if(readData == false)
            {
                printf("| %-14s | %d%d%d%d%d%d | %-13d | %-13d | %-2d/%-2d/%-2d      | %-2d/%-2d/%-2d    |\n",Name,archive,directory,volumeName,system,hidden,readOnly,fileSize,tempClusNumber,Seconds,Minutes,Hours,Days,Months,Years);
            }
            if(directory == 0 && volumeName == 0)
            {
                nFileRead = (char*)malloc(sizeof(char)*(fileSize));
                for(int d=0;tempClusNumber<0xFFF8;d++)
                {
                    lseek(f, directoryEnd + ((tempClusNumber-2)*clusterSize), SEEK_SET);
                    if(readData == true)
                    {
                        read(f,nFileRead,fileSize);
                        printf("%s",nFileRead);
                    }
                    tempClusNumber = FAT[tempClusNumber];
                }
                free(nFileRead);
            }
            if(directory == 1 && volumeName == 0)
            {
                for(int d=0;tempClusNumber<0xFFF8;d++)
                {
                    directoryRead(f, clusterSize/sizeof(NormalFile),directoryEnd + ((tempClusNumber-2)*clusterSize),clusterSize,directoryEnd,FATSize,FAT,readData);
                    tempClusNumber = FAT[tempClusNumber];
                }
            }
            if(directory == 0 && volumeName == 0)
            {
                
            }
        }
    }
}

int main()
{
    int FAT16End;
    int directoryEnd;
    int reservedSectorEnd;
    int FATSize;
    int clusterSize;
    int rootDirectorySize;
    BootSector *sector = (BootSector*)malloc(sizeof(BootSector)); // allocate sector memory equal to the size of BootSector
    int f = open("fat16.img", O_RDONLY); // opens file "fat16.img" in ReadOnly mode and stores the file descriptor in f
    read(f,sector,sizeof(BootSector));  // reads sizeof(BootSector) amount of bytes stores it sector from the file descriptor
    reservedSectorEnd = (sector->BPB_BytsPerSec)*(sector->BPB_RsvdSecCnt); // this variable stores the size of ReservedSectors
    FAT16End = reservedSectorEnd + (sector->BPB_FATSz16)*(sector->BPB_BytsPerSec)*(sector->BPB_NumFATs); // this variable stores the number of bytes required to jump to get to the End of the FAT cluster table from the start of the FAT file
    directoryEnd = FAT16End + sizeof(NormalFile)*(sector->BPB_RootEntCnt); // this variable stores the number of bytes required to jump to get to the end of the RootDirectory from the start of the FAT file
    lseek(f,reservedSectorEnd,SEEK_SET); // jumps to the end of the ReservedSector
    FATSize = (sector->BPB_BytsPerSec)*(sector->BPB_BytsPerSec/2); // calculates the number of entries in the FAT array
    uint16_t FAT[FATSize]; // creates an Array of uint16_t with the number of entries of FATSIZE
    for(int i=0; i<FATSize;i++)
    {
        read(f,&FAT[i],sizeof(uint16_t)); // reads the number of bytes which is the sizeof(uint16_t) into the Array
    }
    lseek(f,FAT16End,SEEK_SET); // jumps to the end of THE FAT entries region 
    clusterSize = (sector->BPB_BytsPerSec)*(sector->BPB_SecPerClus); // this calculates the size of a cluster in bytes
    rootDirectorySize = sector->BPB_RootEntCnt; // this is the total number of Root Directories in the FILE
    printf("-------------------------------------------------------------------------------------------\n");
    printf("|                                   |SCC211 FAT16|                                        |\n");
    printf("-------------------------------------------------------------------------------------------\n");
    printf("|      NAME      | ADVSHR |  FILE_LENGTH  | STARTING_CLUS | LAST_MOD_TIME | LAST_MOD_DATE |\n");
    printf("-------------------------------------------------------------------------------------------\n");
    directoryRead(f,rootDirectorySize,FAT16End,clusterSize,directoryEnd,FATSize,FAT,false);
    printf("-------------------------------------------------------------------------------------------\n");
    directoryRead(f,rootDirectorySize,FAT16End,clusterSize,directoryEnd,FATSize,FAT,true);
    free(sector);
}
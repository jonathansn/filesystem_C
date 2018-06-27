#include "filesystem.h"

/*
----------------------------------------------
ADDRESS OF BLOCKS
----------------------------------------------
    METADATA    (size 89)
        size:   0x00  - 0xb3 (4)
    FREE SPACE
        size:   0xb4  - 0x275 (10)
    INODE
        size:   0xb4  - 0x10d ()
    DATA
        size:   0x10e - 0x167
---------------------------------------------- 
BLOCK INFORMATIONS
----------------------------------------------
    METADATA
        - blocks position (size 6)
            free_space: 0X00 - 0x05
            inode:      0x06 - 0x0A
            data:       0x0B - 0x0F

        - disk information (size 5)
            size:       0x2d - 0x31
            
        - block information
            size:       0x5a
    FREE SPACE
        - 
    INODE
        file(id_inode,)
    DATA
----------------------------------------------
    disk_size:               0x19000  -    10MB
    block_size:              0xFF     -    90B
    file_name_size:          max 10
    folder_name_size:        max 10
    max_file_folders:        max 10
    max_blocks_file_folders: max 10
----------------------------------------------
*/
int main(int argc, char *argv[]){
    if(argc == 3){
        disk = argv[2];
        size = argv[1];
        char *cmd;
        strcpy(cmd, "truncate -s ");
        strcat(cmd, argv[1]);
        strcat(cmd, " ");
        strcat(cmd, argv[2]);
        system(cmd);
   }
   else if(argc == 0){
        disk = DISK_NAME;
        size = DISK_SIZE;
        char *cmd;
        strcpy(cmd, "truncate -s ");
        strcat(cmd, DISK_SIZE);
        strcat(cmd, " ");
        strcat(cmd, DISK_NAME);
        system(cmd);
   } else{
      printf("./filesystem [disk size] [disk name]\n");
   }

    printf("Disk %s created successfully!\n", argv[2]);

    
    createFile(disk,"teste.txt");
       
}

void setPosition(char *disk){
    FILE *fp;
    fp = fopen(disk, "rb+");

    if(fp != NULL){
        fseek(fp, 0, SEEK_SET);
        fclose(fp);
    } else {
        printf("Error while trying to open file!\n");
    }
}

void createFile(char *disk, char *file){
    FILE *fp;
    fp = fopen(disk, "rb+");

    if(fp != NULL){
        fseek(fp, 5, SEEK_SET);
        fputs(file, fp);
        fclose(fp);
    } else {
        printf("Error while trying to create file!\n");
    }
}
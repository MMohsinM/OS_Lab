#include "simplefs-ops.h"
extern struct filehandle_t file_handle_array[MAX_OPEN_FILES]; // Array for storing opened files
#define FILE_NOT_EXISTS -1

int if_name_exists(char *filename){
    inode_num = -1;
    int i=0;
    struct inode_t my_inode;
    for (i = 0;i < NUM_INODES; i++){
        simplefs_readInode(i, &my_inode);
        if (my_inode.status == INODE_IN_USE){
            if (my_inode.name == filename) {
                inode_num = i;
                break
            }
        }
    }
    return inode_num;
}

int simplefs_create(char *filename){
    /*
	    Create file with name `filename` from disk
	*/
    //  Check if name exists
    if (if_name_exists(filename) != FILE_NOT_EXISTS)
        return -1;

    //CHeck if disk space free
    int datablock_num = simplefs_allocDataBlock();
    if (datablock_num == -1)
        return -1;
    // CHECK if i node is free
    int  inode_num = simplefs_allocInode();
    if (inode_num == -1){
        return -1;
    }
    struct inode_t my_inode;
    simplefs_readInode(inode_num, my_inode);
    my_inode.name = filename;
    /*See empty i node*/
    /* Allocate the file blocks*/
    return inode_num;
}


void simplefs_delete(char *filename){
    /*
	    delete file with name `filename` from disk
	*/
    int inode_num = if_name_exists(filename);
    if (inode_num != FILE_NOT_EXISTS){
        //Free datablock
        struct inode_t my_inode;
        simplefs_readInode(inode_num, &my_inode);
        int data_block_index;
        for (data_block_index=0; data_block_index< MAX_FILE_SIZE; data_block_index++)
        {
            if (my_inode.direct_blocks[data_block_index] != DATA_BLOCK_FREE)
                simplefs_freeDataBlock(my_inode.direct_blocks[data_block_index])
        }
        simplefs_freeInode(inode_num)
    }
}

int simplefs_open(char *filename){
    /*
	    open file with name `filename`
	*/
    int inode_num = if_name_exists(filename);
    int file_handler_index = -1;
    if (inode_num != FILE_NOT_EXISTS){
           for (int i=0;i<MAX_OPEN_FILES;i++){
               if (file_handle_array[i].inode_number == -1)
               {
                   file_handler_index = i;
                   file_handle_array[i].inode_number = inode_num;
                   break
               }
           }
    }
    return file_handler_index;
}

void simplefs_close(int file_handle){
    /*
	    close file pointed by `file_handle`
	*/
    file_handle_array[file_handle].inode_number = -1;
    file_handle_array[file_handle].offset = 0;

}

int simplefs_read(int file_handle, char *buf, int nbytes){
    /*
	    read `nbytes` of data into `buf` from file pointed by `file_handle` starting at current offset
	*/
    int current_offset = file_handle_array[file_handle].offset;
    if (file_handle_array[file_handle].offset + nbytes < MAX_FILE_SIZE * BLOCKSIZE)
        return -1;
    file_handle_array[file_handle].offset = current_offset + nbytes;
    file_handle_array[file_handle]
    return -1;
}


int simplefs_write(int file_handle, char *buf, int nbytes){
    /*
	    write `nbytes` of data from `buf` to file pointed by `file_handle` starting at current offset
	*/
    return -1;
}


int simplefs_seek(int file_handle, int nseek){
    /*
	   increase `file_handle` offset by `nseek`
	*/
    if (file_handle_array[file_handle].offset + nseek < MAX_FILE_SIZE * BLOCKSIZE)
        return -1;
    file_handle_array[file_handle].offset = file_handle_array[file_handle].offset + nseek;
}
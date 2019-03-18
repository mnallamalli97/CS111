//NAME: Mehar Nallamalli, Aditi Mittal
//EMAIL: mnallamalli97.ucla.edu, aditi.mittal2@gmail.com
//ID: 804769644, 104736807
//SLIPDAYS: 1

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/fs.h>
#include <inttypes.h>
#include "ext2_fs.h"
#include <time.h>

static unsigned int sb_size;
#define BASE_OFFSET    1024            // beginning of super block
#define BLOCK_OFFSET(block) (BASE_OFFSET + (block-1) * sb_size)

int filename = -1;
struct ext2_super_block superblock;
//unsigned int sb_size;
struct ext2_group_desc group;
unsigned char bitmap;
off_t offset = 1024;
struct ext2_inode *inode;
struct ext2_dir_entry entry;

void superblock_function()
{
    sb_size = 1024 << superblock.s_log_block_size;
    //reading superblock
    int p = pread(filename, &superblock, sb_size, offset);
    if (p < 0)
    {
        fprintf(stderr, "Error: could not read superblock\n");
        exit(1);
    }
    
    //PRINTING TO STDOUT
    fprintf(stdout, "SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", superblock.s_blocks_count, superblock.s_inodes_count, sb_size, superblock.s_inode_size, superblock.s_blocks_per_group, superblock.s_inodes_per_group, superblock.s_first_ino);
}

void group_function()
{
    pread(filename, &group, sizeof(group), 1024 + sb_size);
    
    int blocks_per_group = superblock.s_blocks_count % superblock.s_blocks_per_group;
    int inodes_per_group = superblock.s_inodes_per_group;
    
    //PRINTING TO STDOUT
    fprintf(stdout, "GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n", 0, blocks_per_group, inodes_per_group, group.bg_free_blocks_count, group.bg_free_inodes_count, group.bg_block_bitmap, group.bg_inode_bitmap, group.bg_inode_table);
    
    //free blocks
    free_blocks();
    
    //free inodes
    free_inodes();
}

void free_blocks()
{
    for (int i = 0; i < sb_size; i++)
    {
        pread(filename, &bitmap, 1, (group.bg_block_bitmap*sb_size + i));
        for (int j = 0; j < 8; j++)
        {
            int n_freeblocks = (8*i) + (1+j);
            if ((bitmap & (1 << j)) == 0)
                fprintf(stdout, "BFREE,%d\n", n_freeblocks);
        }
    }
}

void free_inodes()
{
    for (int i = 0; i < sb_size; i++)
    {
        pread(filename, &bitmap, 1, (group.bg_inode_bitmap*sb_size + i));
        for (int j = 0; j < 8; j++)
        {
            int n_freeinodes = (8*i) + (1+j);
            if ((bitmap & (1 << j)) == 0)
                fprintf(stdout, "IFREE,%d\n", n_freeinodes);
        }
    }
}

void inode_function()
{
    unsigned char temp[sb_size];
    for (int i = 1; i < superblock.s_inodes_count; i++)
    {
        //fprintf(stdout, "%d\n", i);
        pread(filename, temp, sizeof(struct ext2_inode), BLOCK_OFFSET(group.bg_inode_table)+(i-1)*sizeof(struct ext2_inode));
        inode = (struct ext2_inode *) temp;
        if (inode->i_mode != 0 && inode->i_links_count != 0)
        {
            //file type
            char ftype;
            if (S_ISREG( inode->i_mode))
                ftype = 'f';
            else if (S_ISDIR( inode->i_mode))
                ftype = 'd';
            else if (S_ISLNK( inode->i_mode))
                ftype = 's';
            else
                ftype = '?';
            
            //first 7 stdout
            fprintf(stdout, "INODE,%d,%c,%o,%d,%d,%d", i, ftype, ( inode->i_mode&0xFFF),  inode->i_uid,  inode->i_gid,  inode->i_links_count);
            
            //times
            struct tm i_time_struct;
            time_t i_time;
            char ctime[20], mtime[20], atime[20];
            i_time =  inode->i_ctime;
            i_time_struct = *gmtime(&i_time);
            strftime(ctime, 80, "%m/%d/%y %H:%M:%S", &i_time_struct);
            i_time =  inode->i_mtime;
            i_time_struct = *gmtime(&i_time);
            strftime(mtime, 80, "%m/%d/%y %H:%M:%S", &i_time_struct);
            i_time =  inode->i_atime;
            i_time_struct = *gmtime(&i_time);
            strftime(atime, 80, "%m/%d/%y %H:%M:%S", &i_time_struct);
            fprintf(stdout, ",%s,%s,%s", ctime, mtime, atime);
            
            //last three stdout
            fprintf(stdout, ",%d,%d",  inode->i_size,  inode->i_blocks);
            for (int d = 0; d < 15; d++)
            {
                fprintf(stdout, ",%d",  inode->i_block[d]);
            }
            fprintf(stdout, "\n");
            
            //DIRECTORY ENTRIES
            if (ftype == 'd')
            {
                directory_entries(i);
            }
            
            //INDIRECT ENTRY - SINGLE
            if (inode->i_block[12] != 0)
            {
                single_indirect_entry(i, ftype);
            }
            
            //INDIRECT ENTRY - DOUBLE
            if (inode->i_block[13] != 0)
            {
                double_indirect_entry(i, ftype);
            }
            
            //INDIRECT ENTRY - TRIPLE
            if (inode->i_block[14] != 0)
            {
                //fprintf(stdout, "INDIRECT3 HERE\n");
                triple_indirect_entry(i, ftype);
            }
        }
    }
}

void triple_indirect_entry(int parent_inode, char ftype)
{
    int *d_ptr = malloc(sb_size);
    int *s_ptr = malloc(sb_size);
    int *t_ptr = malloc(sb_size);
    int n_ptr = sb_size/4;
    pread(filename, t_ptr, sb_size, BLOCK_OFFSET(inode->i_block[14]));
    for (int i = 0; i < n_ptr; i++)
    {
        if (t_ptr[i] != 0)
        {
            fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n", parent_inode, 3, 65536+256+12+i, inode->i_block[14], t_ptr[i]);
            
            pread(filename, d_ptr, sb_size, t_ptr[i]*sb_size);
            for (int j = 0; j < n_ptr; j++)
            {
                if (d_ptr[j] != 0)
                {
                    fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n", parent_inode, 2, 65536+256+12+j, t_ptr[i], d_ptr[j]);
                    
                    pread(filename, s_ptr, sb_size, d_ptr[j]*sb_size);
                    for (int k = 0; k < n_ptr; k++)
                    {
                        if (s_ptr[k] != 0)
                        {
                            if (ftype == 'd')
                                directory_entries(parent_inode);
                            fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n", parent_inode, 1, 65536+256+12+k, d_ptr[j], s_ptr[k]);
                        }
                    } //end for single
                }
            } //end for double
        }
    } //end for triple
    free(d_ptr);
    free(s_ptr);
    free(t_ptr);
}

void double_indirect_entry(int parent_inode, char ftype)
{
    int *d_ptr = malloc(sb_size);
    int *s_ptr = malloc(sb_size);
    int n_ptr = sb_size/4;
    pread(filename, d_ptr, sb_size, BLOCK_OFFSET(inode->i_block[13]));
    for (int i = 0; i < n_ptr; i++)
    {
        if (d_ptr[i] != 0)
        {
            fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n", parent_inode, 2, 256+12+i, inode->i_block[13], d_ptr[i]);
            
            pread(filename, s_ptr, sb_size, d_ptr[i]*sb_size);
            for (int j = 0; j < n_ptr; j++)
            {
                if (s_ptr[j] != 0)
                {
                    if (ftype == 'd')
                        directory_entries(parent_inode);
                    fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n", parent_inode, 1, 256+12+j, d_ptr[i], s_ptr[j]);
                }
            } //end for single
        }
    } //end for double
    free(d_ptr);
    free(s_ptr);
}

void single_indirect_entry(int parent_inode, char ftype)
{
    int *s_ptr = malloc(sb_size);
    int n_ptr = sb_size/4;
    pread(filename, s_ptr, sb_size, BLOCK_OFFSET(inode->i_block[12]));
    
    for (int i = 0; i < n_ptr; i++)
    {
        if (s_ptr[i] != 0)
        {
            if (ftype == 'd')
                directory_entries(parent_inode);
            fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n", parent_inode, 1, 12+i, inode->i_block[12], s_ptr[i]);
        }
    }
    free(s_ptr);
}

//code helped and written with TA Claudia during Office Hours: 2/25/2019 3:10pm
void directory_entries(int parent_inode)
{
    unsigned char block[sb_size];
    int size = 0;
    
    for (int i = 0; i < EXT2_N_BLOCKS; i++)
    {
        if (inode->i_block[i] == 0)
            break;
        int direntry_off = inode->i_block[i]*sb_size;
        void* tempdep = (void*) &entry;
        
        while (size < sb_size)
        {
            pread(filename, tempdep, sizeof(entry), direntry_off+size);
            int name_len_mine = strlen(entry.name);
            if (entry.inode != 0)
            {
                fprintf(stdout, "DIRENT,%d,%d,%d,%d,%d,'%s'\n", parent_inode, size, entry.inode, entry.rec_len, name_len_mine, entry.name);
            }
            size = size + entry.rec_len;
        }
    }
    return;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Error: bad arguments\n");
        exit(1);
    }
    
    filename = open(argv[1], O_RDONLY);
    if (filename < 0)
    {
        fprintf(stderr, "Error: file was unable to be opened\n");
        exit(1);
    }
    
    superblock_function();
    group_function();
    inode_function();
}

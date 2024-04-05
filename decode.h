#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

typedef struct _decodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;

    /* Decoded file Info */
    char dest_fname[20];
    FILE *fptr_dest_file;
    char *dest_file_extn;
    int dest_file_extn_size;
    int dest_file_data_size;
} DecodeInfo;

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decoding_files(DecodeInfo *decInfo);

/* Decode the data from the source image */
Status decode_byte_to_lsb(char *image_buffer, char *data);

/* Decode the magic string from the source image */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode data from the source image */
Status decode_data_from_image(char *data, int size, FILE *fptr_src_image);

/* Decode the size from the least significant bits (LSB) */
Status decode_size_from_lsb(char *image_buffer, int *size);

/* Decode the secret file extension from the source image */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode the size of the secret file extension from the source image */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode the size of the secret file data from the source image */
Status decode_secret_file_data_size(DecodeInfo *decInfo);

/* Decode the secret file data from the source image */
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif

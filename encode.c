#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (argv[2] != NULL)
    {
        if (!strcmp(strchr(argv[2], '.'), ".bmp"))
        {
            encInfo->src_image_fname = argv[2];
        }
        else
        {
            printf("INFO: Please pass a BMP file as an argument.\n");
            return e_failure;
        }
    }
    else
    {
        printf("INFO: Please pass a BMP file as an argument.\n");
        return e_failure;
    }

    if (argv[3] != NULL)
    {
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file, strchr(argv[3], '.'));
    }
    else
    {
        printf("INFO: Please pass the secret file.\n");
        return e_failure;
    }

    if (argv[4] != NULL)
    {
        encInfo->stego_image_fname = argv[4];
    }
    else







































    {
        encInfo->stego_image_fname = "default_stego.bmp";
    }

    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    printf("INFO: ## Encoding Procedure Started ##\n");
    if (open_files(encInfo) == e_success)
    {
        printf("INFO: Open files successful.\n");
    }
    else
    {
        printf("INFO: Failed to open files.\n");
        return e_failure;
    }

    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: BMP header copied successfully.\n");
    }
    else
    {
        printf("INFO: Failed to copy BMP header.\n");
        return e_failure;
    }

    if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
    {
        printf("INFO: Encode magic string successful.\n");
    }
    else
    {
        printf("INFO: Failed to encode magic string.\n");
        return e_failure;
    }

    if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_success)
    {
        printf("INFO: Secret file extension size encoding successful.\n");
    }
    else
    {
        printf("INFO: Failed to encode secret file extension size.\n");
        return e_failure;
    }

    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
    {
        printf("INFO: Secret file extension encoding successful.\n");
    }
    else
    {
        printf("INFO: Failed to encode secret file extension.\n");
        return e_failure;
    }

    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
    {
        printf("INFO: Encode secret file size successful.\n");
    }
    else
    {
        printf("INFO: Failed to encode secret file size.\n");
        return e_failure;
    }

    if (encode_secret_file_data(encInfo) == e_success)
    {
        printf("INFO: Encode secret file data successful.\n");
    }
    else
    {
        printf("INFO: Failed to encode secret file data.\n");
        return e_failure;
    }

    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: Copied remaining data successfully.\n");
    }
    else
    {
        printf("INFO: Failed to copy remaining data.\n");
        return e_failure;
    }

    // No failure, return e_success
    return e_success;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char bmp_header[54];
    rewind(fptr_src_image);
    rewind(fptr_dest_image);
    fread(bmp_header, 54, 1, fptr_src_image);
    fwrite(bmp_header, 54, 1, fptr_dest_image);
    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }
    else
    {
        rewind(encInfo->fptr_secret);
        fseek(encInfo->fptr_secret, 0, SEEK_END);
        encInfo->size_secret_file = ftell(encInfo->fptr_secret);
    }
    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
        return e_failure;
    }

    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 7; i >= 0; i--)
    {
        image_buffer[i] = ((data & (1 << i)) >> i) | (image_buffer[i] & 0xFE);
    }
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char str[8];
    for (int i = 0; i < size; i++)
    {
        fread(str, 8, 1, fptr_src_image);
        encode_byte_to_lsb(data[i], str);
        fwrite(str, 8, 1, fptr_stego_image);
    }
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char str[32];
    fread(str, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(size, str);
    fwrite(str, 1, 32, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_size_to_lsb(int size, char *image_buffer)
{
    for (int i = 31; i >= 0; i--)
    {
        image_buffer[i] = ((size & (1 << i)) >> i) | (image_buffer[i] & 0xFE);
    }
}

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char str[32];
    fread(str, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, str);
    fwrite(str, 1, 32, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char str[encInfo->size_secret_file];
    rewind(encInfo->fptr_secret);
    fread(str, 1, encInfo->size_secret_file, encInfo->fptr_secret);
    encode_data_to_image(str, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while (fread(&ch, 1, 1, fptr_src) > 0)
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }
    return e_success;
}

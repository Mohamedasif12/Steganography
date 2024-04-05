/*
Name        :Mohamed Asif
Date        :27-02-2024
description : project lsb stegography(encoding and decoding)
             encode text file in bmp image file
             decode text file from bmp stego image
sample input:bmp image file,secret data text file
*/


#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>

OperationType check_operation_type(char *argv[])
{
    if (!strcmp(argv[1], "-e"))
    {
        return e_encode;
    }
    else if (!strcmp(argv[1], "-d"))
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}

int main(int argc, char *argv[])
{
    if (argv[1] != NULL)
    {
        printf("INFO: Operation type: %s\n", argv[1]);

        int operation = check_operation_type(argv);
        if (operation == e_encode)
        {
            printf("INFO:<----------Selected encoding--------->\n");
            EncodeInfo encInfo;
            if (read_and_validate_encode_args(argv, &encInfo) == e_success)
            {
                printf("INFO: read and validate encode arguments success\n");
                if (do_encoding(&encInfo) == e_success)
                {
                    printf("INFO: ## Encoding Procedure Success ##\n");
                }
                else
                {
                    printf("INFO: do encoding failure\n");
                }
            }
            else
            {
                printf("INFO: read and validate enc args failed\n");
            }
        }
        else if (operation == e_decode)
        {
            printf("INFO: <-------selected decoding------->\n");
            DecodeInfo decInfo;
            if (read_and_validate_decode_args(argv, &decInfo) == e_success)
            {
                printf("INFO: read and validate decode args success\n");
                if (do_decoding(&decInfo) == e_success)
                {
                    printf("INFO: ## Decoding Procedure Success ##\n");
                }
                else
                {
                    printf("INFO: Decoding failure\n");
                }
            }
            else
            {
                printf("INFO: read and validate decode arguments failed\n");
            }
        }
        else
        {
            printf("INFO: Please pass the correct option\n");
        }
    }
    else
    {
        printf("INFO: Please provide the operation type.\n");
        return e_unsupported;
    }
    return 0;
}

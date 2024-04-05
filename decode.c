#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"

//Read and validate 
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
	if (argv[2] != NULL)
	{
		if (!strcmp(strchr(argv[2], '.'), ".bmp"))
		{
			decInfo->src_image_fname = argv[2];
		}
		else
		{
			printf("INFO: please pass argument bmp file\n");
		}
	}
	else
	{
		printf("INFO: please pass argument bmp file\n");
		return e_failure;
	}

	if (argv[3] != NULL)
	{
		strcpy(decInfo->dest_fname, argv[3]);
	}
	else
	{
		strcpy(decInfo->dest_fname, "decoded_file");
	}

	return e_success;
}
//decoding
Status do_decoding(DecodeInfo *decInfo)
{
	printf("INFO: ## Decoding Procedure Started ##\n");

	if (open_decoding_files(decInfo) == e_success)
	{
		printf("INFO: Opening required files\n");
	}
	else
	{
		printf("ERROR: Failed to open files\n");
		return e_failure;
	}

	if (decode_magic_string(decInfo) == e_success)
	{
		printf("INFO: Decoding Magic String Signature\n");
	}
	else
	{
		printf("ERROR: Failed to decode magic string\n");
		return e_failure;
	}

	if (decode_secret_file_extn_size(decInfo) == e_success)
	{
		printf("INFO: Success decode secret file extension size\n");
	}
	else
	{
		printf("ERROR: Failed to decode secret file extension size\n");
		return e_failure;
	}

	if (decode_secret_file_extn(decInfo) == e_success)
	{
		printf("INFO: Success decode secret file extension\n");
	}
	else
	{
		printf("ERROR: Failed to decode secret file extension\n");
		return e_failure;
	}

	if (decode_secret_file_data_size(decInfo) == e_success)
	{
		printf("INFO: Success decode secret file data size\n");
	}
	else
	{
		printf("ERROR: Failed to decode secret file data size\n");
		return e_failure;
	}

	if (decode_secret_file_data(decInfo) == e_success)
	{
		printf("INFO: Success decode secret file data\n");
	}
	else
	{
		printf("ERROR: Failed to decode secret file data\n");
		return e_failure;
	}

	return e_success;
}
//open decode file
Status open_decoding_files(DecodeInfo *decInfo)
{
	// Src Image default stego file
	decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "r");
	// Do Error handling
	if (decInfo->fptr_src_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->src_image_fname);
		return e_failure;
	}
	else
	{
		rewind(decInfo->fptr_src_image);
		fseek(decInfo->fptr_src_image, 54, SEEK_SET);
	}

	return e_success;
}
//decode magic string
Status decode_magic_string(DecodeInfo *decInfo)
{
	char magic_string[strlen(MAGIC_STRING) + 1];
	decode_data_from_image(magic_string, strlen(MAGIC_STRING), decInfo->fptr_src_image);
	if (strcmp(MAGIC_STRING, magic_string) == 0)
	{
                printf("INFO: Magic string detected.\n"); 
		return e_success;
	}
	else
	{
		printf("INFO: Magic string not detected.\n");
		return e_failure;
	}
}
//decode data from image and store in output file
Status decode_data_from_image(char *data, int size, FILE *fptr_src_image)
{
	char str[8];
	char decoded_byte[8];

	for (int i = 0; i < size; i++)
	{
		fread(str, 8, 1, fptr_src_image);
		decode_byte_to_lsb(str, decoded_byte);

		data[i] = 0; // Initialize the byte

		for (int j = 0; j <= 7; j++)
		{
			data[i] |= (decoded_byte[j] << j);
		}
	}

	data[size] = '\0'; // Add null terminator to form a valid string
	return e_success;
}

//decode byte to lsb
Status decode_byte_to_lsb(char *image_buffer, char *data)
{
	for (int i = 0; i < 8; i++)
	{
		data[i] = (image_buffer[i] & 0x01);
	}

	return e_success;
}

//decode secret file exten
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
	char str[32];
	int size;

	if (fread(str, 1, 32, decInfo->fptr_src_image) != 32)
	{
		printf("ERROR: Failed to read secret file extension size from source image\n");
		return e_failure;
	}

	if (decode_size_from_lsb(str, &size) != e_success)
	{
		printf("ERROR: Failed to decode secret file extension size\n");
		return e_failure;
	}

	decInfo->dest_file_extn_size = size;

	return e_success;
}

//decode size from lsb
Status decode_size_from_lsb(char *image_buffer, int *size)
{
	*size = 0;

	for (int i = 0; i < 32; i++)
	{
		*size |= (image_buffer[i] & 0x01) << i;
	}

	return e_success;
}

//decode secret file ext
Status decode_secret_file_extn(DecodeInfo *decInfo)
{

	char secret_file_extn[decInfo->dest_file_extn_size + 1];

	if (decode_data_from_image(secret_file_extn, decInfo->dest_file_extn_size, decInfo->fptr_src_image) != e_success)
	{
		printf("ERROR: Failed to decode secret file extension\n");
		return e_failure;
	}
	secret_file_extn[decInfo->dest_file_extn_size] = '\0'; // Add null terminator
	strcpy(decInfo->dest_fname, strcat(decInfo->dest_fname, secret_file_extn));
	return e_success;
}
//decode secret file data size

Status decode_secret_file_data_size(DecodeInfo *decInfo)
{
	char str[32];
	int size;

	if (fread(str, 1, 32, decInfo->fptr_src_image) != 32)
	{
		printf("ERROR: Failed to read secret file extension size from source image\n");
		return e_failure;
	}

	if (decode_size_from_lsb(str, &size) != e_success)
	{
		printf("ERROR: Failed to decode secret file extension size\n");
		return e_failure;
	}
	decInfo->dest_file_data_size = size;
	return e_success;
}

//decode secret file data
Status decode_secret_file_data(DecodeInfo *decInfo)
{
	char secret_file_data[decInfo->dest_file_data_size + 1];

	if (decode_data_from_image(secret_file_data, decInfo->dest_file_data_size, decInfo->fptr_src_image) != e_success)
	{
		printf("ERROR: Failed to decode secret file data\n");
		return e_failure;
	}

	decInfo->fptr_dest_file = fopen(decInfo->dest_fname, "w");
	// Do Error handling
	if (decInfo->fptr_dest_file == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open or write to file %s\n", decInfo->dest_fname);
		return e_failure;
	}

	fwrite(secret_file_data, 1, decInfo->dest_file_data_size, decInfo->fptr_dest_file);

	fclose(decInfo->fptr_dest_file); // Close the destination file
	return e_success;
}

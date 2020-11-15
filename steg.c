#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct pixel
{
	unsigned char b, g, r, a;
} pixel_t;

typedef struct bitmap
{
	unsigned int offset;
	unsigned int depth;
	unsigned int file_size;
	unsigned int width, height;
	pixel_t * pixels;
} bitmap_t;

bitmap_t * read_bitmap(FILE * file)
{
	bitmap_t * bmp = (bitmap_t *)malloc(sizeof(bitmap_t));

	// BM Header
	char header_field [2];
	fread(&header_field, 1, 2, file);

	if(header_field[0] != 'B' && header_field[1] != 'M')
		return NULL;

	unsigned int file_size;
	fread(&file_size, sizeof(unsigned int), 1, file);
	bmp->file_size = file_size;

	// Offset
	unsigned int offset;
	fseek(file, 10, SEEK_SET);
	fread(&offset, sizeof(unsigned int), 1, file);
	bmp->offset = offset;

	// DIB header info
	unsigned int h_size;
	fread(&h_size, sizeof(unsigned int), 1, file);

	// Width and hieght
	unsigned int width, height;
	unsigned short planes, depth;
	fread(&width, 4, 1, file);
	fread(&height, 4, 1, file);
	fread(&planes, 2, 1, file);
	fread(&depth, 2, 1, file);

	bmp->width = width;
	bmp->height = height;
	bmp->depth = depth;

	fseek(file, offset, SEEK_SET);
	bmp->pixels = (pixel_t*)malloc(width * height * sizeof(pixel_t));

	unsigned char r, g, b, a;
	int index = 0;
	while( ftell(file) != file_size)
	{

		fread(&b, sizeof(unsigned char), 1, file);
		fread(&g, sizeof(unsigned char), 1, file);
		fread(&r, sizeof(unsigned char), 1, file);
		fread(&a, sizeof(unsigned char), 1, file);

		bmp->pixels[index].r = r;
		bmp->pixels[index].g = g;
		bmp->pixels[index].b = b;
		bmp->pixels[index].a = a;

		index++;
	}

	return bmp;
}


void write_bitmap(FILE* out, bitmap_t * bmp)
{	
	int size = bmp->width * bmp->height;

	fseek(out, bmp->offset, SEEK_SET);
	fwrite(bmp->pixels, 4, size, out);
#if 0
	for(int i = 0; i < size; i++)
	{
		struct pixel px = bmp->pixels[i];
		fwrite(&px.b, sizeof(char), 1, out);
		fwrite(&px.g, sizeof(char), 1, out);
		fwrite(&px.r, sizeof(char), 1, out);
		fwrite(&px.a, sizeof(char), 1, out);
	}
#endif
}


void enc_char(char c, pixel_t * px)
{
	char bin[9] = { '0', '0', '0','0', '0', '0', '0', '0' ,'\0' };
	unsigned int cv = c;

	for(int i = 7; i > -1; i --)
	{
		if(cv % 2 != 0)
		{
			bin[i] = '1';
		}
		else
		{
			bin[i] = '0';
		}

		cv = cv / 2;
	}

	// printf("r: %u g: %u, b: %u, a: %u\n", px->r, px->g, px->b, px->a);

	px->r = px->r - (px->r % 4);
	px->g = px->g - (px->g % 4);
	px->b = px->b - (px->b % 4);
	px->a = px->a - (px->a % 4);

	if(bin[0] == '0' && bin[1] == '1') px->a ++;
	else if(bin[0] == '1' && bin[1] == '0') px->a += 2;
	else if(bin[0] == '1' && bin[1] == '1') px->a += 3;

	if(bin[2] == '0' && bin[3] == '1') px->r ++;
	else if(bin[2] == '1' && bin[3] == '0') px->r += 2;
	else if(bin[2] == '1' && bin[3] == '1') px->r += 3;

	if(bin[4] == '0' && bin[5] == '1') px->g ++;
	else if(bin[4] == '1' && bin[5] == '0') px->g += 2;
	else if(bin[4] == '1' && bin[5] == '1') px->g += 3;

	if(bin[6] == '0' && bin[7] == '1') px->b ++;
	else if(bin[6] == '1' && bin[7] == '0') px->b += 2;
	else if(bin[6] == '1' && bin[7] == '1') px->b += 3;

}

unsigned char dec_char(pixel_t * px)
{
	unsigned int value = 0;
	int r = px->r % 4;
	int g = px->g % 4;
	int b = px->b % 4;
	int a = px->a % 4;

	if(a == 1) value += 64;			// 0100 0000
	else if(a == 2) value += 128;	// 1000 0000
	else if(a == 3) value += 198;	// 1100 0000

	if(r == 1) value += 16;			// 0001 0000
	else if(r == 2) value += 32;	// 0010 0000
	else if(r == 3) value += 48;	// 0011 0000

	if(g == 1) value += 4;			// 0000 0100
	else if(g == 2) value += 8;		// 0000 1000
	else if(g == 3) value += 12 ;	// 0000 1100

	if(b == 1) value += 1;			// 0000 0001
	else if(b == 2) value += 2;		// 0000 0010
	else if(b == 3) value += 3;		// 0000 0011

	return (unsigned char)value;
}

void enc_text(char * text, pixel_t * data)
{
	size_t size = strlen(text);

	for(size_t i = 0; i < size - 1 ; i++)
	{
		char c = text[i];
		// printf("int : %u ", c);
		pixel_t * px = &data[i];

		enc_char(c, px);
	}

	enc_char('\0', &data[size - 1]);
}

void copy_file(FILE * in, FILE * out)
{
	fseek(in, 0, SEEK_END);
	long file_size = ftell(in);
	rewind(in);

	char * d = malloc(file_size * sizeof(char) + 1);
	fread(d, sizeof(char), file_size, in);

	d[file_size + 1] = '\0';

	fseek(out, 0, SEEK_SET);
	fwrite(d, sizeof(char), file_size, out);

	free(d);
}

char * read_plain_text(const char * file_name)
{
	FILE * in = fopen(file_name, "rb");

	fseek(in, 0, SEEK_END);
	long file_size = ftell(in);
	rewind(in);

	char * d = malloc(file_size * sizeof(char) + 1);
	fread(d, sizeof(char), file_size, in);

	fclose(in);
	return d;
}

void print_help()
{
	const char * help = "Usage: steg [mode] [type] [input_file] [out_file] [plain_text]\n"
		"Mode : Encypting or decrypting\n\t-e - encrypting\n\t"
		"-d - decrypting\nType : Image or text data\n\t"
		"-t - Text data\n\t"
		"-i - Image data\n"
		"Example : Encrypting\n\t"
		"\x1B[32msteg -e -t input.bmp output.bmp plain.txt\x1B[0m\n"
		"Example : Decrypting\n\t"
		"\x1B[32msteg -d -t input.bmp output.txt\x1B[0m\n"
		"\n\n-------------------\n"
		"Author  : James Njenga\nWebsite : jnjenga.com\n"
		"Email   : me@jnjenga.com\n";

	printf("%s\n", help);
}

int main(int argc, const char *argv[argc+1])
{
	if(argc < 4)
	{
		print_help();
		exit(1);
	}

	const char * mode = argv[1];
	const char * type = argv[2];
	const char * in_name = argv[3];
	const char * out_name = argv[4];

	FILE * in_file, * out_file;
	in_file = fopen(in_name, "rb");
	out_file = fopen(out_name, "wb");

	if(in_file == NULL)
	{
		printf("\x1b[31mError : %s doesn't exist\x1b[0m\n", in_name);
		fclose(out_file);
		return 0;
	}
	else if(out_file == NULL)
	{
		printf("\x1b[31mError : %s doesn't exist\x1b[0m\n", out_name);
		fclose(out_file);
		return 0;
	}
	bitmap_t * bmp = read_bitmap(in_file);

	if(mode[0] == '-' && mode[1] == 'e')
	{
		printf("Encrypting...\n");
		copy_file(in_file, out_file);
		if(type[0] == '-' && type[1] == 't')
		{
			// Read text file
			const char * ptext_path = argv[5];
			char * data = read_plain_text(ptext_path);

			if(strlen(data) > (bmp->width * bmp->height))
			{
				printf("\x1b[31mText too large for file\n"
						"Should be less than %d characters\x1B[0m\n", bmp->width * bmp->height);
				fclose(in_file);
				fclose(out_file);
				free(bmp->pixels);
				free(bmp);			
				return 0;
			}

			enc_text(data, bmp->pixels);

			write_bitmap(out_file, bmp);
			free(data);
		}
		// else if(type[0] == '-' && type[0] == 'i')
		// {
		// }
		else{
			print_help();
			fclose(in_file);
			fclose(out_file);
			free(bmp->pixels);
			free(bmp);
		}
		printf("Done\n\x1B[32mOuput : %s\n\x1b[0m", out_name);
	}
	else if(mode[0] == '-' && mode[1] == 'd')
	{
		printf("Decrypting...\n");
		if(type[0] == '-' && type[1] == 't')
		{
			int size = bmp->width * bmp->height;
			for(int i = 0; i < size; i ++)
			{
				struct pixel* px = &(bmp->pixels[i]);
				unsigned char c = dec_char(px);
				if(c == '\0') break;
				fwrite(&c, sizeof(unsigned char), 1, out_file);
			}
		}
		// else if(type[0] == '-' && type[0] == 'i')
		// {
		// }
		else
		{
			print_help();
			fclose(in_file);
			fclose(out_file);
			free(bmp->pixels);
			free(bmp);
		}
		printf("Done\n\x1B[32mOuput : %s\n\x1b[0m", out_name);
	}else
	{
		print_help();
		fclose(in_file);
		fclose(out_file);
		free(bmp->pixels);
		free(bmp);

	}

	fclose(in_file);
	fclose(out_file);
	free(bmp->pixels);
	free(bmp);
	return 0;
}

/*
 * A simple tool to generate bootable image for sunxi platform.
 *
 * Copyright (C) 2007-2011 Allwinner Technology Co., Ltd.
 *	Tom Cubie <tangliang@allwinnertech.com>
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

/* boot head definition from sun4i boot code */
struct boot_file_head {
	uint32_t jump_instruction;	/* one intruction jumping to real code */
	uint8_t magic[8];		/* ="eGON.BT0" or "eGON.BT1", not C-style str */
	uint32_t check_sum;		/* generated by PC */
	uint32_t length;		/* generated by PC */
	/* We use a simplified header, only filling in what is needed by the
	 * boot ROM. To be compatible with Allwinner tools the larger header
	 * below should be used, followed by a custom header if desired. */
	uint8_t pad[12];		/* align to 32 bytes */
};

static const char *BOOT0_MAGIC = "eGON.BT0";
static const uint32_t STAMP_VALUE = 0x5F0A6C39;
static const int HEADER_SIZE = 32;
/* Checksum at most 24 KiB */
#define SRAM_LOAD_MAX_SIZE ((24 << 10) - sizeof(struct boot_file_head))
static const int BLOCK_SIZE = 512;

inline static uint32_t le32_to_h(const void *src)
{
	const uint8_t *b = src;
	return ((b[3] << 24) | (b[2] << 16) | (b[1] << 8) | (b[0] << 0));
}

inline static void h_to_le32(uint32_t val32, void *dest)
{
	uint8_t *b = dest;
	b[0] = (val32 >> 0) & 0xff;
	b[1] = (val32 >> 8) & 0xff;
	b[2] = (val32 >> 16) & 0xff;
	b[3] = (val32 >> 24) & 0xff;
};

static void serialize_header(void *dest, const struct boot_file_head *hdr)
{
	/* Unused fields are zero */
	memset(dest, 0, HEADER_SIZE);

	h_to_le32(hdr->jump_instruction, dest + 0);
	memcpy(dest + 4, BOOT0_MAGIC, 8);
	h_to_le32(hdr->check_sum, dest + 12);
	h_to_le32(hdr->length, dest + 16);
}

/* Check sum function from sun4i boot code */
static int fill_check_sum(struct boot_file_head *hdr, const void *boot_code)
{
	size_t i;
	uint8_t raw_hdr[HEADER_SIZE];
	uint32_t chksum;

	if ((hdr->length & 0x3) != 0) {
		fprintf(stderr, "BUG! Load size is not 4-byte aligned\n");
		return EXIT_FAILURE;
	}

	/* Fill in checksum seed */
	hdr->check_sum = STAMP_VALUE;

	chksum = 0;
	/* Checksum the header */
	serialize_header(raw_hdr, hdr);
	for (i = 0; i < HEADER_SIZE; i += 4)
		chksum += le32_to_h(raw_hdr + i);

	/* Checksum the boot code */
	for (i = 0; i < hdr->length - HEADER_SIZE; i += 4)
		chksum += le32_to_h(boot_code + i);

	/* write back check sum */
	hdr->check_sum = chksum;

	return EXIT_SUCCESS;
}

static uint32_t align(uint32_t size, uint32_t alignment)
{
	return ((size + alignment - 1) / alignment) * alignment;
}

static void fill_header(struct boot_file_head *hdr, size_t load_size)
{
	/* B instruction */
	hdr->jump_instruction = 0xEA000000;
	/* Jump to the first instr after the header */
	hdr->jump_instruction |= (sizeof(*hdr) / sizeof(uint32_t) - 2);
	/* No '0' termination in magic string */
	memcpy(&hdr->magic, BOOT0_MAGIC, 8);

	hdr->length = align(load_size + sizeof(hdr), BLOCK_SIZE);
}

static long int fsize(FILE *file)
{
	long int size;

	fseek(file, 0L, SEEK_END);
	size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	return size;
}

int main(int argc, char *argv[])
{
	FILE *fd_in, *fd_out;
	struct boot_file_head hdr;
	long int file_size, load_size;
	void *file_data;
	uint8_t raw_hdr[HEADER_SIZE];
	int count;

	/*
	 * TODO: We could take an additional argument to see how much of the
	 * file to checksum. This way, the build system can tell us how large
	 * the bootblock is, so we can tell the BROM to load only the bootblock.
	 */
	if (argc < 2) {
		printf("\tThis program makes an input bin file to sun4i "
		       "bootable image.\n"
		       "\tUsage: %s input_file out_putfile\n", argv[0]);
		return EXIT_FAILURE;
	}

	fd_in = fopen(argv[1], "r");
	if (!fd_in) {
		fprintf(stderr, "Cannot open input %s", argv[1]);
		return EXIT_FAILURE;
	}

	/* Get input file size */
	file_size = fsize(fd_in);
	if ((file_data = malloc(file_size)) == NULL) {
		fprintf(stderr, "Cannot allocate memory\n");
		return EXIT_FAILURE;
	}

	printf("File size: 0x%x\n", file_size);
	if (fread(file_data, file_size, 1, fd_in) != 1) {
		fprintf(stderr, "Cannot read %s: %s\n", argv[1],
			strerror(errno));
		return EXIT_FAILURE;
	}

	load_size = align(file_size, sizeof(uint32_t));

	if (load_size > SRAM_LOAD_MAX_SIZE)
		load_size = SRAM_LOAD_MAX_SIZE;

	printf("Load size: 0x%x\n", load_size);

	fd_out = fopen(argv[2], "w");
	if (!fd_out) {
		fprintf(stderr, "Cannot open output %s\n", argv[2]);
		return EXIT_FAILURE;
	}

	/* Fill the header */
	fill_header(&hdr, load_size);
	fill_check_sum(&hdr, file_data);

	/* Now write the header */
	serialize_header(raw_hdr, &hdr);
	if (fwrite(raw_hdr, HEADER_SIZE, 1, fd_out) != 1) {
		fprintf(stderr, "Cannot write header to %s: %s\n", argv[1],
			strerror(errno));
		return EXIT_FAILURE;
	}

	/* And finally, the boot code */
	if (fwrite(file_data, file_size, 1, fd_out) != 1) {
		fprintf(stderr, "Cannot write to %s: %s\n", argv[1],
			strerror(errno));
		return EXIT_FAILURE;
	}

	fclose(fd_in);
	fclose(fd_out);

	return EXIT_SUCCESS;
}
/* Allow usage of US GSM-providers on Motorola MSM8960 phones by setting RF NV 8322 to 0
 *
 * Copyright (c) 2015 The CyanogenMod Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int logging_mode(int internal)
{
	char *mode;
	int fd;
	int len;

	if (internal)
		mode = "internal";
	else
		mode = "usb";

	printf("Setting logging_mode to %s...\n", mode);
	fd = open("/sys/devices/virtual/diag/diag/logging_mode", O_WRONLY);
	if (fd == -1) {
		perror("open");
		return 0;
	}

	len = write(fd, mode, strlen(mode));
	if (len != (int)strlen(mode)) {
		perror("write");
		return 0;
	}

	close(fd);

	return 1;
}

uint16_t crc16(unsigned char* data_p, unsigned char length){
	unsigned short crc = 0xFFFF;
	int i;

	while (length--) {
		crc ^= *data_p++;
		for (i = 0; i < 8; i++) {
			if (crc & 1) {
				crc = (crc >> 1) ^ 0x8408;
			} else {
				crc = (crc >> 1);
			}
		}
	}

	return ~crc;
}

int diag_rw(int fd, unsigned char *req, int req_len, unsigned char *resp, int *resp_len)
{
	int ret;
	int len;
	unsigned char buf[1024];
	uint16_t crc;
	int i;

	memset(buf, 0, sizeof(buf));
	memcpy(buf, req, req_len);

	crc = crc16(req, req_len);
	buf[req_len++] = crc & 0xff;
	buf[req_len++] = (crc >> 8) & 0xff;

	for (i = 0; i < req_len; i++) {
		if ((buf[i] != 0x7d) && ((buf[i] != 0x7e)))
			continue;

		memmove(buf+i+1, buf+i, req_len - i);
		req_len++;
		buf[i] = 0x7d;
		i++;
		buf[i] ^= 0x20;
	}

	buf[i] = 0x7e;
	req_len++;

	do {
		errno = 0;
		ret = write(fd, buf, req_len);
	} while ((ret != req_len) && (errno == EAGAIN));

	if (ret != req_len) {
		perror("write");
		return 0;
	}

	len = read(fd, resp, *resp_len);
	if (len == -1) {
		perror("read");
		return 0;
	}

	for (i = 0; i < len; i++) {
		if (resp[i] == 0x7e) {
			/* Should actually check CRC here... */
			len = i - 2;
			break;
		}

		if (resp[i] != 0x7d)
			continue;

		memmove(resp + i, resp + i + 1, len - (i + 1));
		resp[i] ^= 0x20;
		len--;
	}

	*resp_len = len;

	return 1;
}

int main(int argc, char **argv)
{
	unsigned char nv_get[] = { 0x26, 0x82, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	unsigned char nv_set[] = { 0x27, 0x82, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	unsigned char data[512];
	int fd;
	int len;
	int ret;
	int i;

	if (argc > 1) {
		do {
			if (argc == 2) {
				if (!strcmp(argv[1], "lock")) {
					printf("Locking USA GSM!\n");
					nv_set[3] = 0x01;
					break;
				}
			}

			fprintf(stderr, "Syntax: %s [lock]\n\n", argv[0]);
			fprintf(stderr, "\tlock\tlock operation on US GSM providers\n");
			exit(EXIT_FAILURE);
		} while(0);
	}

	if (!logging_mode(1)) {
		exit(EXIT_FAILURE);
	}

	fd = open("/dev/diag_tty", O_RDWR);
	if ((fd == -1) && (errno == ENOENT)) {
		dev_t dev;

		printf("Creating /dev/diag_tty...\n");

		dev = makedev(185, 0);
		ret = mknod("/dev/diag_tty", S_IFCHR | 0600, dev);
		if (ret == -1) {
			perror("mknod");
			goto err;
		}

		fd = open("/dev/diag_tty", O_RDWR);
	}

	if (fd == -1) {
		perror("open");
		goto err;
	}

	printf("Opened diag...\n");

	/* GSM inside US */
	printf("\n");
	printf("Reading value of NV-item 8322...\n");

	len = sizeof(data);
	if (!diag_rw(fd, nv_get, sizeof(nv_get), data, &len)) {
		goto err;
	}

	if (len != sizeof(nv_get)) {
		printf("Wrong amount of data read %d != %d!\n", len, sizeof(nv_get));
		goto err;
	}

	printf("GSM locked in the US: %d\n", data[3]);
	if (data[3] == nv_set[3]) {
		printf("No need to change anything!\n");
	} else {
		printf("Setting value of NV-item 8322 to %d...\n", nv_set[3]);

		len = sizeof(data);
		if (!diag_rw(fd, nv_set, sizeof(nv_set), data, &len)) {
			goto err;
		}

		printf("Reading value of NV-item 8322...\n");

		len = sizeof(data);
		if (!diag_rw(fd, nv_get, sizeof(nv_get), data, &len)) {
			goto err;
		}

		if (len != sizeof(nv_get)) {
			printf("Wrong amount of data read %d != %d!\n", len, sizeof(nv_get));
			goto err;
		}

		printf("GSM locked in the US: %d\n", data[3]);
		if (data[3] != nv_set[3]) {
			printf("Couldn't change GSM-us-lock-bit!\n");
			goto err;
		}
	}

	close(fd);

	if (!logging_mode(0)) {
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);

err:
	logging_mode(0);
	exit(EXIT_FAILURE);
}

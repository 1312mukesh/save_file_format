#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include<error.h>

#define QEMU_VM_FILE_MAGIC           0x5145564d
#define QEMU_VM_FILE_VERSION_COMPAT  0x00000002
#define QEMU_VM_FILE_VERSION         0x00000003

int buf_index;
/*int buf_size = 4;
void qemu_file_skip(QEMUFile *f, int size)
{
    if (buf_index + size <= buf_size) {
        buf_index += size;
    }
}
*/
int qemu_get_byte(FILE *f, uint8_t *buff)
{
    int result;

    result = fread(buff, 1, 1, f);
//    qemu_file_skip(f, 1);
	return buff[buf_index];
}

unsigned int qemu_get_be32(FILE *f, uint8_t *buff)
{
    unsigned int v;
    v = (unsigned int)qemu_get_byte(f, buff) << 24;
//    buf_index += 1;
    v |= qemu_get_byte(f, buff) << 16;
//    buf_index += 1;
    v |= qemu_get_byte(f, buff) << 8;
//    buf_index += 1;
    v |= qemu_get_byte(f, buff);
//    buf_index += 1;
    return v;
}

int main()
{
        FILE *fp;
        uint8_t buff[16];
/*        char buff1[8];
        int read_byte;
	int count = 0;*/
	unsigned int v;
 
        memset(buff, '\0', 16);
//        memset(buff1, '\0', 8);
        fp = fopen("STATEFILE.txt", "r");

        v = qemu_get_be32(fp, buff); 
       if (v != QEMU_VM_FILE_MAGIC) {
          printf("Not a migration stream:%x:\n", v);
          return -1;
      }
	printf("printing ::%x\n", v);
      v = qemu_get_be32(fp, buff);
      if (v == QEMU_VM_FILE_VERSION_COMPAT) {
         printf("SaveVM v2 format is obsolete and don't work anymore");
         return -1;
     }
	printf("printing ::%x\n", v);
     if (v != QEMU_VM_FILE_VERSION) {
         printf("Unsupported migration stream version");
         return -1;
     }
	printf("printing ::%x\n", v);

/*	while (1) {
		if ((read_byte = fread(buff, 1, 1, fp)) != 0) {
				strncpy(buff1, buff, read_byte);
		printf("%s\n", buff1);
        	memset(buff, '\0', 16);
        	memset(buff1, '\0', 8);
		count += read_byte;
		if (count == 400)
			break;
	}
*/
        return 0;
}


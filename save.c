#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include<error.h>

#define QEMU_VM_FILE_MAGIC           0x5145564d
#define QEMU_VM_FILE_VERSION_COMPAT  0x00000002
#define QEMU_VM_FILE_VERSION         0x00000003

#define QEMU_VM_EOF                  0x00
#define QEMU_VM_SECTION_START        0x01
#define QEMU_VM_SECTION_PART         0x02
#define QEMU_VM_SECTION_END          0x03
#define QEMU_VM_SECTION_FULL         0x04 
#define QEMU_VM_SUBSECTION           0x05
#define QEMU_VM_VMDESCRIPTION        0x06
#define QEMU_VM_CONFIGURATION        0x07
#define QEMU_VM_SECTION_FOOTER       0x7e

int buf_index;
int qemu_get_byte(FILE *f, char *buff)
{
	int result;
	result = fread(buff, 1, 1, f);
	return buff[buf_index];
}

unsigned int qemu_get_be32(FILE *f, char *buff)
{
    unsigned int v;
    v = (unsigned int)qemu_get_byte(f, buff) << 24;
    v |= qemu_get_byte(f, buff) << 16;
    v |= qemu_get_byte(f, buff) << 8;
    v |= qemu_get_byte(f, buff);
    return v;
}

int qemu_get_buffer(FILE *f, uint8_t *buf, int size)
{
    int pending = size;
    int done = 0;

    while (pending > 0) {
        int res;
        uint8_t src[4];

//        res = qemu_peek_buffer(f, &src, MIN(pending, IO_BUF_SIZE), 0);
        res = fread(src, 1, 1, f);
        if (res == 0) {
            return done;
        }
        memcpy(buf, src, res);
//        qemu_file_skip(f, res);
        buf += res;
        pending -= res;
        done += res;
    }
    return done;
}

size_t qemu_get_counted_string(FILE *f, char buf[256])
{
    size_t len = qemu_get_byte(f, buf);
    size_t res = qemu_get_buffer(f, (uint8_t *)buf, len);
    printf("length::%u\n res::%u\n", len, res);
    buf[res] = 0;

    return res == len ? res : 0;
}

int main()
{
        FILE *fp;
        char buff[16];
	unsigned int v;
	int vm_configrtn;
	int count = 0;
 
	memset(buff, '\0', 16);
        fp = fopen("STATEFILE.txt", "r");

        v = qemu_get_be32(fp, buff); 
       if (v != QEMU_VM_FILE_MAGIC) {
          printf("Not a migration stream:%x:\n", v);
          return -1;
      }
	printf("Qemu VM File Magic::%x\n", v);
      v = qemu_get_be32(fp, buff);
      if (v == QEMU_VM_FILE_VERSION_COMPAT) {
         printf("SaveVM v2 format is obsolete and don't work anymore");
         return -1;
     }
     if (v != QEMU_VM_FILE_VERSION) {
         printf("Unsupported migration stream version");
         return -1;
     }
	printf("Qemu VM file version::%x\n", v);

	if ((vm_configrtn = qemu_get_byte(fp, buff))!= QEMU_VM_CONFIGURATION) {
		printf("Configuration section missing");
		return -1;
	}
	printf("Qemu VM configuration::%x\n", vm_configrtn);
	
	int read_byte;
	read_byte = qemu_get_be32(fp, buff);
	printf("length of the configuration::%x\nconfiguration is::", read_byte);
	while (read_byte--){ 
		qemu_get_byte(fp, buff);
		printf("%c", *buff);
	}
	printf("\n");
		
//	qemu_get_byte(fp, buff)
	uint8_t section_type;
    while ((section_type = qemu_get_byte(fp, buff)) != QEMU_VM_EOF) {
        uint32_t instance_id, version_id, section_id;
//        SaveStateEntry *se;
//        LoadStateEntry *le;
        char idstr[256];

//        trace_qemu_loadvm_state_section(section_type);
	printf("section type::%x\n", section_type);
        switch (section_type) {
        case QEMU_VM_SECTION_START:
        case QEMU_VM_SECTION_FULL:
            /* Read section start */
            section_id = qemu_get_be32(fp, buff);
		printf("section id::%x\n", section_id);
            if (!qemu_get_counted_string(fp, idstr)) {
                printf("Unable to read ID string for section %u",
                            section_id);
                return -1;
            }
            instance_id = qemu_get_be32(fp, buff);
            version_id = qemu_get_be32(fp, buff);
		printf("instance id::%x\n", instance_id);
		printf("version id::%x\n", version_id);
		printf("idstr::%s\n", idstr);

//            trace_qemu_loadvm_state_section_startfull(section_id, idstr,
//                                                      instance_id, version_id);
            /* Find savevm section */
/*            se = find_se(idstr, instance_id);
            if (se == NULL) {
                error_report("Unknown savevm section or instance '%s' %d",
                             idstr, instance_id);
                ret = -EINVAL;
                goto out;
            }
*/
            /* Validate version */
/*            if (version_id > se->version_id) {
                error_report("savevm: unsupported version %d for '%s' v%d",
                             version_id, idstr, se->version_id);
                ret = -EINVAL;
                goto out;
            }
*/
            /* Add entry */
/*            le = g_malloc0(sizeof(*le));

            le->se = se;
            le->section_id = section_id;
            le->version_id = version_id;
            QLIST_INSERT_HEAD(&mis->loadvm_handlers, le, entry);

                                                                             1116,13       70%
            ret = vmstate_load(f, le->se, le->version_id);
            if (ret < 0) {
                error_report("error while loading state for instance 0x%x of"
                             " device '%s'", instance_id, idstr);
                goto out;
            }
            if (!check_section_footer(f, le)) {
                ret = -EINVAL;
                goto out;
            }*/
            break;
        case QEMU_VM_SECTION_PART:
        case QEMU_VM_SECTION_END:
/*            section_id = qemu_get_be32(f);

            trace_qemu_loadvm_state_section_partend(section_id);
            QLIST_FOREACH(le, &mis->loadvm_handlers, entry) {
                if (le->section_id == section_id) {
                    break;
                }
            }
            if (le == NULL) {
                error_report("Unknown savevm section %d", section_id);
                ret = -EINVAL;
                goto out;
            }
            printf("section end\n");
            ret = vmstate_load(f, le->se, le->version_id);
            if (ret < 0) {
                error_report("error while loading state section id %d(%s)",
                             section_id, le->se->idstr);
                goto out;
            }
            if (!check_section_footer(f, le)) {
                ret = -EINVAL;
                goto out;
            }*/
            break;
        default:
          printf("Unknown savevm section type %d", section_type);
//            ret = -1;           goto out;
        }
    }

        return 0;
}


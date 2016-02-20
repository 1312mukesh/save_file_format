#ifndef __HEADER__
#define __HEADER__
uint64_t qemu_get_be64(FILE *f, char *buff);
/* ram save/restore */

#define RAM_SAVE_FLAG_FULL     0x01 /* Obsolete, not used anymore */
#define RAM_SAVE_FLAG_COMPRESS 0x02
#define RAM_SAVE_FLAG_MEM_SIZE 0x04
#define RAM_SAVE_FLAG_PAGE     0x08
#define RAM_SAVE_FLAG_EOS      0x10
#define RAM_SAVE_FLAG_CONTINUE 0x20
#define RAM_SAVE_FLAG_XBZRLE   0x40
/* 0x80 is reserved in migration.h start with 0x100 next */
#define RAM_SAVE_FLAG_COMPRESS_PAGE    0x100

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

typedef uintptr_t ram_addr_t;
int buf_index;
#define TARGET_PAGE_BITS 12
#define TARGET_PAGE_SIZE (1 << TARGET_PAGE_BITS)
#define TARGET_PAGE_MASK ~(TARGET_PAGE_SIZE - 1)

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

uint64_t qemu_get_be64(FILE *f, char *buff)
{       
    uint64_t v; 
    v = (uint64_t)qemu_get_be32(f, buff) << 32;
    v |= qemu_get_be32(f, buff);
    return v;
}
int qemu_get_buffer(FILE *f, uint8_t *buf, int size)
{
        int pending = size;
        int done = 0;

        while (pending > 0) {
                int res;
                uint8_t src[4];

		res = fread(src, 1, 1, f);
		if (res == 0) {
			return done;
		}
		memcpy(buf, src, res);buf += res;
		pending -= res;
		done += res;
	}
	return done;
}
                
              
                                                                                                                                                       size_t qemu_get_counted_string(FILE *f, char buf[256])
{
	size_t len = qemu_get_byte(f, buf);
	size_t res = qemu_get_buffer(f, (uint8_t *)buf, len);
	
//	printf("length::%u\n res::%u\n", len, res);
	buf[res] = 0;
	return res == len ? res : 0;
}
      
int check_section_footer(FILE *f, char *le, char *buff)
{
    uint8_t read_mark;
    uint32_t read_section_id;

//    if (skip_section_footers) {
        /* No footer to check */
//        return true;
//    }

    read_mark = qemu_get_byte(f, buff);

    if (read_mark != QEMU_VM_SECTION_FOOTER) {
        printf("Missing section footer for %s", read_mark);
        return 0;
    }

    read_section_id = qemu_get_be32(f, buff);
/*  if (read_section_id != le->section_id) {
        error_report("Mismatched section id in footer for %s -"
                     " read 0x%x expected 0x%x",
                     le->se->idstr, read_section_id, le->section_id);
        return false;
    }*/

    /* All good */
    return 1;
}


int ram_load(FILE *f, void *opaque, int version_id, char *buff)
{
    int flags = 0, ret = 0;
    static uint64_t seq_iter;
    int len = 0;

    seq_iter++;

    if (version_id != 4) {
        printf("version_id != 4:::%d\n", version_id);
        ret = -1;
    }

    /* This RCU critical section can be very long running.
     * When RCU reclaims in the code start to become numerous,
     * it will be necessary to reduce the granularity of this
     * critical section.
     */
//    rcu_read_lock();
    while (!ret && !(flags & RAM_SAVE_FLAG_EOS)) {
        ram_addr_t addr, total_ram_bytes;
	void *host;
        uint8_t ch;

        addr = qemu_get_be64(f, buff);
        flags = addr & ~TARGET_PAGE_MASK;
        addr &= TARGET_PAGE_MASK;
        printf("%s\n Loadaddress_while_resuming:%x\n", __func__, addr);
//printf("Ram_load::switch_statement:%d\n", flags & ~RAM_SAVE_FLAG_CONTINUE);
        switch (flags & ~RAM_SAVE_FLAG_CONTINUE) {
        case RAM_SAVE_FLAG_MEM_SIZE:
            /* Synchronize RAM block list */
            total_ram_bytes = addr;
	    printf("total_ram_bytes::%llu\n", total_ram_bytes);
	 
            while (!ret && total_ram_bytes) {
 //               RAMBlock *block;
                char id[256];
                ram_addr_t length;

                len = qemu_get_byte(f, buff);
                qemu_get_buffer(f, (uint8_t *)id, len);
                id[len] = 0;
		printf("id::%s\n", id);
                length = qemu_get_be64(f, buff);
		printf("length::%lu\n", length);
/*
                QLIST_FOREACH_RCU(block, &ram_list.blocks, next) {
                    if (!strncmp(id, block->idstr, sizeof(id))) {
                        if (length != block->used_length) {
                            Error *local_err = NULL;

                            ret = qemu_ram_resize(block->offset, length, &local_err);
                                                                        1532,1        93%
                            if (local_err) {
                                error_report_err(local_err);
                            }
                        }
                        ram_control_load_hook(f, RAM_CONTROL_BLOCK_REG,
                                              block->idstr);
                        break;
                    }
                }

                if (!block) {
                    error_report("Unknown ramblock \"%s\", cannot "
                                 "accept migration", id);
                    ret = -EINVAL;
                }*/

                total_ram_bytes -= length;
            }
            break;
        case RAM_SAVE_FLAG_COMPRESS:
	      printf("RAM_SAVE_FLAG_COMPRESS:Address or the offset::%u\n", addr);
/*            host = host_from_stream_offset(f, addr, flags);
            if (!host) {
                error_report("Illegal RAM offset " RAM_ADDR_FMT, addr);
                ret = -EINVAL;
                break;
            }
            ch = qemu_get_byte(f);
            ram_handle_compressed(host, ch, TARGET_PAGE_SIZE);*/
	
//		qemu_get_byte(f, buff);
            break;
        case RAM_SAVE_FLAG_PAGE:
	      printf("RAM_SAVE_FLAG_PAGE:Address or the offset::%u\n", addr);
/*            host = host_from_stream_offset(f, addr, flags);
            if (!host) {
                error_report("Illegal RAM offset " RAM_ADDR_FMT, addr);
                ret = -EINVAL;
                break;
            }*/
//            qemu_get_buffer(f, buff, TARGET_PAGE_SIZE);
            break;
        case RAM_SAVE_FLAG_COMPRESS_PAGE:
	      printf("RAM_SAVE_FLAG_COMPRESS_PAGE:Address or the offset::%u\n", addr);
/*            host = host_from_stream_offset(f, addr, flags);
            if (!host) {
                error_report("Invalid RAM offset " RAM_ADDR_FMT, addr);
                ret = -EINVAL;
                break;
           }

            len = qemu_get_be32(f);
            if (len < 0 || len > compressBound(TARGET_PAGE_SIZE)) {
                error_report("Invalid compressed data length: %d", len);
                ret = -EINVAL;
                break;
            }
            qemu_get_buffer(f, compressed_data_buf, len);
            decompress_data_with_multi_threads(compressed_data_buf, host, len);*/
            break;
        case RAM_SAVE_FLAG_XBZRLE:
/*            host = host_from_stream_offset(f, addr, flags);
            if (!host) {
                error_report("Illegal RAM offset " RAM_ADDR_FMT, addr);
                ret = -EINVAL;
                break;
            }
            if (load_xbzrle(f, addr, host) < 0) {
                error_report("Failed to decompress XBZRLE page at "
                             RAM_ADDR_FMT, addr);
                ret = -EINVAL;
                break;
            }*/
            break;
      case RAM_SAVE_FLAG_EOS:
	printf("RAM_SAVE_FLAG_EOS::%d, ret = %d\n", RAM_SAVE_FLAG_EOS, ret);
            /* normal exit */
            break;
/*          if (flags & RAM_SAVE_FLAG_HOOK) {
                ram_control_load_hook(f, RAM_CONTROL_HOOK, NULL);
            } else {
                error_report("Unknown combination of migration flags: %#x",
                             flags);
                ret = -EINVAL;
            }*/
        }
      //  if (!ret) {
//            ret = qemu_file_get_error(f);
    	//    return ret;
      //  }
    }

//    rcu_read_unlock();
//    DPRINTF("Completed load of VM with exit code %d seq iteration "
//            "%" PRIu64 "\n", ret, seq_iter);
    return ret;
}

         
#endif

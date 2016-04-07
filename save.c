#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include<error.h>
#include "header.h"


int main()
{
	FILE *fp;
	char buff[32768];
	unsigned int v;
	int vm_configrtn;
	int count = 0;
	int ret;

	memset(buff, '\0', 32768);
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

	if ((vm_configrtn = qemu_get_byte(fp, buff))!= QEMU_VM_CONFIGURATION){
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
				printf("instance id::%u\n", instance_id);
				printf("version id::%u\n", version_id);
				printf("idstr::%s\n", idstr);

				//            trace_qemu_loadvm_state_section_startfull(section_id, idstr,
				//                                                      instance_id, version_id);
				/* Find savevm section */
//				            se = find_se(idstr, instance_id);
//					      if (se == NULL) {
//					      printf("Unknown savevm section or instance '%s' %d",
//					      idstr, instance_id);
//					      ret = -1;
//					      return ret;
				//	      goto out;
//					      }
					      
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
					      QLIST_INSERT_HEAD(&mis->loadvm_handlers, le, entry);*/

					      if (version_id == 4)
					      ret = ram_load(fp, NULL, version_id, buff);
						else {
						ret = 0;
						memset(buff, '\0', 32768);
						printf("%x\n", qemu_get_be32(fp, buff));	
				//		printf("%x\n", qemu_get_byte(fp, buff));	
				//		printf("%x\n", qemu_get_byte(fp, buff));	
					/*	memset(buff, '\0', 32768);
						qemu_get_be32(fp, buff);
						memset(buff, '\0', 32768);
						qemu_get_byte(fp, buff);*/
						}
					      /*ret = vmstate_load(f, le->se, le->version_id);*/
					      if (ret < 0) {
					      printf("error while loading state for instance 0x%x of"
					      " device '%s'\n", instance_id, idstr);
					      return ret;
					      }
					      if (!check_section_footer(fp, NULL, buff)) {
					      return -1;
					      }
				break;
			case QEMU_VM_SECTION_PART:
			case QEMU_VM_SECTION_END:
					      printf("QEMU_VM_SECTION_END:\n");
				              section_id = qemu_get_be32(fp, buff);
					      printf("section id::%x\n", section_id);
					      version_id = 4;
/*					      trace_qemu_loadvm_state_section_partend(section_id);
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
                                             if (version_id == 4) {
						memset(buff, '\0', 32768);
						printf("hello\n");
                                              ret = ram_load(fp, NULL, version_id, buff);
						}
                                              else {
                                              ret = 0;
					      memset(buff, '\0', 32768);
					     printf("%s\n", qemu_get_be32(fp, buff));	
					//	version_id =4;
//                                              memset(buff, '\0', 32768);
//                                              printf("%x\n", qemu_get_be32(fp, buff));
						}
                                              /*ret = vmstate_load(f, le->se, le->version_id);*/
                                              if (ret < 0) {
                                              printf("error while loading state for instance 0x%x of"
                                              " device '%s'\n", instance_id, idstr);
                                              return ret;
                                              }
                                              if (!check_section_footer(fp, NULL, buff)) {
                                              return -1;
                                              }

				break;
			default:
				printf("Unknown savevm section type %d", section_type);
				//            ret = -1;           goto out;
		}
	}

	return 0;
}


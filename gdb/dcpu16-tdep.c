#include "dcpu16-tdep.h"
#include "defs.h"
#include "gdbcore.h"

//extern initialize_file_ftype _initialize_dcpu16_tdep; /* -Wmissing-prototypes */

static struct gdbarch* dcpu16_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches) {
    struct gdbarch_list* list = gdbarch_list_lookup_by_info(arches, &info);
    struct gdbarch* result;
    
    if (list != NULL) {
        result = list->gdbarch;
    } else {        // If we didn't already have a arch configuration, make one
        result = gdbarch_alloc(&info, NULL);
    }
    
    return result;
}
                              
void _initialize_dcpu16_tdep() {
    gdbarch_register (bfd_arch_dcpu16, dcpu16_gdbarch_init, NULL); //dcpu16_dump_tdep);
    
}

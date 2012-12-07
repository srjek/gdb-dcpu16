#include "dcpu16-tdep.h"
#include "defs.h"
#include "gdbcore.h"
#include "arch-utils.h"

//extern initialize_file_ftype _initialize_dcpu16_tdep; /* -Wmissing-prototypes */

const char * dcpu16_register_name(struct gdbarch *gdbarch, int regnum) {
    //TODO: use target_desc, not hardcoded values
    
    switch (regnum) {
        case 0:
            return "A";
        case 1:
            return "B";
        case 2:
            return "C";
        case 3:
            return "X";
        case 4:
            return "Y";
        case 5:
            return "Z";
        case 6:
            return "I";
        case 7:
            return "J";
        case 8:
            return "PC";
        case 9:
            return "SP";
        case 10:
            return "EX";
        case 11:
            return "IA";
        case 12:
            return "IAQ";   //TODO: Is this even supposed to be here?
    }
    return NULL;
}

struct type * dcpu16_register_type(struct gdbarch *gdbarch, int regnum) {
    return NULL;    //TODO: this (I might be avoiding just enough code so the gdb executable doesn't give an error)
}

CORE_ADDR dcpu16_unwind_pc(struct gdbarch *gdbarch, struct frame_info *next_frame) {   //TODO: this (I may have just done the default implementation without knowing what it did)
    ULONGEST pc;
    pc = frame_unwind_register_unsigned (next_frame, ARCH_PC_REGNUM);
    return gdbarch_addr_bits_remove (gdbarch, pc);
}

CORE_ADDR dcpu16_unwind_sp(struct gdbarch *gdbarch, struct frame_info *next_frame) {   //TODO: this (I may have just done the default implementation without knowing what it did)
    ULONGEST sp;
    sp = frame_unwind_register_unsigned (next_frame, ARCH_SP_REGNUM);
    return gdbarch_addr_bits_remove (gdbarch, sp);
}

static struct gdbarch* dcpu16_gdbarch_init(struct gdbarch_info info, struct gdbarch_list *arches) {
    struct gdbarch_list* list = gdbarch_list_lookup_by_info(arches, &info);
    struct gdbarch* result;
    
    if (info.target_desc == NULL) { //Lets fill in the defaults
        //Or not (TODO)
    } else {
        //TODO: special stuff
    }
    
    if (list != NULL) {
        result = list->gdbarch;
    } else {        // If we didn't already have a arch configuration, make one
        result = gdbarch_alloc(&info, NULL);
        set_gdbarch_pc_regnum(result, 8);
        set_gdbarch_sp_regnum(result, 9);
        
        set_gdbarch_register_name(result, dcpu16_register_name);
        set_gdbarch_register_type(result, dcpu16_register_type);
        //set_gdbarch_skip_prologue(result, ); //TODO
        set_gdbarch_inner_than(result, core_addr_lessthan);
        
        set_gdbarch_unwind_pc(result, dcpu16_unwind_pc);
        set_gdbarch_unwind_sp(result, dcpu16_unwind_sp);
    }
    
    return result;
}
                              
void _initialize_dcpu16_tdep() {
    gdbarch_register (bfd_arch_dcpu16, dcpu16_gdbarch_init, NULL); //dcpu16_dump_tdep);
    
}

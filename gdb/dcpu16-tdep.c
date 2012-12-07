#include "defs.h"
#include "frame.h"
#include "gdbtypes.h"
#include "gdbcore.h"
#include "arch-utils.h"
#include "include/dis-asm.h"

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
    switch (regnum) {
        case 0:     //A
        case 1:     //B
        case 2:     //C
        case 3:     //X
        case 4:     //Y
        case 5:     //Z
        case 6:     //I
        case 7:     //J
            return builtin_type(gdbarch)->builtin_uint16;
        case 8:     //PC
            return builtin_type(gdbarch)->builtin_data_ptr; //Not a function pointer, a builtin_type doesn't exist for code pointers, so data pointer (because everything is data!)
        case 9:     //SP
            return builtin_type(gdbarch)->builtin_data_ptr;
        case 10:    //EX
            return builtin_type(gdbarch)->builtin_uint16;
        case 11:    //IA
            return builtin_type(gdbarch)->builtin_data_ptr; //Same situation as the PC register
        case 12:    //IAQ
            return builtin_type(gdbarch)->builtin_uint16;   //TODO: Is this even supposed to be here?
    }
    return NULL;    //We don't know what register this is (TODO: anything here to do once we add xml file support?)
}

CORE_ADDR dcpu16_skip_prologue (struct gdbarch *gdbarch, CORE_ADDR pc) {
    return pc;  //TODO: actually figure out what is supposed to go here (There's no prologue to skip in assembled files, and I don't know what the prologue in C,C++,etc is, if it exists) (It might be osabi?)
}

CORE_ADDR dcpu16_unwind_pc(struct gdbarch *gdbarch, struct frame_info *next_frame) {   //TODO: this (I may have just done the default implementation without knowing what it did)
    ULONGEST pc;
    pc = frame_unwind_register_unsigned (next_frame, 8);
    return gdbarch_addr_bits_remove (gdbarch, pc);
}

CORE_ADDR dcpu16_unwind_sp(struct gdbarch *gdbarch, struct frame_info *next_frame) {   //TODO: this (I may have just done the default implementation without knowing what it did)
    ULONGEST sp;
    sp = frame_unwind_register_unsigned (next_frame, 9);
    return gdbarch_addr_bits_remove (gdbarch, sp);
}

const gdb_byte *dcpu16_breakpoint_from_pc (gdbarch, pcptr, lenptr) {
    return NULL;    //Don't support software breakpoints (dcpu16 has no dedicated breakpoint instruction, through others could be repurposed, but only with compatible emulators)
}
int dcpu16_print_insn (bfd_vma vma, struct disassemble_info *info) {   //Disassemlbes an instruction
    return print_insn_dcpu16(vma, info);
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
        
        set_gdbarch_num_regs(result, 13);   //TODO: should be 12, but this includes the IAQ "register"
        set_gdbarch_pc_regnum(result, 8);
        set_gdbarch_sp_regnum(result, 9);
        
        set_gdbarch_register_name(result, dcpu16_register_name);
        set_gdbarch_register_type(result, dcpu16_register_type);
        set_gdbarch_skip_prologue(result, dcpu16_skip_prologue);
        set_gdbarch_inner_than(result, core_addr_lessthan);
        
        set_gdbarch_unwind_pc(result, dcpu16_unwind_pc);
        set_gdbarch_unwind_sp(result, dcpu16_unwind_sp);
        
        set_gdbarch_breakpoint_from_pc(result, dcpu16_breakpoint_from_pc);
        set_gdbarch_print_insn(result, dcpu16_print_insn);
    }
    
    return result;
}

extern initialize_file_ftype _initialize_dcpu16_tdep; /* -Wmissing-prototypes */

void
_initialize_dcpu16_tdep (void) {
    gdbarch_register (bfd_arch_dcpu16, dcpu16_gdbarch_init, NULL); //dcpu16_dump_tdep);
    
}

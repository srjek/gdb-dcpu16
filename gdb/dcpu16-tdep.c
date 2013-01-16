#include "defs.h"
#include "frame.h"
#include "frame-unwind.h"
#include "frame-base.h"
#include "gdbtypes.h"
#include "gdbcore.h"
#include "target-descriptions.h"
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
            return builtin_type(gdbarch)->builtin_uint16;   //Size of data_ptr is off, so using uint16 for now (until I either change the emulator or data_ptr) (TODO)
            //return builtin_type(gdbarch)->builtin_data_ptr; //Not a function pointer, a builtin_type doesn't exist for code pointers, so data pointer (because everything is data!)
        case 9:     //SP
            return builtin_type(gdbarch)->builtin_uint16;   //Size of data_ptr is off, so using uint16 for now (until I either change the emulator or data_ptr) (TODO)
            //return builtin_type(gdbarch)->builtin_data_ptr;
        case 10:    //EX
            return builtin_type(gdbarch)->builtin_uint16;
        case 11:    //IA
            return builtin_type(gdbarch)->builtin_uint16;   //Size of data_ptr is off, so using uint16 for now (until I either change the emulator or data_ptr) (TODO)
            //return builtin_type(gdbarch)->builtin_data_ptr; //Same situation as the PC register
        case 12:    //IAQ
            return builtin_type(gdbarch)->builtin_uint16;   //TODO: Is this even supposed to be here?
    }
    return NULL;    //We don't know what register this is (TODO: anything here to do once we add xml file support?)
}

/*
//dcpu16 addresses everything in words, but gdb does everything in bytes
CORE_ADDR dcpu16_pointer_to_address(struct gdbarch *gdbarch, struct type *type, const gdb_byte *buf) {
    enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
    CORE_ADDR addr = extract_unsigned_integer (buf, TYPE_LENGTH (type), byte_order);
    return addr << 0;
}
void dcpu16_address_to_pointer (struct gdbarch *gdbarch, struct type *type, gdb_byte *buf, CORE_ADDR addr) {
    enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
    store_unsigned_integer (buf, TYPE_LENGTH (type), byte_order, addr >> 0);
}
*/


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
    return NULL;    //Don't support software breakpoints (dcpu16 has no dedicated breakpoint instruction. Ununsed instructions could be repurposed, but only with compatible emulators, and at risk of future compatiblity)
}
int dcpu16_print_insn (bfd_vma vma, struct disassemble_info *info) {   //Disassemlbes an instruction
    return print_insn_dcpu16(vma, info) >> 1;    //NOTE: GDB uses number from here to advance, disassembly expects byte based addresses (maybe gdb does too? :/)
}


//srjek: I need to do more research to implement the frame functions myself, so I'm going to pull some BS to make this work for assembly programs
//              First of all, there is only one frame, the top level frame
//              Second, the stack starts at 0, the corresponding function starts at 0
enum unwind_stop_reason dcpu16_frame_unwind_stop_reason (struct frame_info * this_frame, void ** this_prologue_cache ) {
    fprintf_unfiltered(gdb_stdlog, "Going to say NOPE\n");
    return UNWIND_OUTERMOST;
}
void dcpu16_frame_this_id (struct frame_info *this_frame, void **this_prologue_cache, struct frame_id *this_id) {
    fprintf_unfiltered(gdb_stdlog, "Going to access this_id.... ");
    this_id->stack_addr = 0;
    this_id->code_addr = 0;
    this_id->special_addr = 0;
    fprintf_unfiltered(gdb_stdlog, "done\n");
}
struct value* dcpu16_frame_prev_register(struct frame_info *this_frame, void **this_prologue_cache, int regnum) {
    fprintf_unfiltered(gdb_stdlog, "NOPE\n");
    return NULL;
}
int dcpu16_frame_sniffer(const struct frame_unwind *self, struct frame_info *this_frame, void **this_prologue_cache) {
    return 1;   //TODO: find out what this function is supposed to do
}


static const struct frame_unwind dcpu16_frame_unwind = {
    NORMAL_FRAME, // enum frame_type type
    dcpu16_frame_unwind_stop_reason, // frame_unwind_stop_reason_ftype *stop_reason
    dcpu16_frame_this_id, // frame_this_id_ftype *this_id
    dcpu16_frame_prev_register, // frame_prev_register_ftype *prev_register   //Hopefully with the things I am pulling I don't have to mess with this
    NULL, // const struct frame_data *unwind_data       //....or this....
    dcpu16_frame_sniffer, // frame_sniffer_ftype *sniffer               //....or this....
    NULL, // frame_prev_pc_ftype *prev_pc;              //....or this....
    NULL, // frame_dealloc_cache_ftype *dealloc_cache;  //....or this....
};                                                      //....or anything in this structure......

const struct frame_base* dcpu16_base_sniffer(struct frame_info* this_frame) {
    fprintf_unfiltered(gdb_stdlog, "Going to return NULL, just thought I would let you know\n");
//    struct frame_base* result = {
//        dcpu16_frame_unwind,
//        };
    return NULL;
}


static struct gdbarch* dcpu16_gdbarch_init(struct gdbarch_info info, struct gdbarch_list *arches) {
    struct tdesc_arch_data* tdesc_data = NULL;
    if ((info.target_desc != NULL) && (tdesc_has_registers(info.target_desc))) {    //Check for a remote target description
        tdesc_data = tdesc_data_alloc();
        const struct tdesc_feature * core = tdesc_find_feature(info.target_desc, "org.srjek.dcpu16.core");
        if (core == NULL) return NULL;
        if (!tdesc_numbered_register (core, tdesc_data, 0, "A")) return NULL;   //Link their regnums to our regnums, or complain if something's missing
        if (!tdesc_numbered_register (core, tdesc_data, 1, "B")) return NULL;
        if (!tdesc_numbered_register (core, tdesc_data, 2, "C")) return NULL;
        if (!tdesc_numbered_register (core, tdesc_data, 3, "X")) return NULL;
        if (!tdesc_numbered_register (core, tdesc_data, 4, "Y")) return NULL;
        if (!tdesc_numbered_register (core, tdesc_data, 5, "Z")) return NULL;
        if (!tdesc_numbered_register (core, tdesc_data, 6, "I")) return NULL;
        if (!tdesc_numbered_register (core, tdesc_data, 7, "J")) return NULL;
        if (!tdesc_numbered_register (core, tdesc_data, 8, "PC")) return NULL;
        if (!tdesc_numbered_register (core, tdesc_data, 9, "SP")) return NULL;
        if (!tdesc_numbered_register (core, tdesc_data, 10, "EX")) return NULL;
        if (!tdesc_numbered_register (core, tdesc_data, 11, "IA")) return NULL;
        if (!tdesc_numbered_register (core, tdesc_data, 12, "IAQ")) return NULL;
    }
    
    struct gdbarch_list* list = gdbarch_list_lookup_by_info(arches, &info);
    struct gdbarch* result;
    
    if (list != NULL) {
        result = list->gdbarch;
        if (tdesc_data != NULL)
            tdesc_data_cleanup(tdesc_data);
        
    } else {        // If we didn't already have a arch configuration, make one
        result = gdbarch_alloc(&info, NULL);
        
        set_gdbarch_num_regs(result, 13);   //TODO: should be 12, but this includes the IAQ "register"
        set_gdbarch_pc_regnum(result, 8);
        set_gdbarch_sp_regnum(result, 9);
        
        set_gdbarch_register_name(result, dcpu16_register_name);
        set_gdbarch_register_type(result, dcpu16_register_type);
        //set_gdbarch_pointer_to_address(result, dcpu16_pointer_to_address);
        //set_gdbarch_integer_to_address(result, dcpu16_pointer_to_address);
        //set_gdbarch_address_to_pointer(result, dcpu16_address_to_pointer);
        
        set_gdbarch_skip_prologue(result, dcpu16_skip_prologue);
        set_gdbarch_inner_than(result, core_addr_lessthan);
        
        set_gdbarch_unwind_pc(result, dcpu16_unwind_pc);
        set_gdbarch_unwind_sp(result, dcpu16_unwind_sp);
        
        set_gdbarch_breakpoint_from_pc(result, dcpu16_breakpoint_from_pc);
        set_gdbarch_print_insn(result, dcpu16_print_insn);
        
        frame_unwind_append_unwinder(result, &dcpu16_frame_unwind);
        //frame_base_append_sniffer(result, dcpu16_base_sniffer);
        //frame_base_set_default(result, dcpu16_base_sniffer);
        
        if (tdesc_data != NULL)
            tdesc_use_registers(result, info.target_desc, tdesc_data);
    }
    
    return result;
}

extern initialize_file_ftype _initialize_dcpu16_tdep; /* -Wmissing-prototypes */

void
_initialize_dcpu16_tdep (void) {
    gdbarch_register (bfd_arch_dcpu16, dcpu16_gdbarch_init, NULL); //dcpu16_dump_tdep);
    
}

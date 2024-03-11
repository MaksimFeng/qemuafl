#include <inttypes.h>
#include <assert.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <glib.h>

#include <qemu-plugin.h>

// Define a structure for storing guest addresses to look for
typedef struct GuestAddressLookup {
    uint64_t guest_address; // The guest address to find
} GuestAddressLookup;

static GuestAddressLookup lookup;


static void vcpu_tb_trans_cb(qemu_plugin_id_t id, struct qemu_plugin_tb *tb)
{
    uint64_t tb_start = qemu_plugin_tb_vaddr(tb);
    size_t n_insns = qemu_plugin_tb_n_insns(tb);
    struct qemu_plugin_insn *last_insn = qemu_plugin_tb_get_insn(tb, n_insns - 1);
    uint64_t tb_end = qemu_plugin_insn_vaddr(last_insn) + qemu_plugin_insn_size(last_insn);

    // Check if the guest address is within this translation block's range
    if (lookup.guest_address >= tb_start && lookup.guest_address < tb_end) {
        // Found the translation block containing the guest address
        qemu_plugin_outs("Found TB containing guest address:\n");
        // Found the translation block containing the guest address
        qemu_plugin_outs("Found TB containing guest address:\n");
        qemu_plugin_outs("TB Start: ");
        qemu_plugin_outs(g_strdup_printf("0x%lx", tb_start));
        qemu_plugin_outs(", TB End: ");
        qemu_plugin_outs(g_strdup_printf("0x%lx\n", tb_end));

    }
}



static void plugin_init(qemu_plugin_id_t id, void *p)
{
    lookup.guest_address = 0x4005a0;

    // Register the callback for translation block translation events
    qemu_plugin_register_vcpu_tb_trans_cb(id, vcpu_tb_trans_cb);
}



QEMU_PLUGIN_EXPORT int qemu_plugin_install(qemu_plugin_id_t id, const qemu_info_t *info, int argc, char **argv)
{
    qemu_plugin_register_vcpu_tb_trans_cb(id, plugin_init);
    return 0;
}

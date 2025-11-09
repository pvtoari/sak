#include <io/acpi.h>
#include <std/string.h>

FADT_t *find_FACP(void *RootSDT) {
    RSDT_t *rsdt = (RSDT_t *) RootSDT;
    int entries = (rsdt->h.Length - sizeof(rsdt->h)) / 4;

    for (int i = 0; i < entries; i++) {
        ACPISDTHeader_t *h = (ACPISDTHeader_t *) rsdt->PointerToOtherSDT[i];
        if (!strcmp(h->Signature, "FACP"))
            return (void *) h;
    }

    // No FACP found
    return NULL;
}

#include "elf.h"
#include "kstdio.h"

elf_t elf_from_multiboot (multiboot_info_t *mb)
{
	size_t i;
	elf_t elf;
	elf_section_header_t *sh = (elf_section_header_t*)(mb->u.elf_sec.addr);

	uint32_t shstrtab = sh[mb->u.elf_sec.shndx].addr;
	for (i = 0; i < mb->u.elf_sec.num; i++)
	{
		const char *name = (const char *) (shstrtab + sh[i].name);
		if (!strcmp (name, ".strtab"))
		{
			elf.strtab = (const char *)sh[i].addr;
			elf.strtabsz = sh[i].size;
		}
		if (!strcmp (name, ".symtab"))
		{
			elf.symtab = (elf_symbol_t*)sh[i].addr;
			elf.symtabsz = sh[i].size;
		}
	}
	return elf;
}

const char *elf_lookup_symbol (uint32_t addr, elf_t *elf)
{
	size_t i;

	for (i = 0; i < (elf->symtabsz/sizeof (elf_symbol_t)); i++)
	{
		if (ELF32_ST_TYPE(elf->symtab[i].info) != 0x2)
			continue;

		if ( (addr >= elf->symtab[i].value) &&
				 (addr < (elf->symtab[i].value + elf->symtab[i].size)) )
		{
			const char *name = (const char *) ((uint32_t)elf->strtab + elf->symtab[i].name);
			return name;
		}
	}
	return 0;
}

void print_stack_trace ()
{
	uint32_t *ebp, *eip;
	__asm__ volatile ("mov %%ebp, %0" : "=r" (ebp));
	while (ebp)
	{
		eip = ebp+1;
		kprintf (DUMP,"	 [0x%x] %s\n", *eip, elf_lookup_symbol (*eip, &kernel_elf));
		ebp = (uint32_t*) *ebp;
	}
}
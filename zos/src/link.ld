OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)
phys = 0x00100000;
SECTIONS
{
	kernel_begin = .;
	.text phys : AT(phys)
	{
		code = .;
		*(.multiboot)
		*(.text)
		. = ALIGN(4096);
	}
	.rodata : AT(phys + (rodata - code))
	{
		rodata = .;
		*(.rodata)
		. = ALIGN(4096);
	}
	.data : AT(phys + (data - code))
	{
		data = .;
		*(.data)
		. = ALIGN(4096);
	}
	.bss : AT(phys + (bss - code))
	{
		bss = .;
		*(.bss)
		. = ALIGN(4096);
	}
	kernel_end = .;
	end = .; _end = .; __end = .;

	/DISCARD/ :
	{
		*(.comment)
		*(.eh_frame)
		*(.note.gnu.build-id)
	}
}
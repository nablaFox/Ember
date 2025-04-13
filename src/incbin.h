/* TEMPORARY: waiting for GCC 15 #embed feature
with #embed we will be able of defining static literals like this:

static constexpr unisgned char data[] = {
	#embed "path/to/file"
}

hence unused shaders can be stripped by the linker and not
embedded in the final binary. Users of the library are
encouraged to use the "-Wl,--gc-sections" flag when compiling
the final executable.
*/

#define _EMBED_BINARY(name, file)            \
	asm(".section .rodata\n"                 \
		".global " #name                     \
		"\n"                                 \
		".type " #name ", @object\n" #name   \
		":\n"                                \
		".incbin \"" file "\"\n" #name       \
		"_end:\n"                            \
		".size " #name ", .-" #name          \
		"\n"                                 \
		".previous\n");                      \
	extern const unsigned char name[];       \
	extern const unsigned char name##_end[]; \
	const size_t name##_size = (size_t)(name##_end - name);

#define EMBED_BINARY(name, file) _EMBED_BINARY(name, ETNA_SOURCE_DIR "/" file)

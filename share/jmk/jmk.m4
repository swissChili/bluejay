divert(-1)

dnl
dnl JMK (Jay MaKe) build system
dnl

define(`foreach', `pushdef(`$1')_foreach($@)popdef(`$1')')
define(`_arg1', `$1')
define(`_foreach', `ifelse(`$2', `()', `',
  `define(`$1', _arg1$2)$3`'$0(`$1', (shift$2), `$3')')')

define(status_log, `	@printf " \e[1;34m%8s\e[m  %s\n" "$1" "$2"')
define(DO_MAKE, `$(MAKE) --no-print-directory MAKEFILE_DEPTH="$$(( $(MAKEFILE_DEPTH) + 1))"')

define(init,
`jmk_project := $1
jmk_target = ifelse(`$#', 2, $2, $1)
ROOT := jmk_root
ASM ?= nasm
CC ?= gcc
LD ?= ld
CFLAGS += -I$(ROOT)/include
jmk_clean_libs =

MAKEFILE_DEPTH ?= 1

all: $(jmk_target)')

dnl preset applies a certain configuration preset to the project
define(preset,
    `ifelse($1, `freestanding',
        `CFLAGS += -nostdlib -nostdinc -fno-builtin -fno-stack-protector -ffreestanding',
        $1, `optimize', `CFLAGS += -O2',
        $1, `debug', `CFLAGS += -g',
        $1, `32', `CFLAGS += -m32',
        $1, `warn', `CFLAGS += -Wall -Wno-unused-function -Wno-unused-variable',
        $1, `nasm', `ASM = nasm')')

dnl this is really, really terrible, but to my knowledge there is no
dnl other way to escape a $. The manual says nothing about this.
define(ident, $1)
define(dollar_at, `ident($)ident(@)')

dnl archetype enables a language archetype
define(archetype,
    `ifelse($1, c, `.c.o:
status_log(CC, $<)
	@$(CC) -c $< -o dollar_at $(CFLAGS)',
        $1, asm, `.s.o:
status_log(AS, $<)
	@$(ASM) $(ASMFLAGS) $< -o dollar_at')')

dnl depends declares an external dependency
define(`depends', `
jmk_clean_libs += jmk_lib_clean_$1
jmk_lib_path_$1 = $2
jmk_lib_target_$1 = ifelse($3, `', $1.a, $3)
jmk_libs_phony += $(jmk_lib_path_$1)/$(jmk_lib_target_$1)

$(jmk_lib_path_$1)/$(jmk_lib_target_$1):
status_log(MAKE[$(MAKEFILE_DEPTH)], Entering $2)
	@DO_MAKE -C $(jmk_lib_path_$1) $(jmk_lib_target_$1)
status_log(MAKE[$(MAKEFILE_DEPTH)], Leaving $2)

jmk_lib_clean_$1:
	@DO_MAKE -C $2 clean')

dnl lib is used to list an external dependency declared with depends()
define(`lib', `$(jmk_lib_path_$1)/$(jmk_lib_target_$1)')

define(`phony', `jmk_custom_phony += $1')

dnl type is used to specify the target type
define(type,
    `ifelse($1, executable,
`$(jmk_target): $(OBJECTS)
status_log(CC, dollar_at)
	@$(CC) -o dollar_at $^ $(CFLAGS)',
    $1, static_lib,
`$(jmk_target): $(OBJECTS)
status_log(AR, dollar_at)
	@ar rcs dollar_at $^',
    $1, custom_link,
`$(jmk_target): $(OBJECTS)
status_log(LD, dollar_at)
	@$(LD) $(LDFLAGS) -o dollar_at $^')')

dnl finish is required at the end of the Jmk file to generate some
dnl final declarations

define(finish,
`clean: $(jmk_clean_libs)
	@rm -f *.o *.a *.so $(jmk_target)

Makefile: Jmk
status_log(JMK, jmk_build_dir)
	@cd "jmk_build_dir" && jmk_build_cmd

.PHONY: $(jmk_libs_phony) $(jmk_custom_phony) $(jmk_clean_libs) clean all')

divert(0)dnl

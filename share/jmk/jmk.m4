divert(-1)

dnl
dnl JMK (Jay MaKe) build system
dnl

define(`foreach', `pushdef(`$1')_foreach($@)popdef(`$1')')
define(`_arg1', `$1')
define(`_foreach', `ifelse(`$2', `()', `',
  `define(`$1', _arg1$2)$3`'$0(`$1', (shift$2), `$3')')')

define(status_log, `	@printf " \e[1;34m%8s\e[m  %s\n" "$1" "$2" > /dev/stderr')
define(DO_MAKE, `$(MAKE) --no-print-directory MAKEFILE_DEPTH="$$(( $(MAKEFILE_DEPTH) + 1))"')

define(gtags_path, $(ROOT)/GTAGS)
define(gen_gtags, ifelse(disable_gtags,true,,cd $(ROOT) && gtags))

define(init,
`jmk_project := $1
jmk_target = ifelse(`$#', 2, $2, $1)
ROOT := jmk_root
ASM ?= nasm
CC ?= gcc
LD ?= ld
LATEXC ?= pdflatex
LATEX_MAKE_GLOSSARIES = 0
LATEX_EXTRAS = *.aux *.glg *.glo *.gls *.idx *.ilg *ind *.ist *.log *.toc
CFLAGS += -I$(ROOT)/include
ASMFLAGS += -felf
jmk_clean_libs =

MAKEFILE_DEPTH ?= 1

ifneq (,$(wildcard ./Jmk.options))
     include Jmk.options
endif

all: $(jmk_target)')

dnl preset applies a certain configuration preset to the project
define(preset,
    `ifelse($1, `freestanding',
        `CFLAGS += -nostdlib -nostdinc -fno-builtin -fno-stack-protector -ffreestanding',
        $1, `optimize', `CFLAGS += -O2',
        $1, `debug', `CFLAGS += -g
ASMFLAGS += -Fdwarf',
        $1, `32', `CFLAGS += -m32',
        $1, `warn', `CFLAGS += -Wall -Wextra -Wno-unused-function -Wno-unused-variable -Wno-incompatible-pointer-types -Wno-unused-parameter',
        $1, `nasm', `ASM = nasm',
		$1, `glossaries', `LATEX_MAKE_GLOSSARIES = 1',
		$1, xelatex, `LATEXC = xelatex')')

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
define(`lib', `ifelse($1,,``$0'',$(jmk_lib_path_$1)/$(jmk_lib_target_$1))')

define(`phony', `jmk_custom_phony += $1')

dnl type is used to specify the target type
define(type,
    `ifelse($1, executable,
`$(jmk_target): $(OBJECTS)
status_log(LD, dollar_at)
	@gen_gtags
	@$(CC) -o dollar_at $^ $(CFLAGS)',
    $1, static_lib,
`$(jmk_target): $(OBJECTS)
status_log(AR, dollar_at)
	@gen_gtags
	@ar rcs dollar_at $^',
    $1, custom_link,
`$(jmk_target): $(OBJECTS)
status_log(LD, dollar_at)
	@gen_gtags
	@$(LD) $(LDFLAGS) -o dollar_at $^',
	$1, latex,
`$(jmk_target): $(LATEX_SOURCES)
status_log(LATEX, dollar_at)
	@$(LATEXC) $<
ifeq ($(LATEX_MAKE_GLOSSARIES),1)
status_log(GLOSSARY, dollar_at)
	@makeglossaries $(patsubst %.tex,%,$<)
	@$(LATEXC) $<
endif')')

define(option,
`$1 ?= $3
jmk_options += $1
jmk_option_help_$1 = $2 (default: $3)')

dnl finish is required at the end of the Jmk file to generate some
dnl final declarations

define(finish,
`clean: $(jmk_clean_libs)
	@rm -f **/*.o **/*.a *.so $(jmk_target) $(OBJECTS) $(LATEX_EXTRAS)

Makefile: Jmk
status_log(JMK, jmk_build_dir)
	@cd "jmk_build_dir" && jmk_build_cmd

gtags_path:
status_log(GTAGS,)
	gen_gtags

show_cflags:
	@echo $(CFLAGS)

.PHONY: $(jmk_libs_phony) $(jmk_custom_phony) $(jmk_clean_libs) clean all show_cflags')

divert(0)dnl

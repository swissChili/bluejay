# Rewrite of JMK in TCL

variable jmk_name {}
variable jmk_target {}
variable cflags {}
variable asmflags {}

variable asm as
variable cc cc

# variable options

variable src \$^
variable first_src \$<
variable target \$@

proc init {name {target {DEFAULT_TARGET}}} {
	if {$target eq {DEFAULT_TARGET}} {
		set target $name
	}

	global jmk_name
	global jmk_target

	set jmk_name $name
	set jmk_target $target

	puts {MAKEFILE_DEPTH ?= 1}

	rule all $target {}
}

proc preset {p} {
	::preset::$p
}

proc presets {args} {
	foreach arg $args {
		preset $arg
	}
}

proc cflag {arg} {
	global cflags
	set cflags "$cflags $arg"
}

proc cflags {args} {
	foreach arg $args {
		cflag $arg
	}
}

proc asmflag {arg} {
	global asmflags 
	set asmflags "$asmflags $arg"
}

proc asmflags {args} {
	foreach arg $args {
		asmflag $arg
	}
}

proc option {name val} {
	global options
	if {![info exists options($name)]} {
		set options($name) $val
	}
}

proc log {category message} {
	puts "\t@printf '\\e\[1;34m%8s\\e\[m  %s\\n' '$category' '$message' > /dev/stderr"
}

proc cc {command} {
	puts "\t@$::cc $command $::cflags"
}

proc asm {command} {
	puts "\t@$::asm $command $::asmflags"
}

proc shell {command} {
	puts "\t@$command"
}

proc rule {target deps does} {
	puts ""
	puts "$target: $deps"
	eval $does
}

proc type {type} {
	::type::$type
}

proc srcs {args} {
	puts ""
	variable objs ""

	foreach src $args {
		variable obj [regsub -- {(.+)\.\w+} $src {\1.o}]
		variable objs "$objs $obj"
	}

	puts "OBJECTS += $objs"
}

namespace eval type {
	proc executable {} {
		global jmk_target

		rule $jmk_target "\$(OBJECTS)" {
			log LD $::target
			cc "-o $::target $::src"
		}

		helpers
	}

proc helpers {} {
	rule .c.o {} {
		log CC $::first_src
		cc "-c $::first_src -o $::target"
	}

	rule .s.o {} {
		log ASM $::first_src
		asm "\$(ASMFLAGS) $::first_src -o $::target"
	}

	rule clean {} {
		shell "rm -f **/*.o **/*.a *.so $::target \$(OBJECTS)"
	}
}	
}

namespace eval preset {
	proc freestanding {} {
		cflags -nostdlib -nostdinc -fno-builtin -fno-stack-protector -ffreestanding
	}

	proc optimize {} {
		cflags -O2
	}
	
	proc 32 {} {
		cflag -m32
		asmflag -felf32
	}

	proc debug {} {
		cflag -g
		asmflag -Fdwarf
	}

	proc warn {} {
		cflags -Wall -Wextra -Wno-unused-function -Wno-unused-variable -Wno-incompatible-pointer-types -Werror
	}

	proc nasm {} {
		global asm
		set asm nasm
	}
}

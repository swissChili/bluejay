# Rewrite of JMK in TCL

variable jmk_name {}
variable jmk_target {}
variable jmk_clean_libs {}
variable jmk_phony_libs {}
variable jmk_lib_paths
variable jmk_lib_targets

variable jmk_sourced

variable cflags {}
variable asmflags {}
variable ldflags {}

variable asm as
variable cc gcc
variable ld ld

variable objs {}

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

	rule Makefile Jmk2 {
		log JMK2 ""
		shell "cd $::jmk_build_dir && $::jmk_build_cmd"
	}
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

proc ldflag {arg} {
	global ldflags
	set ldflags "$ldflags $arg"
}

proc ldflags {args} {
	foreach arg $args {
		ldflag $arg
	}
}

proc option {name val} {
	global options
	if {![info exists options($name)]} {
		set options($name) $val
	}
}

proc log {category message} {
	puts "\t@printf ' \\e\[1;34m%8s\\e\[m  %s\\n' '$category' '$message' > /dev/stderr"
}

proc jmk_log {message} {
	puts stderr $message
}

proc jmk_error {message} {
	puts stderr "\e[31mError\e[0m $message"
	exit 1
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

proc make {command} {
	shell "\$(MAKE) --no-print-directory MAKEFILE_DEPTH=\$\$((\$(MAKEFILE_DEPTH)+1)) $command"
}

proc rule {target deps does} {
	puts ""
	puts "$target: $deps"
	uplevel 1 $does
}

proc type {type} {
	::type::$type
}

proc objs {args} {
	foreach obj $args {
		set ::objs "$::objs $obj"
	}
}

proc srcs {args} {
	puts ""

	foreach src $args {
		set src [file join [pwd] $src]
		variable obj [regsub -- {(.+)\.\w+} $src {\1.o}]
		set ::objs "$::objs $obj"

		if {[string match *.c $src]} {
			variable cc $::cc
			if {[string match *distcc* $cc]} {
				variable cc [regsub -- {.*distcc +(.+)$} $cc {\1}]
			}

			if {[file exists $src]} {
				puts [exec sh -c "$cc $src -MM -MT $obj $::cflags"]
			} else {
				rule $obj $src {}
			}

			log CC [file normalize $src]
			cc "-c $::first_src -o $::target"
			puts ""
		}
	}
}

proc depends {name path {target DEFAULT_TARGET}} {
	if {$target eq {DEFAULT_TARGET}} {
		variable target "lib${name}.a"
	}

	set ::jmk_clean_libs "$::jmk_clean_libs $path"
	set ::jmk_lib_paths($name) $path
	set ::jmk_lib_target($name) $target
	set ::jmk_phony_libs "$::jmk_phony_libs $path"

	rule "$path/$target" {} {
		log "MAKE\[\$(MAKEFILE_DEPTH)\]" "Entering $name"
		make "-C $path $target"
		log "MAKE\[\$(MAKEFILE_DEPTH)\]" "Leaving $name"
	}
}

proc lib {name} {
	return "$::jmk_lib_paths($name)/$::jmk_lib_target($name)"
}

namespace eval type {
	proc executable {} {
		global jmk_target

		rule $jmk_target $::objs {
			log LD $::target
			cc "-o $::target $::src"
		}

		helpers
	}

proc custom_link {} {
	global jmk_target

	rule $jmk_target $::objs {
		log LD $::target
		shell "$::ld $::ldflags -o $::target $::src"
	}

	helpers
}

proc helpers {} {
	# rule .c.o {} {
	# 	log CC $::first_src
	# 	cc "-c $::first_src -o $::target"
	# }

	rule .s.o {} {
		log ASM $::first_src
		asm "\$(ASMFLAGS) $::first_src -o $::target"
	}

	rule clean {} {
		shell "rm -f **/*.o **/*.a *.so $::jmk_target $::objs"

		foreach lib $::jmk_clean_libs {
			make "-C $lib clean"
		}
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
		cflags -Wall -Wno-unused-function -Wno-unused-variable -Wno-incompatible-pointer-types -Wno-sign-compare
	}

	proc nasm {} {
		global asm
		set asm nasm
	}
}

proc jmk_source {path} {
	variable dir [pwd]

	if {![file exists $path]} {
		jmk_error "jmk_source: $dir/$path doesn't exist"
	}

	lappend ::jmk_sourced "$dir/$path"

	cd [file dirname $path]
	uplevel 1 source [file tail $path]
	cd $dir
}

proc jmk_finalize {} {
	puts "Jmk2: $::jmk_sourced"
}

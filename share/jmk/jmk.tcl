# Rewrite of JMK in TCL

variable jmk_name {}
variable jmk_target {}
variable cflags {}
variable asmflags {}

variable asm as
variable cc cc

variable options

proc init {name {target {DEFAULT_TARGET}}} {
	if {$target eq {DEFAULT_TARGET}} {
		set target $name
	}

	global jmk_name
	global jmk_target

	set jmk_name $name
	set jmk_target $target
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

namespace eval preset {
	proc 32 {} {
		cflag -m32
		asmflag -felf32
	}

	proc debug {} {
		cflag -g
		asmflag -Fdwarf
	}

	proc warn {} {
		cflag "-Wall -Wextra -Werror"
	}

	proc nasm {} {
		global asm
		set asm nasm
	}
}

if {[catch {array set options $argv} msg]} {
	puts "Sorry, you must pass an even number of arguments to this script"
	puts "in the form <key> <value>"
	exit 1
}

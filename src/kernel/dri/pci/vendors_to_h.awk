BEGIN {
    FS="\t"

    print "#include <dri/pci/vendors.h>"
    print ""
    print "struct pci_vendor pci_vendors[] ="
    print "{"
}

{
    print  "\t(struct pci_vendor){ .id = " $1 ", .name = \"" $2 "\" },"
}

END {
    print "};"
    print ""
    print "int pci_num_vendors = " NR ";";
}

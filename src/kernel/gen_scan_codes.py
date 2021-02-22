# Generate scan_codes.h from scan_codes.tsv

from functools import reduce

def symbolify(s):
    if not s.startswith("'"):
        return s.replace('+', 'PLUS').replace('*', 'TIMES').replace('/', 'DIV').replace('-', 'MINUS').replace('.', 'DOT')
    else: return s

def gen_set_entry(set, line):
    parts = line.split('\t')

    make_break = parts[set]
    [make, brk] = [[int(y, 16) for y in x.split(' ')] for x in make_break.replace('(base)', '').strip().split('/')]
    brk_pfx = brk[0]
    if not set == 1:
        brk = brk[1:]

    if make[0] == 0xe0 or make[0] == 0xe1:
        return

    [make, brk] = [reduce(lambda o, n: o << 8 | n, x, 0) for x in [make, brk]]

    ascii = str(parts[6] == 'yes').lower()
    symbol = symbolify(parts[7])
    upper_symbol = symbolify(parts[8])
    
    print(f"""\t[{make}] = {'{'} .ascii = {ascii}, .symbol = {symbol},
\t\t.up_symbol = {upper_symbol}, .prefix = {brk_pfx} {'}'},""")

    if set == 1:
        print(f"""\t[{brk}] = {'{'} .ascii = {ascii}, .symbol = {symbol},
\t\t.up_symbol = {upper_symbol}, .prefix = {brk_pfx}, .brk = true {'}'},""")

def gen_set(lines, set):
    # NOTE will only work with scan code set 3
    print("const struct kbd_scan_codes scan_code_set_" + str(set) + "[0xff] =\n{")
    for l in lines:
        gen_set_entry(set, l)
    print("};")

with open('scan_codes.tsv', 'r') as f:
    lines = f.read().split('\n')[1:]
    print('#include "kbd.h"\n\n')
    gen_set(lines, 1)
    print('\n')
    gen_set(lines, 3)


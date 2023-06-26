# elf_tool.py
import sys
import argparse
from elftools.elf.elffile import ELFFile
from elftools.elf.segments import Segment

def dump_handler(args):
    with open(args.filename, 'rb') as elf_file:
        e = ELFFile(elf_file)
        for sec in e.iter_sections():
            if sec.name == args.section_name:
                data = sec.data()
                print('char data[] = {', end='')
                for b in data:
                    print(hex(b), end=', ')
                print('}')
                return

if __name__ == '__main__':
    parser = argparse.ArgumentParser('ELF utility for Kalorona-Kernel Development.')
    subparser = parser.add_subparsers()

    p = subparser.add_parser('dump', help='Dump data in certain section of given ELF file.')
    p.add_argument('filename', help='Filename of the ELF file.')
    p.add_argument('--style', default='char-array', dest='style', help='Set the style of output: char-array.')
    p.add_argument('--section', default='.text', dest='section_name', help='The name of the section to output.')
    p.set_defaults(func=dump_handler)

    arguments = parser.parse_args()
    arguments.func(arguments)

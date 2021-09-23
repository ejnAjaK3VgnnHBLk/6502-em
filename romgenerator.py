# Thanks to Ben Eater for supplying this rather rudimentary code!
rom = bytearray([0xea] * 1028 * 64)

rom[0xfffc] = 0x0
rom[0xfffd] = 0x0
rom[0x0000] = 0x38 # Opcode for set carry flag
rom[0x0001] = 0x18 # Opcode for clear carry flag

with open("test.bin", "wb") as out_file:
    out_file.write(rom)

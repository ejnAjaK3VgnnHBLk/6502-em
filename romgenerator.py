# Thanks to Ben Eater for supplying this rather rudimentary code!
rom = bytearray([0xea] * 1028 * 64)

rom[0xfffc] = 0x0
rom[0xfffd] = 0x0


prog = [
        # ADD ONE PLUS TWO
        0x18,                   # clc           - clear carry flag
        0xD8,                   # cld           - clear decimal flag

        0xA9, 0x01,             # lda #01       - load  #01     -> accumulator
        0x8D, 0x00, 0x61,       # sta 0x6100    - store acc     -> $0x6000
        0xA9, 0x02,             # lda #02       - load  #02     -> accumulator
        0x8D, 0x01, 0x61,       # sta 0x6101    - store acc     -> $0x6101

        0xAD, 0x00, 0x61,       # lda 0x6100    - load  $0x6100 -> accumulator
        0x6D, 0x01, 0x61,       # adc 0x6101    - add   $0x6101 -> accumulator
        0x8D, 0x02, 0x61        # sta 0x6102    - store acc     -> mem[0x6102]        
]

i = 0
for element in prog:
    rom[i] = element
    i+=1

#rom[0x0000] = 0x38 # Opcode for set carry flag
#rom[0x0001] = 0x18 # Opcode for clear carry flag

with open("test.bin", "wb") as out_file:
    out_file.write(rom)

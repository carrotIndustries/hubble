import sys
import struct
import binascii

info_base = 0x1a0
sig_expected = 0x1badb002

revbits = lambda z, bits: sum(((z&(1<<i))!=0)*(1<<(bits-1-i)) for i in range(bits))
rev32 = lambda z: revbits(z, 32)
rev8 = lambda z: revbits(z, 8)


with open(sys.argv[1], "rb") as infile:
	rom = infile.read()
	sig = struct.unpack("I", rom[info_base:info_base+4])[0]
	if sig != sig_expected :
		raise ValueError("wrong sig")
	inf = list(struct.pack("II22s", sig_expected, len(rom), b"hello"))
	rom = list(rom)
	rom[info_base:info_base+len(inf)] = inf
	rom = bytes(rom)
	crc = rev32(binascii.crc32(bytes(rev8(x) for x in rom))^0xffffffff)
	print(hex(crc))
	with open(sys.argv[2], "wb") as outfile :
		outfile.write(rom)
		outfile.write(struct.pack("I", crc))

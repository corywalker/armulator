/usr/local/arm-elf/bin/arm-elf-as -o test.o test.S
/usr/local/arm-elf/bin/arm-elf-ld -e 0 -Ttext=0 -o test.elf test.o
/usr/local/arm-elf/bin/arm-elf-objcopy -O binary test.elf test.bi

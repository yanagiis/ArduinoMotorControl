project('water', 'c')
elf = executable('main', './src/main.c')

objcopy = find_program('avr-objcopy')
avrdude = find_program('avrdude')

hex = custom_target('hex', input: elf, output: 'main.hex',
  command: [
    objcopy, '-O', 'ihex', '-j', '.text', '-j', '.data', '@INPUT@', '@OUTPUT@'
  ],
  depends: elf
)

run_target('ardup',
  command : [avrdude,
    '-v', '-v', '-v', '-v',
    '-patmega328p', '-carduino', '-P/dev/ttyUSB0', '-b57600',
    '-D', '-Uflash:w:@0@:i'.format(hex.full_path())],
  depends : hex,
)

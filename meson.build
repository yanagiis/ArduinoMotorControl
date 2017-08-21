project('water', 'c')

objcopy = find_program('avr-objcopy')
avrdude = find_program('avrdude')

option_board = get_option('board')
if option_board == 'arduino pro mini'
  c_args = ['-DF_CPU=16000000L', '-D__AVR_ATmega328P__', '-mmcu=atmega328p']
  add_project_link_arguments('-mmcu=atmega328p', language: 'c')
else
  error('Please setting board option in meson_options.txt')
endif

srcs = []
include_dirs = []
subdir('src')
elf = executable('main', srcs, c_args: c_args, include_directories: include_dirs)

hex = custom_target('hex', input: elf, output: 'main.hex',
  command: [
    objcopy, '-O', 'ihex', '-j', '.text', '-j', '.data', '@INPUT@', '@OUTPUT@'
  ],
  depends: elf
)

run_target('flash',
  command : [avrdude,
    '-patmega328p', '-carduino', '-P/dev/ttyUSB0', '-b57600',
    '-D', '-Uflash:w:@0@:i'.format(hex.full_path())],
  depends : hex,
)

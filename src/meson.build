srcs += files([
    'main.c',
    'motor.c',
    'step_command.c',
    'step_tick.c',
    'hcode.c'
])

option_board = get_option('board')
if option_board == 'arduino pro mini'
  include_dirs += include_directories('atmega328p', '.')
  c_args += ['-DUSART0_ENABLED']
  srcs += files([
    './atmega328p/gpio.c',
    './atmega328p/timer.c',
    './atmega328p/uart.c'
  ])
endif

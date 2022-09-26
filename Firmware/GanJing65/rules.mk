# MCU name
MCU = atmega32u4

# Processor frequency
F_CPU = 16000000

# Bootloader selection
BOOTLOADER = atmel-dfu

# Build Options
# Change yes to no to disable
BOOTMAGIC_ENABLE = lite     # Enable Bootmagic Lite
MOUSEKEY_ENABLE = no        # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = no         # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration

# Do not enable SLEEP_LED_ENABLE. it uses the same timer as BACKLIGHT_ENABLE
SLEEP_LED_ENABLE = no       # Breathing sleep LED during USB suspend

# If this doesn't work, see here: https://github.com/tmk/tmk_keyboard/wiki/FAQ#nkro-doesnt-work
NKRO_ENABLE = no            # USB Nkey Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
AUDIO_ENABLE = no           # Audio output
ENCODER_ENABLE = yes		# Enable use of rotory knob
OLED_ENABLE = yes			# Enable use of OLED display
WPM_ENABLE = yes			# Calculates the words per minute as a rolling average of your keystrokes and outputs as an unsigned integer

EXTRAFLAGS += -flto			# To save on firmware space

LAYOUTS = solder hotswap

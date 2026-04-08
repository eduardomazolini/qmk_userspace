# Build Options
#   change yes to no to disable
#
VIA_ENABLE = yes
VIAL_ENABLE = yes

BOOTMAGIC_ENABLE = yes      # Enable Bootmagic Lite
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = no         # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration
NKRO_ENABLE = yes           # Enable N-Key Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow 
                            # usando Matrix tem que desabilitar
AUDIO_ENABLE = no           # Audio output
ENCODER_ENABLE = yes
OLED_ENABLE = yes

SPLIT_KEYBOARD = yes
ENCODER_MAP_ENABLE = yes
CONVERT_TO = rp2040_ce

#Eu adicionei daqui pra baixo
RGB_MATRIX_ENABLE = yes
RGB_MATRIX_DRIVER = ws2812
VIALRGB_ENABLE = yes

RGB_MATRIX_CUSTOM_USER = yes

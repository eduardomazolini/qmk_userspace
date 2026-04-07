# QMK Userspace

This is a template repository which allows for an external set of QMK keymaps to be defined and compiled. This is useful for users who want to maintain their own keymaps without having to fork the [main QMK repository](https://github.com/qmk/qmk_firmware). You must still fork the main QMK repository if writing firmware for a *new* keyboard.

## Howto configure your build targets

1. Run the normal `qmk setup` procedure if you haven't already done so -- see [QMK Docs](https://docs.qmk.fm/#/newbs) for details.
1. Fork this repository
1. Clone your fork to your local machine
1. Enable userspace in QMK config using `qmk config user.overlay_dir="$(realpath qmk_userspace)"`
1. Add a new keymap for your board using `qmk new-keymap`
    * This will create a new keymap in the `keyboards` directory, in the same location that would normally be used in the main QMK repository. For example, if you wanted to add a keymap for the Planck, it will be created in `keyboards/planck/keymaps/<your keymap name>`
    * You can also create a new keymap using `qmk new-keymap -kb <your_keyboard> -km <your_keymap>`
    * Alternatively, add your keymap manually by placing it in the location specified above.
    * `layouts/<layout name>/<your keymap name>/keymap.*` is also supported if you prefer the layout system
1. Add your keymap(s) to the build by running `qmk userspace-add -kb <your_keyboard> -km <your_keymap>`
    * This will automatically update your `qmk.json` file
    * Corresponding `qmk userspace-remove -kb <your_keyboard> -km <your_keymap>` will delete it
    * Listing the build targets can be done with `qmk userspace-list`
1. Commit your changes

## Howto build with GitHub

1. In the GitHub Actions tab, enable workflows
1. Push your changes above to your forked GitHub repository
1. Look at the GitHub Actions for a new actions run
1. Wait for the actions run to complete
1. Inspect the Releases tab on your repository for the latest firmware build

## Howto build locally

1. Run the normal `qmk setup` procedure if you haven't already done so -- see [QMK Docs](https://docs.qmk.fm/#/newbs) for details.
1. Fork this repository
1. Clone your fork to your local machine
1. `cd` into this repository's clone directory
1. Set global userspace path: `qmk config user.overlay_dir="$(realpath .)"` -- you MUST be located in the cloned userspace location for this to work correctly
    * This will be automatically detected if you've `cd`ed into your userspace repository, but the above makes your userspace available regardless of your shell location.
1. Compile normally: `qmk compile -kb your_keyboard -km your_keymap` or `make your_keyboard:your_keymap`

Alternatively, if you configured your build targets above, you can use `qmk userspace-compile` to build all of your userspace targets at once.

## Extra info

If you wish to point GitHub actions to a different repository, a different branch, or even a different keymap name, you can modify `.github/workflows/build_binaries.yml` to suit your needs.

To override the `build` job, you can change the following parameters to use a different QMK repository or branch:
```
    with:
      qmk_repo: qmk/qmk_firmware
      qmk_ref: master
```

If you wish to manually manage `qmk_firmware` using git within the userspace repository, you can add `qmk_firmware` as a submodule in the userspace directory instead. GitHub Actions will automatically use the submodule at the pinned revision if it exists, otherwise it will use the default latest revision of `qmk_firmware` from the main repository.

This can also be used to control which fork is used, though only upstream `qmk_firmware` will have support for external userspace until other manufacturers update their forks.

1. (First time only) `git submodule add https://github.com/qmk/qmk_firmware.git`
1. (To update) `git submodule update --init --recursive`
1. Commit your changes to your userspace repository

# MEU AVALANCHE

## rules.mk

```
CONVERT_TO = rp2040_ce

RGB_MATRIX_ENABLE = yes
RGB_MATRIX_DRIVER = ws2812
VIALRGB_ENABLE = yes
```

## config.h

```
#define RGB_MATRIX_LED_COUNT 64 // informa o total de leds
#define RGB_MATRIX_SPLIT { 32, 32 } // sem isso o segundo teclado não funciona.
#define SPLIT_LAYER_STATE_ENABLE // para sinalizar layer alterado
#define SPLIT_LED_STATE_ENABLE  // para sinalizar caps, num ...
#define SPLIT_TRANSPORT_MIRROR
#define SPLIT_MODS_ENABLE

```

## Mapa de Leds

### LEDs

```
        {27, 26,  18, 17,  8,  7, 255},
        {28, 25,  19, 16,  9,  6, 255},
        {29, 24,  20, 15, 10,  5,  2},
        {30, 23,  21, 14, 11,  4,  3},
        {31, 255, 22, 13, 12,  0,  1},

        {59,  58, 50, 49, 40, 39, 255},
        {60,  57, 51, 48, 41, 38, 255},
        {61,  56, 52, 47, 42, 37,  34},
        {62,  55, 53, 46, 43, 36,  35},
        {63, 255, 54, 45, 44, 32,  33}
```

### Coordenadas

```
y 0-64
4, 4, 1, 0, 1, 3 ,xx
16,16,13,12,13,15,xx
28,28,25,24,15,27,54
40,40,37,36,27,39,46
28,xx,49,49,52,58,64

x = 0 106
12,24,36,48,60,72,XX
12,24,36,48,60,72,XX
12,24,36,48,60,72,106
12,24,36,48,60,72,90
00,xx,42,56,68,84,98

x = 224 118

212,200,188,176,164,152,xxx
212,200,188,176,164,152,xxx 
212,200,188,176,164,152,118
212,200,188,176,164,152,134
224,xxx,180,168,156,140,126
```

# TODO

- Efeito "Direct Control" não funciona.
- CAPS-LOCK INVERTE
- acentos ficam em maiúscula

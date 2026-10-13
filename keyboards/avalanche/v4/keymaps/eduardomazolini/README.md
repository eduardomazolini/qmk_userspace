# Avalanche v4, keymap `eduardomazolini`

Avalanche v4 com controlador **RP2040** (`CONVERT_TO = rp2040_ce`), Vial, RGB Matrix com efeito próprio
(`MY_COOL_EFFECT`), OLED e dois knobs.

## Um firmware para cada lado

O cabo USB fica **sempre na metade esquerda**. Por isso há um firmware para cada metade: a esquerda é
sempre a mestre e a direita sempre a escrava, sem nenhuma detecção na hora de ligar.

O firmware único do QMK usa `SPLIT_USB_DETECT`: a metade espera o PC conversar pela USB (2,5 s) para
decidir se é mestre. Ao voltar da suspensão, quando o PC demorava, a metade com o cabo decidia que era
escrava, chamava `usb_disconnect()` e apagava até ser replugada. Com o lado fixo, isso não acontece. A
escolha é feita em `is_keyboard_master_impl()`/`is_keyboard_left_impl()`, no fim do `keymap.c`, conforme a
variável `LADO` (`rules.mk`).

Também ficam ligados `SPLIT_WATCHDOG_ENABLE` e `USB_SUSPEND_WAKEUP_DELAY 200` (`config.h`).

## Compilar

```sh
qmk compile -kb avalanche/v4 -km eduardomazolini -e LADO=esquerdo
cp .build/avalanche_v4_eduardomazolini.uf2 avalanche_v4_eduardomazolini_esquerdo.uf2

qmk compile -kb avalanche/v4 -km eduardomazolini -e LADO=direito
cp .build/avalanche_v4_eduardomazolini.uf2 avalanche_v4_eduardomazolini_direito.uf2
```

Rode na pasta do `vial-qmk` (ou do `qmk_firmware`) configurado com este userspace
(`qmk config user.overlay_dir=...`).

- Copie o `.uf2` **logo depois de cada compilação**: os dois lados geram um arquivo com o mesmo nome.
- Sem `-e LADO=...`, sai o firmware único, com a detecção automática do QMK. Ele funciona com o cabo
  em qualquer lado, mas tem o problema da suspensão.

## Gravar

**Antes de gravar, salve o mapa no Vial (File → Save current layout).** O vial-qmk sorteia um `BUILD_ID` a
cada compilação, e todo firmware novo volta o teclado ao mapa do `keymap.c`. Depois de gravar, use
**File → Load saved layout**.

Para cada metade:

1. Plugue o cabo USB **nela** e dê **dois toques no botão de reset** da placa. Aparece o disco `RPI-RP2`.
2. *(Opcional, recomendado)* Guarde a memória atual. Se esqueceu o `.vil`, é dela que o mapa pode ser
   recuperado com o [`recuperar-vial`](../../../../../recuperar-vial/README.md):
   ```sh
   sudo picotool save -r 0x10000000 0x10200000 backup-esquerda.uf2
   ```
3. Grave o firmware daquele lado. Copie o `.uf2` para o disco `RPI-RP2` ou use:
   ```sh
   sudo picotool load -x avalanche_v4_eduardomazolini_esquerdo.uf2
   ```

Depois das duas metades, **volte o cabo para a esquerda**. Plugada sozinha, a direita não aparece no PC,
e isso é o esperado.

Nunca conecte ou desconecte o cabo entre as metades (TRRS) com o USB plugado.

# Recuperar o mapa do Vial a partir de um backup da memória

Gravou um firmware novo e as teclas que você tinha mudado pelo Vial voltaram ao padrão? Se existir uma
**cópia da memória do teclado feita antes da gravação**, o `recuperar_vial.py` tira de lá o mapa
(teclas de todas as camadas e knobs). Depois você o devolve ao teclado sem regravar o firmware: pelo
próprio Vial (arquivo `.vil`) ou direto pelo script.

Funciona com qualquer teclado QMK com Vial. O script descobre sozinho o tamanho da matriz, o número de
camadas e de knobs, e onde o mapa está guardado.

## Por que o mapa some

O Vial guarda o que você muda na EEPROM do teclado, e não no firmware. Ao ligar, o firmware confere um
número mágico gravado ali:

- no **vial-qmk**, esse número é o `BUILD_ID`, sorteado a cada compilação (`util/build_id.py`);
- no VIA comum, é a data da compilação.

Se o número não bate, o firmware considera a EEPROM inválida e volta ao mapa do `keymap.c`. Na prática,
**todo firmware recompilado e gravado zera o que foi feito pelo Vial**, mesmo que o código não tenha mudado.

**Prevenção:** antes de gravar, abra o Vial e use **File → Save current layout**. Depois da gravação,
**File → Load saved layout**. Este script serve para quando isso não foi feito, mas existe um backup.

## O que precisa

- Linux com Python 3. O script só usa a biblioteca padrão.
- O teclado plugado, para descobrir a geometria e comparar o mapa. Dá para usar sem ele: veja
  [Sem o teclado plugado](#sem-o-teclado-plugado).
- Permissão no `/dev/hidraw*` do teclado. É a mesma regra udev que o Vial pede
  ([manual oficial](https://get.vial.today/manual/linux-udev.html)):

  ```sh
  export USER_GID=`id -g`; sudo --preserve-env=USER_GID sh -c 'echo "KERNEL==\"hidraw*\", SUBSYSTEM==\"hidraw\", ATTRS{serial}==\"*vial:f64c2b3c*\", MODE=\"0660\", GROUP=\"$USER_GID\", TAG+=\"uaccess\", TAG+=\"udev-acl\"" > /etc/udev/rules.d/99-vial.rules && udevadm control --reload && udevadm trigger'
  ```

## 1. Fazer o backup (antes de gravar o firmware)

O backup precisa incluir a área onde fica a EEPROM. O jeito mais simples é copiar a **flash inteira**.
Em teclado dividido, copie a metade que fica com o cabo USB.

### RP2040 (Raspberry Pi Pico, Elite-Pi, Liatris, Helios, KB2040…)

1. Coloque a placa em modo de gravação (BOOTSEL). Conforme a placa: dois toques rápidos no reset,
   segurar BOOT ao plugar, ou a tecla de bootmagic (geralmente a do canto superior esquerdo) ao plugar.
   Aparece o disco `RPI-RP2` e o dispositivo USB `2e8a:0003`.
2. Copie a flash com o [`picotool`](https://github.com/raspberrypi/picotool) (no Debian/Ubuntu:
   `sudo apt install picotool`):

   ```sh
   sudo picotool save -r 0x10000000 0x10200000 backup.uf2
   ```

   Isso copia os primeiros 2 MB, que é o tamanho que o QMK assume para o RP2040
   (`PICO_FLASH_SIZE_BYTES`), mesmo que o chip tenha mais. A EEPROM fica nos últimos 8 KB desses 2 MB.
   Se o firmware do seu teclado define outro tamanho, copie até esse limite, ou copie tudo com
   `picotool save -a` e informe `--tamanho-flash` ao script.
3. Se for gravar agora, grave o firmware novo a seguir. Para guardar o backup também é preciso estar em
   BOOTSEL, então faça os dois na mesma ida ao modo de gravação.

### STM32 com bootloader DFU

```sh
dfu-util -a 0 -s 0x08000000:<tamanho-da-flash> -U backup.bin
```

A EEPROM emulada fica em setores da flash (driver `wear_leveling_efl`). O script já a procura nos
últimos `--tamanho-area` bytes, mas o lugar exato e o tamanho de escrita (`--tamanho-escrita`, que vai de
2 a 8 conforme o chip) variam. Confira em `platforms/chibios/drivers/wear_leveling/wear_leveling_efl_config.h`
do seu QMK. Placas STM32 que usam o driver antigo `eeprom_stm32` (sem *wear leveling*) **não** são suportadas.

### AVR (ATmega32u4: Pro Micro, Elite-C…)

O AVR tem EEPROM de verdade. Copie só ela e use `--bruto` no script:

```sh
avrdude -p atmega32u4 -c avr109 -P /dev/ttyACM0 -U eeprom:r:eeprom.bin:r    # bootloader Caterina
dfu-programmer atmega32u4 read --eeprom --bin > eeprom.bin                   # bootloader Atmel DFU
```

## 2. Extrair o mapa

Com o teclado já rodando o firmware novo e plugado:

```sh
python3 recuperar_vial.py vil backup.uf2 -o recuperado.vil
```

Exemplo de saída:

```
Área da EEPROM: 0x101fe000 (8192 bytes); cópia consolidada vazia/inválida (usado só o log); 517 entradas no log.
Mapa encontrado no offset 0x29: 266 de 280 teclas iguais às atuais.
Layout salvo em recuperado.vil. No Vial: File → Load saved layout.
```

Leia a linha "Mapa encontrado". Como o script compara o backup com o mapa atual, o normal é que **a
maioria das teclas bata**: só as que você mudou pelo Vial ficam diferentes. Se menos da metade bater, ele
avisa. Nesse caso, confira os parâmetros da área ou informe `--offset`.

Para só reconstruir a EEPROM e examinar à mão (por exemplo, com `xxd`):

```sh
python3 recuperar_vial.py eeprom backup.uf2 -o eeprom.bin
```

### Parâmetros da área (só se o padrão não servir)

| Opção | Padrão | No QMK |
|---|---|---|
| `--tamanho-flash` | tamanho do backup | `PICO_FLASH_SIZE_BYTES` (RP2040) |
| `--tamanho-area` | 8192 | `WEAR_LEVELING_BACKING_SIZE` |
| `--tamanho-logico` | metade da área | `WEAR_LEVELING_LOGICAL_SIZE` |
| `--tamanho-escrita` | 2 | `BACKING_STORE_WRITE_SIZE` |
| `--inicio` | `tamanho-flash − tamanho-area` | `WEAR_LEVELING_RP2040_FLASH_BASE` / setor do EFL |
| `--sem-inverter` | (inverte) | flash que apaga para `0x00`, raro |
| `--bruto` | — | o backup já é a EEPROM (AVR) |

Esses valores ficam no `config.h` da placa ou do teclado, ou nos `*_config.h` de
`platforms/chibios/drivers/wear_leveling/`.

## 3. Redefinir as teclas

Escolha um dos dois caminhos.

**a) Pelo Vial (recomendado para conferir antes).** Abra o Vial, use **File → Load saved layout** e
escolha o `recuperado.vil`. Se o Vial disser que o layout é de outro teclado, foi gerado com `--sem-teclado`
sem `--uid`. Pode confirmar se for o mesmo teclado. Depois salve um `.vil` novo com **File → Save current layout**.

**b) Direto pelo script**, gravando só o que difere (teclas e knobs):

```sh
python3 recuperar_vial.py aplicar backup.uf2 --simular   # mostra o que mudaria
python3 recuperar_vial.py aplicar backup.uf2             # grava
```

Nenhum dos dois regrava o firmware. Usam os mesmos comandos do VIA/Vial que o aplicativo usa ao trocar
uma tecla, e não precisam desbloquear o Vial.

## Sem o teclado plugado

Informe a geometria e o offset (o padrão do vial-qmk atual costuma ser `0x29`, mas varia com a versão e
os recursos do firmware):

```sh
python3 recuperar_vial.py vil backup.uf2 -o recuperado.vil --sem-teclado \
    --camadas 4 --linhas 10 --colunas 7 --encoders 2 --offset 0x29
```

As linhas e colunas são as da **matriz** (`matrix_size` / `MATRIX_ROWS`, `MATRIX_COLS`; num teclado
dividido, as linhas das duas metades somadas), e não as do desenho do teclado.

## O que é recuperado e o que não é

- **Recuperado:** teclas de todas as camadas e knobs (*encoders*).
- **Não recuperado:** macros, tap dance, combos, *key overrides* e as "QMK Settings". Eles também estão
  no backup (`eeprom` gera o arquivo), mas a posição deles depende da versão e das opções do firmware.
  Refaça pelo Vial ou extraia à mão, seguindo `quantum/nvm/eeprom/nvm_dynamic_keymap.c` do seu vial-qmk.

## Como funciona

1. **EEPROM emulada.** RP2040 e STM32 não têm EEPROM, e o QMK a emula na flash com *wear leveling*
   (`quantum/wear_leveling/wear_leveling.c`). A área tem uma cópia consolidada, uma soma FNV-1a de
   64 bits e um log de escritas. O script refaz a cópia e reaplica o log, como o firmware faz ao ligar.
   Em flash que apaga para `0xFF`, tudo é gravado com os bits invertidos.
2. **Mapa.** O VIA guarda o mapa como `camadas × linhas × colunas` códigos de 16 bits *big-endian*, a
   partir de `DYNAMIC_KEYMAP_EEPROM_ADDR`, logo depois das configurações do QMK e do VIA. Esse endereço
   muda com a versão do firmware. Por isso o script procura a posição em que o backup mais se parece
   com o mapa atual do teclado.
3. **Knobs.** No vial-qmk, os knobs ficam logo depois do mapa: para cada camada e knob, 2 bytes
   (sentido horário) + 2 bytes (anti-horário). No `.vil`, a ordem é `[anti-horário, horário]`.
4. **Teclado.** A geometria vem da definição que o próprio teclado envia ao Vial (JSON comprimido com
   LZMA). As leituras e gravações usam o protocolo VIA/Vial pela interface *raw HID* (usage page `0xFF60`).

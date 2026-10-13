#!/usr/bin/env python3
"""Recupera o mapa do Vial (teclas e knobs) a partir de uma cópia da memória do teclado.

Uso rápido (veja o README.md desta pasta):
    python3 recuperar_vial.py eeprom  backup.uf2 -o eeprom.bin     # só reconstrói a EEPROM
    python3 recuperar_vial.py vil     backup.uf2 -o recuperado.vil  # gera um .vil para abrir no Vial
    python3 recuperar_vial.py aplicar backup.uf2 --simular          # mostra/grava as diferenças no teclado

Só usa a biblioteca padrão do Python. A conversa com o teclado usa /dev/hidraw* (Linux).
"""
import argparse
import glob
import json
import lzma
import os
import select
import sys

UF2_MAGIC = b"UF2\n"
FNV_INICIO, FNV_PRIMO = 0xCBF29CE484222325, 0x100000001B3


# ───────────────────────── leitura do backup ─────────────────────────

def ler_imagem(caminho):
    """Devolve (endereço inicial, bytes) de um .uf2 (picotool/UF2) ou de um binário puro."""
    dados = open(caminho, "rb").read()
    if not dados.startswith(UF2_MAGIC):
        return 0, dados
    blocos = {}
    for i in range(0, len(dados), 512):
        b = dados[i:i + 512]
        if b[:4] != UF2_MAGIC:
            continue
        endereco = int.from_bytes(b[12:16], "little")
        tamanho = int.from_bytes(b[16:20], "little")
        blocos[endereco] = b[32:32 + tamanho]
    inicio = min(blocos)
    fim = max(a + len(v) for a, v in blocos.items())
    imagem = bytearray(b"\xff" * (fim - inicio))
    for a, v in blocos.items():
        imagem[a - inicio:a - inicio + len(v)] = v
    return inicio, bytes(imagem)


def fnv1a64(dados):
    h = FNV_INICIO
    for c in dados:
        h = ((h ^ c) * FNV_PRIMO) & 0xFFFFFFFFFFFFFFFF
    return h


def reconstruir_eeprom(area, logico, escrita, inverter=True):
    """Refaz a EEPROM emulada pelo "wear leveling" do QMK (quantum/wear_leveling/wear_leveling.c).

    A área tem: cópia consolidada (logico bytes) + FNV-1a de 64 bits (8 bytes) + log de escritas.
    Em flash que apaga para 0xFF o QMK grava tudo invertido (~valor).
    """
    if inverter:
        area = bytes(b ^ 0xFF for b in area)
    cache = bytearray(area[:logico])
    soma = int.from_bytes(area[logico:logico + 8], "little")
    consolidada_ok = fnv1a64(cache) == soma
    if not consolidada_ok:
        cache = bytearray(logico)  # o firmware faz o mesmo: descarta a cópia e usa só o log

    a, entradas, problema = logico + 8, 0, None
    while a + escrita <= len(area):
        palavra = area[a:a + escrita]
        if not any(palavra):
            break  # posição vazia: fim do log
        a += escrita
        e = bytearray(8)
        e[:escrita] = palavra
        tipo = e[0] >> 6
        if tipo == 0:  # vários bytes
            if escrita == 2:
                e[2:4] = area[a:a + 2]
                a += 2
            endereco = ((e[0] & 0x07) << 16) | (e[1] << 8) | e[2]
            comprimento = (e[0] >> 3) & 0x07
            if endereco + comprimento > logico:
                problema = f"entrada fora da área em 0x{a:x}"
                break
            if escrita == 2:
                if comprimento > 1:
                    e[4:6] = area[a:a + 2]
                    a += 2
                if comprimento > 3:
                    e[6:8] = area[a:a + 2]
                    a += 2
            elif escrita == 4 and comprimento > 1:
                e[4:8] = area[a:a + 4]
                a += 4
            cache[endereco:endereco + comprimento] = e[3:3 + comprimento]
        elif tipo == 1 and escrita == 2:  # 1 byte em endereço < 64
            cache[e[0] & 0x3F] = e[1]
        elif tipo == 2 and escrita == 2:  # palavra 0 ou 1
            endereco = ((e[0] & 0x1F) << 9) | (e[1] << 1)
            cache[endereco] = (e[0] >> 5) & 1
            cache[endereco + 1] = 0
        else:
            problema = f"tipo de entrada desconhecido em 0x{a:x}"
            break
        entradas += 1
    return bytes(cache), consolidada_ok, entradas, problema


def obter_eeprom(args):
    """Lê o backup e devolve a EEPROM lógica, conforme as opções da linha de comando."""
    base, imagem = ler_imagem(args.backup)
    if args.bruto:
        print(f"Backup tratado como EEPROM pura ({len(imagem)} bytes).", file=sys.stderr)
        return imagem
    area_tam = args.tamanho_area
    logico = args.tamanho_logico or area_tam // 2
    tam_flash = args.tamanho_flash or len(imagem)
    inicio = args.inicio if args.inicio is not None else tam_flash - area_tam
    if inicio < 0 or inicio + area_tam > len(imagem):
        sys.exit(f"A área da EEPROM (0x{inicio:x}..0x{inicio + area_tam:x}) não cabe no backup de {len(imagem)} bytes. "
                 "Confira --tamanho-flash/--inicio ou se o backup cobre a flash inteira.")
    area = imagem[inicio:inicio + area_tam]
    eeprom, ok, entradas, problema = reconstruir_eeprom(area, logico, args.tamanho_escrita, not args.sem_inverter)
    print(f"Área da EEPROM: 0x{base + inicio:08x} ({area_tam} bytes); cópia consolidada "
          f"{'válida' if ok else 'vazia/inválida (usado só o log)'}; {entradas} entradas no log.", file=sys.stderr)
    if problema:
        print(f"Aviso: log interrompido ({problema}). Confira --tamanho-escrita e --inicio.", file=sys.stderr)
    return eeprom


# ───────────────────────── conversa com o teclado (VIA/Vial) ─────────────────────────

class Teclado:
    """Fala com a interface "raw HID" do VIA/Vial (usage page 0xFF60) por /dev/hidraw*."""

    def __init__(self, dispositivo=None):
        if dispositivo is None:
            achados = []
            for h in sorted(glob.glob("/sys/class/hidraw/*")):
                try:
                    descritor = open(h + "/device/report_descriptor", "rb").read()
                except OSError:
                    continue
                if b"\x06\x60\xff" in descritor:  # Usage Page 0xFF60 = raw HID do QMK
                    achados.append("/dev/" + os.path.basename(h))
            if not achados:
                sys.exit("Nenhum teclado VIA/Vial encontrado em /dev/hidraw*. Ele está plugado?")
            if len(achados) > 1:
                sys.exit(f"Mais de um teclado VIA/Vial: {', '.join(achados)}. Escolha com --dispositivo.")
            dispositivo = achados[0]
        try:
            self.fd = os.open(dispositivo, os.O_RDWR)
        except PermissionError:
            sys.exit(f"Sem permissão em {dispositivo}. Instale a regra udev do Vial (veja o README.md).")
        self.dispositivo = dispositivo

    def cmd(self, *dados):
        pacote = bytes(dados)
        os.write(self.fd, pacote + b"\x00" * (32 - len(pacote)))
        if not select.select([self.fd], [], [], 2)[0]:
            sys.exit("O teclado não respondeu.")
        return os.read(self.fd, 32)

    def uid(self):
        return int.from_bytes(self.cmd(0xFE, 0x00)[4:12], "little")

    def camadas(self):
        return self.cmd(0x11)[1]

    def definicao(self):
        tamanho = int.from_bytes(self.cmd(0xFE, 0x01)[:4], "little")
        dados = b""
        pagina = 0
        while len(dados) < tamanho:
            dados += self.cmd(0xFE, 0x02, pagina & 0xFF, pagina >> 8)
            pagina += 1
        return json.loads(lzma.decompress(dados[:tamanho]))

    def opcoes_layout(self):
        return int.from_bytes(self.cmd(0x02, 0x02)[2:6], "big")

    def tecla(self, camada, linha, coluna):
        r = self.cmd(0x04, camada, linha, coluna)
        return (r[4] << 8) | r[5]

    def gravar_tecla(self, camada, linha, coluna, codigo):
        self.cmd(0x05, camada, linha, coluna, codigo >> 8, codigo & 0xFF)

    def encoder(self, camada, indice):
        """Devolve (anti-horário, horário), a mesma ordem do .vil."""
        r = self.cmd(0xFE, 0x03, camada, indice)
        return (r[0] << 8) | r[1], (r[2] << 8) | r[3]

    def gravar_encoder(self, camada, indice, sentido, codigo):
        self.cmd(0xFE, 0x04, camada, indice, sentido, codigo >> 8, codigo & 0xFF)


def contar_encoders(definicao):
    """Conta os knobs no layout KLE da definição do Vial (rótulo com "e" na 10ª linha)."""
    indices = set()
    for linha in definicao.get("layouts", {}).get("keymap", []):
        for item in linha:
            if isinstance(item, str):
                partes = item.split("\n")
                if len(partes) > 9 and partes[9] == "e":
                    indices.add(int(partes[0].split(",")[0]))
    return max(indices) + 1 if indices else 0


# ───────────────────────── mapa dentro da EEPROM ─────────────────────────

def geometria(args, teclado):
    """Camadas, linhas, colunas e knobs: da linha de comando ou perguntando ao teclado."""
    if teclado is not None:
        d = teclado.definicao()
        g = {"camadas": teclado.camadas(), "linhas": d["matrix"]["rows"], "colunas": d["matrix"]["cols"],
             "encoders": contar_encoders(d), "tem_opcoes": bool(d.get("layouts", {}).get("labels"))}
    else:
        g = {}
    for chave in ("camadas", "linhas", "colunas", "encoders"):
        valor = getattr(args, chave)
        if valor is not None:
            g[chave] = valor
        elif chave not in g:
            sys.exit(f"Informe --{chave} (ou deixe o teclado plugado para descobrir sozinho).")
    return g


def ler_mapa(eeprom, offset, g):
    mapa = {}
    for c in range(g["camadas"]):
        for li in range(g["linhas"]):
            for co in range(g["colunas"]):
                p = offset + 2 * ((c * g["linhas"] + li) * g["colunas"] + co)
                mapa[c, li, co] = int.from_bytes(eeprom[p:p + 2], "big")
    return mapa


def ler_encoders(eeprom, offset, g):
    """Knobs ficam logo depois do mapa (vial-qmk): por knob, 2 bytes horário + 2 bytes anti-horário."""
    inicio = offset + 2 * g["camadas"] * g["linhas"] * g["colunas"]
    enc = {}
    for c in range(g["camadas"]):
        for i in range(g["encoders"]):
            p = inicio + 4 * (c * g["encoders"] + i)
            horario = int.from_bytes(eeprom[p:p + 2], "big")
            anti = int.from_bytes(eeprom[p + 2:p + 4], "big")
            enc[c, i] = (anti, horario)
    return enc


def achar_offset(eeprom, g, atual):
    """Procura onde começa o mapa: o offset em que o backup mais se parece com o mapa atual do teclado."""
    tam = 2 * g["camadas"] * g["linhas"] * g["colunas"]
    melhor, pontos = None, -1
    for offset in range(0, min(len(eeprom) - tam, 1024) + 1):
        p = sum(1 for k, v in ler_mapa(eeprom, offset, g).items() if v == atual[k])
        if p > pontos:
            melhor, pontos = offset, p
    total = len(atual)
    print(f"Mapa encontrado no offset 0x{melhor:x}: {pontos} de {total} teclas iguais às atuais.", file=sys.stderr)
    if pontos < total // 2:
        print("Aviso: menos da metade bate. Talvez o teclado tenha voltado ao padrão com muitas mudanças; "
              "confira o resultado ou passe --offset.", file=sys.stderr)
    return melhor


def preparar(args):
    eeprom = obter_eeprom(args)
    teclado = None if args.sem_teclado else Teclado(args.dispositivo)
    g = geometria(args, teclado)
    if args.offset is not None:
        offset = args.offset
    elif teclado is not None:
        atual = {(c, li, co): teclado.tecla(c, li, co)
                 for c in range(g["camadas"]) for li in range(g["linhas"]) for co in range(g["colunas"])}
        offset = achar_offset(eeprom, g, atual)
    else:
        sys.exit("Sem o teclado plugado, informe --offset.")
    return eeprom, teclado, g, ler_mapa(eeprom, offset, g), ler_encoders(eeprom, offset, g)


# ───────────────────────── comandos ─────────────────────────

def cmd_eeprom(args):
    eeprom = obter_eeprom(args)
    open(args.saida, "wb").write(eeprom)
    print(f"EEPROM salva em {args.saida} ({len(eeprom)} bytes).")


def cmd_vil(args):
    eeprom, teclado, g, mapa, enc = preparar(args)
    vil = {
        "version": 1,
        "uid": args.uid if args.uid is not None else (teclado.uid() if teclado else 0),
        "layout": [[[mapa[c, li, co] for co in range(g["colunas"])] for li in range(g["linhas"])]
                   for c in range(g["camadas"])],
        "encoder_layout": [[list(enc[c, i]) for i in range(g["encoders"])] for c in range(g["camadas"])],
        # -1 = sem opções de layout, como o próprio Vial salva
        "layout_options": teclado.opcoes_layout() if teclado and g.get("tem_opcoes") else -1,
    }
    json.dump(vil, open(args.saida, "w"))
    print(f"Layout salvo em {args.saida}. No Vial: File → Load saved layout.")


def cmd_aplicar(args):
    eeprom, teclado, g, mapa, enc = preparar(args)
    if teclado is None:
        sys.exit("O comando aplicar precisa do teclado plugado.")
    mudancas = 0
    for (c, li, co), codigo in mapa.items():
        atual = teclado.tecla(c, li, co)
        if atual != codigo:
            print(f"camada {c} linha {li} coluna {co}: 0x{atual:04x} → 0x{codigo:04x}")
            if not args.simular:
                teclado.gravar_tecla(c, li, co, codigo)
            mudancas += 1
    for (c, i), (anti, horario) in enc.items():
        atual = teclado.encoder(c, i)
        for sentido, (antes, depois) in enumerate(zip(atual, (anti, horario))):
            if antes != depois:
                nome = ("anti-horário", "horário")[sentido]
                print(f"camada {c} knob {i} {nome}: 0x{antes:04x} → 0x{depois:04x}")
                if not args.simular:
                    teclado.gravar_encoder(c, i, sentido, depois)
                mudancas += 1
    if args.simular:
        print(f"{mudancas} diferença(s). Nada foi gravado (--simular).")
    else:
        print(f"{mudancas} posição(ões) regravada(s) no teclado.")


def numero(texto):
    return int(texto, 0)


def main():
    p = argparse.ArgumentParser(description="Recupera o mapa do Vial (teclas e knobs) a partir de um backup da memória do teclado.")
    sub = p.add_subparsers(dest="comando", required=True)

    def comuns(sp):
        sp.add_argument("backup", help="cópia da flash (.uf2 do picotool, .bin) ou da EEPROM (com --bruto)")
        sp.add_argument("--bruto", action="store_true", help="o backup já é a EEPROM pura (ex.: AVR lido com avrdude)")
        sp.add_argument("--tamanho-flash", type=numero, help="tamanho da flash que o firmware assume (padrão: tamanho do backup)")
        sp.add_argument("--tamanho-area", type=numero, default=8192, help="WEAR_LEVELING_BACKING_SIZE (padrão 8192)")
        sp.add_argument("--tamanho-logico", type=numero, help="WEAR_LEVELING_LOGICAL_SIZE (padrão: metade da área)")
        sp.add_argument("--tamanho-escrita", type=numero, default=2, choices=(2, 4, 8),
                        help="BACKING_STORE_WRITE_SIZE (padrão 2, o do RP2040)")
        sp.add_argument("--inicio", type=numero, help="posição da área no backup (padrão: tamanho-flash − tamanho-area)")
        sp.add_argument("--sem-inverter", action="store_true", help="flash que apaga para 0x00 (raro)")

    def teclado(sp):
        sp.add_argument("--dispositivo", help="/dev/hidrawN do teclado (padrão: procura sozinho)")
        sp.add_argument("--sem-teclado", action="store_true", help="não falar com o teclado (informe a geometria e o --offset)")
        sp.add_argument("--offset", type=numero, help="onde o mapa começa na EEPROM (padrão: descobre comparando com o teclado)")
        for nome in ("camadas", "linhas", "colunas", "encoders"):
            sp.add_argument(f"--{nome}", type=int)

    sp = sub.add_parser("eeprom", help="só reconstrói a EEPROM e salva em um arquivo")
    comuns(sp)
    sp.add_argument("-o", "--saida", required=True)
    sp.set_defaults(func=cmd_eeprom)

    sp = sub.add_parser("vil", help="gera um .vil para abrir no Vial (File → Load saved layout)")
    comuns(sp)
    teclado(sp)
    sp.add_argument("--uid", type=numero, help="uid do teclado no .vil (padrão: lido do teclado)")
    sp.add_argument("-o", "--saida", required=True)
    sp.set_defaults(func=cmd_vil)

    sp = sub.add_parser("aplicar", help="grava no teclado as teclas e knobs que diferem do backup")
    comuns(sp)
    teclado(sp)
    sp.add_argument("--simular", action="store_true", help="só mostra o que mudaria")
    sp.set_defaults(func=cmd_aplicar)

    args = p.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()

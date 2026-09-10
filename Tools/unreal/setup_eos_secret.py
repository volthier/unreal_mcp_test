#!/usr/bin/env python3
"""Grava o ClientSecret do EOS no config de plataforma do projeto (nunca versionado).

Por que isto existe:
    O EOS_Platform_Create exige ClientCredentials COMPLETAS (ClientId + ClientSecret).
    Sem o segredo o log mostra
        LogEOSSDK: Error: ClientCredentials.ClientSecret cannot be null
        LogOnline: Error: EOS: FOnlineSubsystemEOS::PlatformCreate() failed to init EOS platform
    e o login nao funciona nem com o Dev Auth Tool: a plataforma nem sobe.

    Os identificadores publicos (ClientId, ProductId, SandboxId, DeploymentId, EncryptionKey)
    ficam versionados em Config/DefaultEngine.ini. O SEGREDO nao pode ir para o Git, entao ele
    mora em Config/<Plataforma>/<Plataforma>Engine.ini (ex.: Config/Mac/MacEngine.ini), que o
    engine le por cima do Default e que o .gitignore mantem fora do repositorio.

Uso:
    python3 Tools/unreal/setup_eos_secret.py <ClientSecret>
    EOS_CLIENT_SECRET=<segredo> python3 Tools/unreal/setup_eos_secret.py

O segredo nunca e impresso. Rodar de novo sobrescreve o arquivo do usuario.
"""

from __future__ import annotations

import os
import re
import sys
from pathlib import Path

PROJETO = Path(__file__).resolve().parents[2]
INI_PADRAO = PROJETO / "Config" / "DefaultEngine.ini"


def config_de_plataforma() -> Path:
    """Config de plataforma do PROJETO: Config/<Plataforma>/<Plataforma>Engine.ini.

    O engine le esse arquivo DEPOIS de Config/DefaultEngine.ini, e e o unico lugar fora do
    versionado que sobrevive ao ciclo do engine: o Saved/Config/<Plataforma>/Engine.ini e
    reescrito (e some) quando o editor fecha.
    """
    if sys.platform == "darwin":
        plataforma = "Mac"
    elif os.name == "nt":
        plataforma = "Windows"
    else:
        plataforma = "Linux"
    return PROJETO / "Config" / plataforma / (plataforma + "Engine.ini")


CABECALHO = """; --- Configuracao do EOS que NAO vai para o Git -----------------------------------------
;
; Gerado por Tools/unreal/setup_eos_secret.py. O EOS_Platform_Create exige ClientCredentials
; completas (ClientId + ClientSecret); sem o segredo o log acusa
;   LogEOSSDK: Error: ClientCredentials.ClientSecret cannot be null
; e o EOS nao inicializa.
;
; O .gitignore mantem este arquivo fora do repositorio (Config/<Plataforma>/<Plataforma>Engine.ini):
; os identificadores publicos ficam versionados em Config/DefaultEngine.ini e aqui vive so o
; segredo do artefato DEV. NAO COMMITAR. Nao distribuir em build de cliente.
[/Script/OnlineSubsystemEOS.EOSSettings]
; A linha abaixo SUBSTITUI a lista de artefatos do DefaultEngine.ini (repare que nao tem o
; prefixo +), e e isso que permite trocar o artefato por um completo, com o segredo.
"""


def ler_artefato(texto: str) -> "dict[str, str]":
    """Extrai os campos do +Artifacts=(...) versionado, na ordem em que estao."""
    linha = re.search(r"^\+Artifacts=\((.*)\)\s*$", texto, re.MULTILINE)
    if not linha:
        raise SystemExit("Nao achei a linha +Artifacts=(...) em Config/DefaultEngine.ini")
    campos = re.findall(r'(\w+)="([^"]*)"', linha.group(1))
    if not campos:
        raise SystemExit("A linha de artefato nao tem campos legiveis")
    return dict(campos)


def montar_artefato(campos: "dict[str, str]", segredo: str) -> str:
    """Refaz a linha do artefato com o segredo, logo depois do ClientId."""
    partes = []
    escrito = False
    for chave, valor in campos.items():
        if chave == "ClientSecret":
            continue
        partes.append(chave + '="' + valor + '"')
        if chave == "ClientId" and not escrito:
            partes.append('ClientSecret="' + segredo + '"')
            escrito = True
    if not escrito:
        partes.append('ClientSecret="' + segredo + '"')
    return "Artifacts=(" + ",".join(partes) + ")"


def main() -> int:
    segredo = (sys.argv[1] if len(sys.argv) > 1 else os.environ.get("EOS_CLIENT_SECRET", "")).strip()
    if not segredo:
        print(__doc__)
        return 2

    campos = ler_artefato(INI_PADRAO.read_text(encoding="utf-8"))
    destino = config_de_plataforma()
    destino.parent.mkdir(parents=True, exist_ok=True)
    destino.write_text(CABECALHO + montar_artefato(campos, segredo) + "\n", encoding="utf-8")

    print("EOS: segredo gravado em " + str(destino.relative_to(PROJETO)) + " (ignorado pelo Git)")
    print("EOS: artefato " + campos.get("ArtifactName", "?")
          + " client_id=" + campos.get("ClientId", "?")[:8] + "...")
    print("EOS: confira com  Automation RunTests Runner.Sessao.EOSConfigurado")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

#!/bin/bash
# Roda os testes de automacao do projeto e FALHA se o numero de testes nao for o esperado.
#
# POR QUE ISTO EXISTE, e a licao que ele guarda: eu quebrei o build, commitei, e depois quebrei a INICIALIZACAO
# do engine com o style system. Nos dois casos a guarda que eu tinha era frouxa - ela checava apenas
# "falhas == 0", e um resultado com ZERO TESTES tambem tem zero falhas. Resultado: dois commits entraram com
# verificacao vazia, e o projeto ficou parado de inicializar sem ninguem perceber pelo teste.
#
# A regra agora: se o numero de testes NAO for 15, isto falha - com zero testes inclusive.
#
# Uso: bash Tools/unreal/testes.sh   (ou ./Tools/unreal/testes.sh)

set -u
cd "$(dirname "$0")/../.." || exit 1

ESPERADOS=15
LOG=/tmp/testes_projeto.log

"/Users/Shared/Epic Games/UE_5.8/Engine/Binaries/Mac/UnrealEditor-Cmd" \
  "$(pwd)/PloidrekRPG.uproject" \
  -ExecCmds="Automation RunTests Runner;Quit" \
  -unattended -nosplash -nullrhi -stdout -testexit="Automation Test Queue Empty" > "$LOG" 2>&1

TESTES=$(grep 'Test Completed' "$LOG" | grep -c 'Path={Runner')
FALHAS=$(grep 'Test Completed' "$LOG" | grep 'Path={Runner' | grep -c 'Fail')
LINHAS=$(wc -l < "$LOG" | tr -d ' ')

echo "  testes: $TESTES (esperado $ESPERADOS) | falhas: $FALHAS | log com $LINHAS linhas"

if [ "$TESTES" != "$ESPERADOS" ]; then
  echo "  FALHOU: rodaram $TESTES testes em vez de $ESPERADOS."
  echo "  Quando o numero vem ZERO, o engine saiu antes de rodar - modulo quebrado ou inicializacao falhando."
  echo "  Nao trate isso como aprovacao: um resultado vazio nao e um resultado bom."
  exit 1
fi

if [ "$FALHAS" != "0" ]; then
  echo "  FALHOU: $FALHAS teste(s) com falha."
  exit 1
fi

echo "  OK: $ESPERADOS testes, zero falhas."

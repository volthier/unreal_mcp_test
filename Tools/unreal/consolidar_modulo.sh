#!/usr/bin/env bash
# Consolida o modulo depois de um build feito com o EDITOR ABERTO.
#
# Por que existe: com o editor aberto, o UBT nao consegue escrever o dylib principal e cria iteracoes
# numeradas (libUnrealEditor-PloidrekRPG-0001.dylib, -0002, ...). Copiar a mais nova sobre o principal NAO
# basta: o arquivo carrega a propria IDENTIDADE embutida (LC_ID_DYLIB) apontando para o nome numerado, e o
# loader deixa de resolver o modulo - o sintoma e cruel: o editor abre, mas commandlets rodam ZERO testes e
# o pyscript nem executa, sem erro claro no log.
#
# Uso: bash Tools/unreal/consolidar_modulo.sh
set -euo pipefail
cd "$(dirname "$0")/../.."

PRINCIPAL="Binaries/Mac/libUnrealEditor-PloidrekRPG.dylib"
NOVA=$(ls -t Binaries/Mac/libUnrealEditor-PloidrekRPG-*.dylib 2>/dev/null | head -1 || true)

if [ -z "$NOVA" ]; then
  echo "nenhuma iteracao numerada: o build escreveu o principal direto (editor fechado). Nada a fazer."
  exit 0
fi

echo "iteracao mais nova: $NOVA"
cp "$NOVA" "$PRINCIPAL"
install_name_tool -id "@rpath/libUnrealEditor-PloidrekRPG.dylib" "$PRINCIPAL"
codesign --force --sign - "$PRINCIPAL" >/dev/null 2>&1 || true
rm -f Binaries/Mac/libUnrealEditor-PloidrekRPG-*.dylib

echo "identidade do principal: $(otool -D "$PRINCIPAL" | tail -1)"
echo "pronto. Se ainda rodar zero testes, o conserto limpo e FECHAR o editor e rodar o UBT de novo."
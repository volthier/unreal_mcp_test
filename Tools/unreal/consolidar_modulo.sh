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

# A CAUSA RAIZ (descoberta na rodada 9): o engine NAO carrega 'libUnrealEditor-PloidrekRPG.dylib' por
# convencao - ele carrega o arquivo nomeado em Binaries/Mac/UnrealEditor.modules. Copiar para o nome
# 'principal' e apagar a iteracao deixa o manifesto apontando para um arquivo que nao existe, e o engine
# simplesmente nao carrega o modulo: o editor abre, mas commandlets rodam ZERO testes e o pyscript nao
# executa, sem erro no log. Entao: o manifesto e que manda - mantem o arquivo com o nome que ele pede.
python3 - <<'PY'
import json, shutil
from pathlib import Path
manifesto = Path('Binaries/Mac/UnrealEditor.modules')
principal = Path('Binaries/Mac/libUnrealEditor-PloidrekRPG.dylib')
if manifesto.exists() and principal.exists():
    dados = json.loads(manifesto.read_text())
    nome = dados.get('Modules', {}).get('PloidrekRPG')
    if nome:
        alvo = Path('Binaries/Mac') / nome
        if not alvo.exists():
            shutil.copy2(principal, alvo)
            print('  arquivo restaurado com o nome do manifesto:', nome)
        else:
            print('  manifesto e arquivo ja concordam:', nome)
PY

echo "identidade do principal: $(otool -D "$PRINCIPAL" | tail -1)"
echo "manifesto: $(python3 -c "import json;print(json.load(open('Binaries/Mac/UnrealEditor.modules'))['Modules']['PloidrekRPG'])")"
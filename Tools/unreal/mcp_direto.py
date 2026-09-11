#!/usr/bin/env python3
'''Cliente MCP minimo, por HTTP, para falar com o editor do Unreal numa porta escolhida.

Por que existe: quando o autor esta com o editor aberto, a instancia dele segura a porta 8000 E o lock do
projeto - e o editor dele carregou o modulo C++ de quando subiu, entao nao roda codigo novo nem recarrega
asset alterado. Subindo uma instancia propria numa porta livre (8010, via ServerPortNumber no
DefaultEditorPerProjectUserSettings.ini), eu falo direto com um editor que tem o modulo ATUAL.

Uso:
  python3 Tools/unreal/mcp_direto.py toolsets
  python3 Tools/unreal/mcp_direto.py chamar <ferramenta> '<json dos argumentos>' [toolset]
  python3 Tools/unreal/mcp_direto.py capturar <caminho.png> [x y z pitch yaw]
'''

from __future__ import annotations

import base64
import json
import sys
import urllib.request

PORTA = 8010
URL = 'http://127.0.0.1:%d/mcp' % PORTA
SESSAO = {'id': None}


def _post(corpo: dict, guardar_sessao: bool = False) -> dict:
    dados = json.dumps(corpo).encode('utf-8')
    cabecalhos = {
        'Content-Type': 'application/json',
        'Accept': 'application/json, text/event-stream',
    }
    if SESSAO['id']:
        cabecalhos['Mcp-Session-Id'] = SESSAO['id']
    pedido = urllib.request.Request(URL, data=dados, headers=cabecalhos, method='POST')
    with urllib.request.urlopen(pedido, timeout=180) as resposta:
        if guardar_sessao:
            SESSAO['id'] = resposta.headers.get('Mcp-Session-Id') or resposta.headers.get('mcp-session-id')
        texto = resposta.read().decode('utf-8', 'replace')
    # o transporte pode responder em SSE: pega as linhas 'data:'
    for linha in texto.splitlines():
        if linha.startswith('data:'):
            try:
                return json.loads(linha[5:].strip())
            except json.JSONDecodeError:
                continue
    try:
        return json.loads(texto)
    except json.JSONDecodeError:
        return {'bruto': texto[:400]}


def abrir() -> None:
    _post({
        'jsonrpc': '2.0', 'id': 1, 'method': 'initialize',
        'params': {'protocolVersion': '2024-11-05', 'capabilities': {},
                   'clientInfo': {'name': 'ploidrek-agente', 'version': '1'}},
    }, guardar_sessao=True)
    _post({'jsonrpc': '2.0', 'method': 'notifications/initialized', 'params': {}})


def chamar(ferramenta: str, argumentos: dict, toolset: str | None = None) -> dict:
    abrir()
    if toolset:
        corpo = {'jsonrpc': '2.0', 'id': 2, 'method': 'tools/call',
                 'params': {'name': 'call_tool',
                            'arguments': {'toolset_name': toolset, 'tool_name': ferramenta, 'arguments': argumentos}}}
    else:
        corpo = {'jsonrpc': '2.0', 'id': 2, 'method': 'tools/call',
                 'params': {'name': ferramenta, 'arguments': argumentos}}
    return _post(corpo)


def texto(resposta: dict) -> str:
    resultado = resposta.get('result') or {}
    conteudo = resultado.get('content') or []
    if conteudo and isinstance(conteudo, list):
        return str(conteudo[0].get('text', ''))
    return json.dumps(resposta)[:600]


def principal() -> int:
    acao = sys.argv[1] if len(sys.argv) > 1 else 'toolsets'
    if acao == 'toolsets':
        abrir()
        lista = _post({'jsonrpc': '2.0', 'id': 3, 'method': 'tools/list', 'params': {}})
        nomes = [f.get('name') for f in (lista.get('result') or {}).get('tools', [])]
        print('ferramentas do editor: ' + ', '.join(n for n in nomes if n))
        return 0
    if acao == 'chamar':
        ferramenta = sys.argv[2]
        argumentos = json.loads(sys.argv[3]) if len(sys.argv) > 3 else {}
        toolset = sys.argv[4] if len(sys.argv) > 4 else None
        print(texto(chamar(ferramenta, argumentos, toolset))[:1200])
        return 0
    if acao == 'capturar':
        destino = sys.argv[2]
        resto = [float(v) for v in sys.argv[3:8]] if len(sys.argv) >= 8 else None
        argumentos: dict = {'annotations': {}, 'bShowUI': False}
        if resto:
            x, y, z, pitch, yaw = resto
            argumentos['captureTransform'] = {'location': {'x': x, 'y': y, 'z': z},
                                              'rotation': {'pitch': pitch, 'yaw': yaw, 'roll': 0.0}}
        resposta = chamar('CaptureViewport', argumentos, 'EditorToolset.EditorAppToolset')
        try:
            dados = json.loads(texto(resposta))['returnValue']['image']['data']
        except Exception as erro:
            print('falhou: %s | %s' % (erro, texto(resposta)[:200]))
            return 1
        with open(destino, 'wb') as arquivo:
            arquivo.write(base64.b64decode(dados))
        print('captura salva: ' + destino)
        return 0
    print('acao desconhecida: ' + acao)
    return 2


if __name__ == '__main__':
    raise SystemExit(principal())

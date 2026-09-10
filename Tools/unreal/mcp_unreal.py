#!/usr/bin/env python3
'''Cliente minimo do Unreal MCP (o servidor MCP oficial que roda DENTRO do editor).

Por que cru, sem SDK: o servidor responde JSON puro em POST /mcp, e o SDK mcp 2.1.1 renomeou a API
(streamablehttp_client -> streamable_http_client), o que quebra codigo de exemplo. Falando direto, nao
dependo de versao de biblioteca.

Salva tambem as imagens que o editor devolve (CaptureViewport, CaptureEditorImage, CaptureAssetImage) -
e isso que finalmente da OLHOS ao agente: capturar a cena e julgar imagem, em vez de confiar em relatorio.

Uso:
  python3 Tools/unreal/mcp_unreal.py listar
  python3 Tools/unreal/mcp_unreal.py toolsets
  python3 Tools/unreal/mcp_unreal.py descrever EditorToolset.EditorAppToolset
  python3 Tools/unreal/mcp_unreal.py chamar CaptureViewport '{"bShowUI": false}' [caminho.png]
'''

import base64
import json
import sys
import urllib.request
from pathlib import Path

URL = 'http://127.0.0.1:8000/mcp'
CABECALHOS = {'Content-Type': 'application/json', 'Accept': 'application/json, text/event-stream'}
PASTA_IMAGENS = Path('Art/generated/editor')


def chamar(metodo, params=None, identificador=1, sessao=None):
    corpo = {'jsonrpc': '2.0', 'id': identificador, 'method': metodo}
    if params is not None:
        corpo['params'] = params
    cabecalhos = dict(CABECALHOS)
    if sessao:
        cabecalhos['Mcp-Session-Id'] = sessao
    pedido = urllib.request.Request(URL, data=json.dumps(corpo).encode('utf-8'), headers=cabecalhos)
    with urllib.request.urlopen(pedido, timeout=120) as resposta:
        texto = resposta.read().decode('utf-8')
        nova_sessao = resposta.headers.get('Mcp-Session-Id')
    try:
        return json.loads(texto), nova_sessao
    except json.JSONDecodeError:
        return {'bruto': texto[:2000]}, nova_sessao


def salvar_imagens(resposta, destino=None):
    '''Salva as capturas. O editor Unreal manda a imagem DENTRO do JSON de texto, no formato
    {"returnValue": {"image": {"mimeType": ..., "data": "<base64>"}}} - nao como bloco de imagem do MCP.'''
    blocos = (resposta.get('result') or {}).get('content') or []
    salvos = []

    def gravar(dados_base64, indice):
        caminho = Path(destino) if destino else (PASTA_IMAGENS / ('captura_%02d.png' % indice))
        caminho.parent.mkdir(parents=True, exist_ok=True)
        caminho.write_bytes(base64.b64decode(dados_base64))
        salvos.append(str(caminho))

    for indice, bloco in enumerate(blocos):
        if bloco.get('type') == 'image':
            gravar(bloco.get('data', ''), indice)
            continue
        if bloco.get('type') != 'text':
            continue
        try:
            dados = json.loads(bloco['text'])
        except Exception:
            continue
        imagem = (dados.get('returnValue') or {}).get('image') if isinstance(dados, dict) else None
        if isinstance(imagem, dict) and imagem.get('data'):
            gravar(imagem['data'], indice)
    return salvos


def texto_de(resposta):
    blocos = (resposta.get('result') or {}).get('content') or []
    return chr(10).join(b.get('text', '') for b in blocos if b.get('type') == 'text')


def principal():
    acao = sys.argv[1] if len(sys.argv) > 1 else 'listar'
    info, sessao = chamar('initialize', {'protocolVersion': '2025-06-18', 'capabilities': {},
                                         'clientInfo': {'name': 'dsh', 'version': '1'}})
    if acao == 'listar':
        print('protocolo:', info.get('result', {}).get('protocolVersion'))
        resposta, _ = chamar('tools/list', {}, 2, sessao)
        for f in (resposta.get('result') or {}).get('tools', []):
            print('  -', f.get('name'), '::', (f.get('description') or '').replace(chr(10), ' ')[:100])
        return 0

    if acao == 'toolsets':
        resposta, _ = chamar('tools/call', {'name': 'list_toolsets', 'arguments': {}}, 3, sessao)
        print(texto_de(resposta))
        return 0

    if acao == 'descrever':
        resposta, _ = chamar('tools/call', {'name': 'describe_toolset', 'arguments': {'toolset_name': sys.argv[2]}}, 3, sessao)
        print(texto_de(resposta))
        return 0

    if acao == 'chamar':
        nome = sys.argv[2]
        argumentos = json.loads(sys.argv[3]) if len(sys.argv) > 3 else {}
        destino = sys.argv[4] if len(sys.argv) > 4 else None
        # aceita 'Tool' (top-level) ou 'Toolset.Tool' - o servidor quer os dois separados
        pedido = {'tool_name': nome, 'arguments': argumentos}
        if '.' in nome:
            conjunto, ferramenta = nome.rsplit('.', 1)
            pedido = {'toolset_name': conjunto, 'tool_name': ferramenta, 'arguments': argumentos}
        resposta, _ = chamar('tools/call', {'name': 'call_tool', 'arguments': pedido}, 3, sessao)
        if 'error' in resposta:
            print('ERRO:', json.dumps(resposta['error'])[:400])
            return 1
        for caminho in salvar_imagens(resposta, destino):
            print('IMAGEM SALVA:', caminho)
        conteudo = texto_de(resposta)
        if conteudo:
            print(conteudo[:3000])
        return 0

    print('acao desconhecida:', acao)
    return 2


if __name__ == '__main__':
    raise SystemExit(principal())

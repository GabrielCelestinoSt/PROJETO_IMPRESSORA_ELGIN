# 🖨️ Sistema PDV - Impressora Elgin i9

Sistema de Ponto de Venda (PDV) desenvolvido em C para gerenciamento e comunicação com impressoras de cupom fiscal Elgin, utilizando a biblioteca oficial E1_Impressora01.dll.

## Integrantes do Grupo

- Gabriel Celestino dos Santos
- João Arthur de Freitas Silva
- Júlia de Souza Silva
- Kauã Henrique Bispo Soares
- Vinicius Henrique Menezes Moreira

## 📋 Descrição do Projeto

Este projeto simula um sistema completo de atendimento de caixa (PDV simplificado) que permite a comunicação e controle de impressoras térmicas Elgin através de um menu interativo no terminal. O sistema integra lógica de programação com automação comercial, oferecendo funcionalidades essenciais para estabelecimentos comerciais.

## Funcionalidades

O sistema contém as seguintes operações:

### Gerenciamento de Conexão
- Configuração de tipo de conexão (USB/Rede)
- Abertura e fechamento de conexão com impressora
- Suporte para modelos i7, i8 e i9

### Impressão de Documentos
- Impressão de texto customizado
- Geração de QR Codes
- Impressão de códigos de barras (CODE128)
- Impressão de XML SAT (cupom fiscal)
- Impressão de XML de cancelamento SAT

### Operações de Hardware
- Abertura de gaveta (modo Elgin e padrão)
- Emissão de sinal sonoro (beeps)
- Avanço de papel
- Corte automático de papel

## Tecnologias Utilizadas

- **Linguagem:** C (ANSI C99)
- **Plataforma de Desenvolvimento** DevC++
- **Biblioteca:** E1_Impressora01.dll (Elgin)
- **Sistema Operacional:** Windows
- **API:** Windows.h para manipulação de DLL

## Requisitos

### Software
- Compilador C (DevC++,VisualStudio Code, MSVC, etc.)
- Windows (qualquer versão moderna)
- Biblioteca E1_Impressora01.dll

### Hardware
- Impressora térmica Elgin (modelos i7, i8 ou i9)
- Cabo USB ou rede TCP/IP
- Gaveta de dinheiro (opcional)

## Como Compilar

### Usando Dev-C++:

#### Método 1: Compilação pelo IDE
1. Abra o Dev-C++
2. Vá em **File → New → Project**
3. Selecione **Console Application** e **C Project**
4. Dê um nome ao projeto (ex: `SistemaPDV`)
5. Clique em **OK** e escolha onde salvar
6. Copie todo o código do `codigo_impressora_v2.c` para o arquivo gerado
7. Vá em **Execute → Compile** (ou pressione `F9`)
8. Execute com **Execute → Run** (ou pressione `F10`)

#### Método 2: Compilação via Terminal do Dev-C++
1. Abra o Dev-C++ e vá em **Tools → Compiler Options**
2. Certifique-se que o compilador está configurado corretamente
3. Abra o terminal (cmd) na pasta do projeto
4. Execute:
```bash
gcc codigo_impressora_v2.c -o sistema_pdv.exe -lkernel32
```

#### Configurações Importantes no Dev-C++:
- **Compiler:** TDM-GCC (padrão do Dev-C++)
- **Standard:** C99 ou superior
- Certifique-se de que a DLL `E1_Impressora01.dll` está na mesma pasta do executável gerado


## Como Usar

### 1. Preparação
- Certifique-se de que a DLL `E1_Impressora01.dll` está no mesmo diretório (pasta) do executável
- Para impressão de XML, tenha os arquivos `XMLSAT.xml` e `CANC_SAT.xml` no diretório

### 2. Execução
```bash
sistema_pdv.exe
```

### 3. Configuração Inicial
- Execute a opção `1 - Configurar Conexao`
- Escolha o tipo de conexão:
  - `1` para USB
  - `5` para Rede (TCP/IP)
- Informe o modelo da impressora (i7, i8 ou i9)
- Para rede, informe o IP e porta (geralmente 9100)

### 4. Conectar
- Execute a opção `2 - Abrir Conexao`
- Aguarde confirmação de sucesso

### 5. Utilizar as Funcionalidades
- Navegue pelo menu e execute as operações desejadas

## 📝 Menu Principal

```
==================== MENU PRINCIPAL ====================

  1  - Configurar Conexao
  2  - Abrir Conexao com Impressora
  3  - Fechar Conexao com Impressora
  4  - Imprimir Texto
  5  - Imprimir QR Code
  6  - Imprimir Codigo de Barras
  7  - Imprimir XML SAT
  8  - Imprimir XML Cancelamento SAT
  9  - Abrir Gaveta (Elgin)
  10 - Abrir Gaveta (Padrao)
  11 - Emitir Sinal Sonoro
  0  - Sair do Sistema

========================================================
```

## 🔍 Exemplos de Uso

### Impressão de Texto Simples
```
Escolha opcao: 4
Digite o texto: EXEMPLO DE CUPOM FISCAL
[SUCESSO] Texto impresso!
```

### Impressão de QR Code
```
Escolha opcao: 5
Digite o conteudo: teste
[SUCESSO] QR Code impresso!
```

### Configuração de Rede
```
Escolha opcao: 1
Tipo: 5
Modelo: i9
IP: 192.168.1.100
Porta: 9100
[OK] Configuracao salva!
```

## Funções da Biblioteca Elgin Implementadas

| Função | Descrição | Parâmetros Utilizados |
|--------|-----------|----------------------|
| `AbreConexaoImpressora()` | Estabelece conexão | tipo, modelo, conexão, parâmetro |
| `FechaConexaoImpressora()` | Encerra conexão | - |
| `ImpressaoTexto()` | Imprime texto | texto, posição, estilo, tamanho |
| `ImpressaoQRCode()` | Gera QR Code | dados, tamanho=6, correção=4 |
| `ImpressaoCodigoBarras()` | Imprime código de barras | tipo=8, código, altura=100, largura=2, HRI=3 |
| `AvancaPapel()` | Avança papel | linhas=5 |
| `Corte()` | Corta papel | tipo=1 |
| `AbreGavetaElgin()` | Abre gaveta Elgin | pino=1, ti=50ms, tf=50ms |
| `AbreGaveta()` | Abre gaveta padrão | pino=1, ti=5ms, tf=10ms |
| `SinalSonoro()` | Emite beeps | qtd=4, ti=50ms, tf=5ms |
| `ImprimeXMLSAT()` | Imprime cupom fiscal | XML, parâmetro=0 |
| `ImprimeXMLCancelamentoSAT()` | Imprime cancelamento | XML, assinatura, parâmetro=0 |

## 📂 Estrutura do Código

```
main.c
├── Configuração de DLL
│   ├── Definições de tipos
│   ├── Ponteiros de função
│   └── Variáveis globais
├── Funções Utilitárias
│   ├── flush_entrada()
│   ├── limparTela()
│   ├── carregarFuncoes()
│   └── liberarBiblioteca()
├── Funções de Interface
│   ├── exibirCabecalho()
│   └── exibirMenu()
├── Funções de Operação
│   ├── configurarConexao()
│   ├── abrirConexao()
│   ├── fecharConexao()
│   ├── imprimirTexto()
│   ├── imprimirQRCode()
│   ├── imprimirCodigoBarras()
│   ├── imprimirXMLSAT()
│   ├── imprimirXMLCancelamentoSAT()
│   ├── abrirGavetaElginOpc()
│   ├── abrirGavetaOpc()
│   └── emitirSinalSonoro()
└── main()
```

## Conceitos de Programação Aplicados

- **Estruturas de Repetição:** Loop principal do menu (while)
- **Estruturas Condicionais:** switch-case para seleção de opções, if-else para validações
- **Funções Personalizadas:** Modularização de cada operação
- **Manipulação de Arquivos:** Leitura de XML para impressão
- **Ponteiros de Função:** Carregamento dinâmico da DLL
- **Tratamento de Erros:** Validação de retorno das funções

## ⚠️ Notas Importantes

- A impressora deve estar ligada e conectada antes de abrir a conexão
- Arquivos XML devem estar no formato correto do SAT fiscal
- A gaveta de dinheiro deve estar conectada na impressora (se for usar essa função)
- Certifique-se de fechar a conexão antes de sair do sistema
- Para conexão em rede, verifique se o firewall permite comunicação na porta configurada


## Objetivo Acadêmico

### Propósito
Desenvolver um programa que simule um sistema de comunicação com impressoras de cupom fiscal da Elgin, utilizando funções específicas da biblioteca, laços de repetição, estruturas condicionais e funções personalizadas. O foco é integrar lógica de programação com um cenário prático de automação comercial.

### Requisitos Implementados
- ✅ Estabelecer e encerrar conexão com impressora
- ✅ Menu interativo com múltiplas opções
- ✅ Utilização de TODAS as 12 funções da biblioteca Elgin
- ✅ Laços de repetição para manter menu em execução
- ✅ Estruturas condicionais (if/else/switch)
- ✅ Organização em funções próprias
- ✅ Tratamento de entrada do usuário
- ✅ Feedback visual das operações

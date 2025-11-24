#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/* ======================= Config DLL ======================= */
static HMODULE g_hDll = NULL;

/* ConvenÃ§Ã£o de chamada (Windows): __stdcall */
#ifndef CALLCONV
#  define CALLCONV WINAPI
#endif

/* ======================= Assinaturas da DLL ======================= */
typedef int (CALLCONV *AbreConexaoImpressora_t)(int, const char *, const char *, int);
typedef int (CALLCONV *FechaConexaoImpressora_t)(void);
typedef int (CALLCONV *ImpressaoTexto_t)(const char *, int, int, int);
typedef int (CALLCONV *Corte_t)(int);
typedef int (CALLCONV *ImpressaoQRCode_t)(const char *, int, int);
typedef int (CALLCONV *ImpressaoCodigoBarras_t)(int, const char *, int, int, int);
typedef int (CALLCONV *AvancaPapel_t)(int);
typedef int (CALLCONV *AbreGavetaElgin_t)(int, int, int);
typedef int (CALLCONV *AbreGaveta_t)(int, int, int);
typedef int (CALLCONV *SinalSonoro_t)(int, int, int);
typedef int (CALLCONV *ImprimeXMLSAT_t)(const char *, int);
typedef int (CALLCONV *ImprimeXMLCancelamentoSAT_t)(const char *, const char *, int);
typedef int (CALLCONV *InicializaImpressora_t)(void);

/* ======================= Ponteiros ======================= */
static AbreConexaoImpressora_t        AbreConexaoImpressora        = NULL;
static FechaConexaoImpressora_t       FechaConexaoImpressora       = NULL;
static ImpressaoTexto_t               ImpressaoTexto               = NULL;
static Corte_t                        Corte                        = NULL;
static ImpressaoQRCode_t              ImpressaoQRCode              = NULL;
static ImpressaoCodigoBarras_t        ImpressaoCodigoBarras        = NULL;
static AvancaPapel_t                  AvancaPapel                  = NULL;
static AbreGavetaElgin_t              AbreGavetaElgin              = NULL;
static AbreGaveta_t                   AbreGaveta                   = NULL;
static SinalSonoro_t                  SinalSonoro                  = NULL;
static ImprimeXMLSAT_t                ImprimeXMLSAT                = NULL;
static ImprimeXMLCancelamentoSAT_t    ImprimeXMLCancelamentoSAT    = NULL;
static InicializaImpressora_t         InicializaImpressora         = NULL;

/* ======================= ConfiguraÃ§Ã£o ======================= */
static int   g_tipo      = 1;
static char  g_modelo[64] = "i9";
static char  g_conexao[128] = "USB";
static int   g_parametro = 0;
static int   g_conectada = 0;

/* ======================= Utilidades ======================= */
#define LOAD_FN(h, name)                                                         \
    do {                                                                         \
        name = (name##_t)GetProcAddress((HMODULE)(h), #name);                    \
        if (!(name)) {                                                           \
            fprintf(stderr, "Falha ao resolver simbolo %s (erro=%lu)\n",         \
                    #name, GetLastError());                                      \
            return 0;                                                            \
        }                                                                        \
    } while (0)

static void flush_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}


static void limparTela(void) {
    system("cls");
}

/* ======================= FunÃ§Ãµes para manipular a DLL ======================= */
static int carregarFuncoes(void)
{
    g_hDll = LoadLibraryA("E1_Impressora01.dll");
    if (!g_hDll) {
        fprintf(stderr, "Erro ao carregar E1_Impressora01.dll (erro=%lu)\n", GetLastError());
        return 0;
    }

    LOAD_FN(g_hDll, AbreConexaoImpressora);
    LOAD_FN(g_hDll, FechaConexaoImpressora);
    LOAD_FN(g_hDll, ImpressaoTexto);
    LOAD_FN(g_hDll, Corte);
    LOAD_FN(g_hDll, ImpressaoQRCode);
    LOAD_FN(g_hDll, ImpressaoCodigoBarras);
    LOAD_FN(g_hDll, AvancaPapel);
    LOAD_FN(g_hDll, AbreGavetaElgin);
    LOAD_FN(g_hDll, AbreGaveta);
    LOAD_FN(g_hDll, SinalSonoro);
    LOAD_FN(g_hDll, ImprimeXMLSAT);
    LOAD_FN(g_hDll, ImprimeXMLCancelamentoSAT);
    LOAD_FN(g_hDll, InicializaImpressora);

    return 1;
}

static void liberarBiblioteca(void)
{
    if (g_hDll) {
        FreeLibrary(g_hDll);
        g_hDll = NULL;
    }
}

/* ======================= FunÃ§Ãµes implementadas ======================= */

static void exibirCabecalho(void)
{
    printf("\n");
    printf("========================================================\n");
    printf("        SISTEMA PDV - IMPRESSORA ELGIN i9              \n");
    printf("              Gerenciamento de Caixa                   \n");
    printf("========================================================\n");
    printf("\n");
    if (g_conectada) {
        printf("Status: [CONECTADA] Impressora: %s via %s\n\n", g_modelo, g_conexao);
    } else {
        printf("Status: [DESCONECTADA] Aguardando conexao...\n\n");
    }
}

static void exibirMenu(void)
{
    printf("==================== MENU PRINCIPAL ====================\n");
    printf("\n");
    printf("  1  - Configurar Conexao\n");
    printf("  2  - Abrir Conexao com Impressora\n");
    printf("  3  - Fechar Conexao com Impressora\n");
    printf("  4  - Imprimir Texto\n");
    printf("  5  - Imprimir QR Code\n");
    printf("  6  - Imprimir Codigo de Barras\n");
    printf("  7  - Imprimir XML SAT\n");
    printf("  8  - Imprimir XML Cancelamento SAT\n");
    printf("  9  - Abrir Gaveta (Elgin)\n");
    printf("  10 - Abrir Gaveta (Padrao)\n");
    printf("  11 - Emitir Sinal Sonoro\n");

    printf("  0  - Sair do Sistema\n");
    printf("\n");
    printf("========================================================\n");
    printf("\nEscolha uma opcao: ");
}


// Função responsável por configurar a conexão com a impressora.
// Aqui eu defino o tipo de conexão, modelo, parâmetros e faço validações.
static void configurarConexao(void)
{
    char resp;                  // Variável para receber resposta 's' ou 'n'
    char modelo_buf[50];        // Buffer não utilizado, mas deixei aqui caso eu precise guardar algo futuramente
    char conexao_buf[100];      // Mesmo caso do modelo_buf

    limparTela();               // Limpo a tela antes de mostrar o menu de configuração
    printf("========== CONFIGURACAO DE CONEXAO ==========\n\n");

    // Se já estiver conectada, não deixo alterar nada
    if (g_conectada) {
        printf("A impressora está conectada!\n");
        printf("Para alterar a configuracao, feche a conexao primeiro.\n");
        return;
    }

    // Escolha do tipo de comunicação
    do {
        printf("--------------------------------------------\n");
        printf("| Valor | Referencia                        |\n");
        printf("--------------------------------------------\n");
        printf("| 1     | USB                               |\n");
        printf("| 2     | RS232                             |\n");
        printf("| 3     | TCP/IP                            |\n");
        printf("| 4     | Bluetooth                          |\n");
        printf("| 5     | Impressoras acopladas (Android)    |\n");
        printf("--------------------------------------------\n");

        printf("Digite o tipo de comunicacao: ");
        scanf("%d", &g_tipo);   // Recebo o tipo escolhido e coloco na variável global g_tipo

        // Se digitou errado, aviso
        if (g_tipo < 1 || g_tipo > 5)
            printf("\nTipo invalido! Digite novamente.\n");

    } while (g_tipo < 1 || g_tipo > 5);   // Repito até digitar um valor válido

    // Se o tipo NÃO for Android (tipo 5), preciso pedir o modelo da impressora
    if (g_tipo != 5) {

        int modeloValido = 0;   // Controle para validar o modelo digitado

        do {
            // Exibo a lista de modelos aceitos
            printf("-----------------------------------\n");
            printf("| Modelos                          |\n");
            printf("-----------------------------------\n");
            printf("| i7                               |\n");
            printf("| i7 Plus                          |\n");
            printf("| i8                               |\n");
            printf("| i9                               |\n");
            printf("| ix                               |\n");
            printf("| Fitpos                           |\n");
            printf("| BK-T681                          |\n");
            printf("| MP-4200 (Para modelos TH e ADV)  |\n");
            printf("| MP-4200 HS                       |\n");
            printf("| MK                               |\n");
            printf("| MP-2800                          |\n");
            printf("-----------------------------------\n");

            printf("Digite o modelo exatamente como na tabela: ");
            scanf(" %[^\n]", g_modelo);  // Leio inclusive nomes com espaço

            // Aqui comparo com todos os modelos permitidos.
            if (strcmp(g_modelo, "i7") == 0 ||
                strcmp(g_modelo, "i7 Plus") == 0 ||
                strcmp(g_modelo, "i8") == 0 ||
                strcmp(g_modelo, "i9") == 0 ||
                strcmp(g_modelo, "ix") == 0 ||
                strcmp(g_modelo, "Fitpos") == 0 ||
                strcmp(g_modelo, "BK-T681") == 0 ||
                strcmp(g_modelo, "MP-4200 (Para modelos TH e ADV)") == 0 ||
                strcmp(g_modelo, "MP-4200 HS") == 0 ||
                strcmp(g_modelo, "MK") == 0 ||
                strcmp(g_modelo, "MP-2800") == 0)
            {
                modeloValido = 1;    // Modelo aceito
            } else {
                printf("\nModelo invalido! Digite novamente.\n");
            }

        } while (!modeloValido);  // Só saio quando digitar um modelo válido

    } else {
        // Tipo 5 não usa modelo, então deixo vazio
        strcpy(g_modelo, "");
    }

    // Defino a conexão padrão conforme o tipo selecionado
    switch (g_tipo) {
        case 1: strcpy(g_conexao, "USB"); break;
        case 2: strcpy(g_conexao, "COM2"); break;
        case 3: strcpy(g_conexao, "192.168.0.20"); break;
        case 4: strcpy(g_conexao, "AA:BB:CC:DD:EE:FF"); break;
        case 5: strcpy(g_conexao, ""); break;
    }

    // Pergunto se deseja alterar a conexão padrão
    if (g_tipo != 5) {
        printf("\nConexao padrao: %s\n", g_conexao);
        printf("Deseja alterar? (s/n): ");
        scanf(" %c", &resp);

        if (resp == 's' || resp == 'S') {
            printf("Digite a nova conexao: ");
            scanf(" %[^\n]", g_conexao);
        }
    }

    // Configuração de parâmetros específicos
    if (g_tipo == 2) {
        printf("\nDigite o baudrate: ");
        scanf("%d", &g_parametro);

    } else if (g_tipo == 3) {
        printf("\nDigite a porta TCP ***: ");
        scanf("%d", &g_parametro);

    } else {
        g_parametro = 0;   // Outros tipos não usam parâmetro numérico
    }

    // Exibo o resumo completo da configuração
    printf("\n\nResumo da configuracao:\n");
    printf("--------------------------------------------\n");
    printf("Tipo: %d\n", g_tipo);
    printf("Modelo: %s\n", g_tipo == 5 ? "(vazio)" : g_modelo);
    printf("Conexao: %s\n", g_conexao[0] ? g_conexao : "(vazio)");
    printf("Parametro: %d\n", g_parametro);
    printf("--------------------------------------------\n");
}



////////////////////////////////////////////////////////////////////////////////////////////////////


// Função usada para abrir a conexão com a impressora utilizando as
// configurações já definidas anteriormente.
static void abrirConexao(void)
{
    limparTela();   // Limpo a tela antes de iniciar o processo

    // Se já estiver conectada, não deixo abrir novamente
    if (g_conectada) {
        printf("A impressora ja esta conectada!\n");
        return;
    }

    printf("\nAbrindo Conexao...\n");

    // Chamo a função da DLL passando as variáveis globais
    int retorno = AbreConexaoImpressora(g_tipo, g_modelo, g_conexao, g_parametro);

    // Caso retorno seja 0 significa sucesso
    if (retorno == 0) {
        printf("Conexao aberta com sucesso!\n");
        g_conectada = 1;
        return;
    }

    // Qualquer valor diferente de 0 significa erro
    printf("\nErro ao abrir conexao! Codigo: %d\n", retorno);
    printf("Configuracao incorreta ou dispositivo nao encontrado.\n");

    char opcao;
    printf("Deseja configurar a conexao? (s/n): ");
    scanf(" %c", &opcao);

    // Se quiser configurar novamente
    if (opcao == 's' || opcao == 'S') {

        configurarConexao(); // Chamo a configuração

        printf("\nTentando novamente abrir conexao...\n");

        retorno = AbreConexaoImpressora(g_tipo, g_modelo, g_conexao, g_parametro);

        if (retorno == 0) {
            printf("Conexao aberta com sucesso!\n");
            g_conectada = 1;
        } else {
            printf("Nao foi possível abrir a conexao. Codigo: %d\n", retorno);
        }

    } else {
        printf("Conexao nao configurada. Retornando ao menu...\n");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// Função responsável por fechar a conexão com a impressora
static void fecharConexao(void)
{
    limparTela();

    // Se não estiver conectada, não tem o que fechar
    if (!g_conectada) {
        printf("Nenhuma conexao aberta para ser fechada.\n");
        return;
    }

    printf("\nFechando conexao...\n");

    int retorno = FechaConexaoImpressora();   // Chamo a função da DLL

    if (retorno == 0) {
        printf("Conexao fechada com sucesso!\n");
        g_conectada = 0;   // Atualizo o status global
    } else {
        printf("Falha ao fechar conexao. Codigo: %d\n", retorno);
    }
}



static void imprimirTexto(void)
{
    char texto[500];
    int resultado;
    
    limparTela();
    printf("========== IMPRESSAO DE TEXTO ==========\n\n");
    
    if (!g_conectada) {
        printf("[ERRO] Impressora nao conectada!\n");

        return;
    }
    
    printf("Digite o texto a ser impresso: ");
    fgets(texto, sizeof(texto), stdin);
    texto[strcspn(texto, "\n")] = '\0';
    
    printf("\nImprimindo...\n");
    
    // Imprime o texto (dados, posicao=1:centro, estilo=0:normal, tamanho=0:normal)
    resultado = ImpressaoTexto(texto, 1, 0, 0);
    
    if (resultado == 0) {
        // AvanÃ§a papel e corta
        AvancaPapel(5);
        Corte(1);
        printf("[SUCESSO] Texto impresso!\n");
    } else {
        printf("[ERRO] Falha na impressao. Codigo: %d\n", resultado);
    }

}

static void imprimirQRCode(void)
{
    char dados[300];
    int resultado;
    
    limparTela();
    printf("========== IMPRESSAO DE QR CODE ==========\n\n");
    
    if (!g_conectada) {
        printf("[ERRO] Impressora nao conectada!\n");

        return;
    }
    
    printf("Digite o conteudo do QR Code: ");
    fgets(dados, sizeof(dados), stdin);
    dados[strcspn(dados, "\n")] = '\0';
    
    printf("\nImprimindo QR Code...\n");
    
    // Parametros conforme solicitado: tamanho=6, nivelCorrecao=4
    resultado = ImpressaoQRCode(dados, 6, 4);
    
    if (resultado == 0) {
        AvancaPapel(5);
        Corte(1);
        printf("[SUCESSO] QR Code impresso!\n");
    } else {
        printf("[ERRO] Falha na impressao. Codigo: %d\n", resultado);
    }

}

static void imprimirCodigoBarras(void)
{
    int resultado;
    
    limparTela();
    printf("========== IMPRESSAO DE CODIGO DE BARRAS ==========\n\n");
    
    if (!g_conectada) {
        printf("[ERRO] Impressora nao conectada!\n");

        return;
    }
    
    printf("Imprimindo codigo de barras padrao...\n");
    printf("Codigo: {A012345678912\n");
    printf("Tipo: CODE128 (8)\n\n");
    
    // Parametros conforme especificado no template
    // tipo=8, codigo="{A012345678912", altura=100, largura=2, HRI=3
    resultado = ImpressaoCodigoBarras(8, "{A012345678912", 100, 2, 3);
    
    if (resultado == 0) {
        AvancaPapel(5);
        Corte(1);
        printf("[SUCESSO] Codigo de barras impresso!\n");
    } else {
        printf("[ERRO] Falha na impressao. Codigo: %d\n", resultado);
    }

}

static void imprimirXMLSAT(void)
{
    FILE *arquivo;
    char *conteudo;
    long tamanho;
    int resultado;
    
    limparTela();
    printf("========== IMPRESSAO XML SAT ==========\n\n");
    
    if (!g_conectada) {
        printf("[ERRO] Impressora nao conectada!\n");

        return;
    }
    
    printf("Abrindo arquivo: ./XMLSAT.xml\n");
    
    arquivo = fopen("./XMLSAT.xml", "rb");
    if (!arquivo) {
        printf("[ERRO] Arquivo XMLSAT.xml nao encontrado!\n");
        printf("Certifique-se de que o arquivo existe no diretorio atual.\n");

        return;
    }
    
    // Obtem tamanho do arquivo
    fseek(arquivo, 0, SEEK_END);
    tamanho = ftell(arquivo);
    fseek(arquivo, 0, SEEK_SET);
    
    // Aloca memoria e le o conteudo
    conteudo = (char *)malloc(tamanho + 1);
    if (!conteudo) {
        printf("[ERRO] Falha ao alocar memoria!\n");
        fclose(arquivo);

        return;
    }
    
    fread(conteudo, 1, tamanho, arquivo);
    conteudo[tamanho] = '\0';
    fclose(arquivo);
    
    printf("Arquivo carregado (%ld bytes)\n", tamanho);
    printf("Imprimindo XML SAT...\n\n");
    
    // Imprime XML SAT (conteudo, param=0:padrao)
    resultado = ImprimeXMLSAT(conteudo, 0);
    
    free(conteudo);
    
    if (resultado == 0) {
        AvancaPapel(5);
        Corte(1);
        printf("[SUCESSO] XML SAT impresso!\n");
    } else {
        printf("[ERRO] Falha na impressao. Codigo: %d\n", resultado);
        if (resultado > 0) {
            printf("Erro detectado na linha %d do XML.\n", resultado);
        }
    }

}

static void imprimirXMLCancelamentoSAT(void)
{
    FILE *arquivo;
    char *conteudo;
    long tamanho;
    int resultado;
    
    // Assinatura fornecida no template
    const char *assinatura = 
        "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZ"
        "jbh3btsZhF+sjHqEMR159i4agru9x6KsepK/q0E2e5xlU5cv3m1woYfgHyOkWDNc"
        "SdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQE"
        "Vd7u+TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6"
        "p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+ZfO5b/+vkyAHS6C"
        "YVFCDtYR9Hi5qgdk31v23w==";
    
    limparTela();
    printf("========== IMPRESSAO XML CANCELAMENTO SAT ==========\n\n");
    
    if (!g_conectada) {
        printf("[ERRO] Impressora nao conectada!\n");

        return;
    }
    
    printf("Abrindo arquivo: ./CANC_SAT.xml\n");
    
    arquivo = fopen("./CANC_SAT.xml", "rb");
    if (!arquivo) {
        printf("[ERRO] Arquivo CANC_SAT.xml nao encontrado!\n");
        printf("Certifique-se de que o arquivo existe no diretorio atual.\n");

        return;
    }
    
    // Obtem tamanho do arquivo
    fseek(arquivo, 0, SEEK_END);
    tamanho = ftell(arquivo);
    fseek(arquivo, 0, SEEK_SET);
    
    // Aloca memoria e le o conteudo
    conteudo = (char *)malloc(tamanho + 1);
    if (!conteudo) {
        printf("[ERRO] Falha ao alocar memoria!\n");
        fclose(arquivo);

        return;
    }
    
    fread(conteudo, 1, tamanho, arquivo);
    conteudo[tamanho] = '\0';
    fclose(arquivo);
    
    printf("Arquivo carregado (%ld bytes)\n", tamanho);
    printf("Imprimindo XML de Cancelamento...\n\n");
    
    // Imprime XML Cancelamento (conteudo, assinatura, param=0:padrao)
    resultado = ImprimeXMLCancelamentoSAT(conteudo, assinatura, 0);
    
    free(conteudo);
    
    if (resultado == 0) {
        AvancaPapel(5);
        Corte(1);
        printf("[SUCESSO] XML de Cancelamento impresso!\n");
    } else {
        printf("[ERRO] Falha na impressao. Codigo: %d\n", resultado);
        if (resultado > 0) {
            printf("Erro detectado na linha %d do XML.\n", resultado);
        }
    }
    
}

static void abrirGavetaElginOpc(void)
{
    int resultado;
    
    limparTela();
    printf("========== ABRIR GAVETA ELGIN ==========\n\n");
    
    if (!g_conectada) {
        printf("[ERRO] Impressora nao conectada!\n");

        return;
    }
    
    printf("Abrindo gaveta Elgin...\n");
    printf("Parametros: pino=1, tempoInicio=50ms, tempoFim=50ms\n\n");
    
    // Conforme especificado no template: pino=1, ti=50, tf=50
    resultado = AbreGavetaElgin(1, 50, 50);
    
    if (resultado == 0) {
        printf("[SUCESSO] Gaveta Elgin aberta!\n");
    } else {
        printf("[ERRO] Falha ao abrir gaveta. Codigo: %d\n", resultado);
    }
    

}

static void abrirGavetaOpc(void)
{
    int resultado;
    
    limparTela();
    printf("========== ABRIR GAVETA PADRAO ==========\n\n");
    
    if (!g_conectada) {
        printf("[ERRO] Impressora nao conectada!\n");

        return;
    }
    
    printf("Abrindo gaveta padrao...\n");
    printf("Parametros: pino=1, tempoInicio=5ms, tempoFim=10ms\n\n");
    
    // Conforme especificado no template: pino=1, ti=5, tf=10
    resultado = AbreGaveta(1, 5, 10);
    
    if (resultado == 0) {
        printf("[SUCESSO] Gaveta aberta!\n");
    } else {
        printf("[ERRO] Falha ao abrir gaveta. Codigo: %d\n", resultado);
    }

}

static void emitirSinalSonoro(void)
{
    int resultado;
    
    limparTela();
    printf("========== SINAL SONORO ==========\n\n");
    
    if (!g_conectada) {
        printf("[ERRO] Impressora nao conectada!\n");

        return;
    }
    
    printf("Emitindo sinal sonoro...\n");
    printf("Parametros: 4 beeps, 50ms ligado, 5ms intervalo\n\n");
    
    // Conforme especificado no template: qtd=4, tempoInicio=50, tempoFim=5
    resultado = SinalSonoro(4, 50, 5);
    
    if (resultado == 0) {
        printf("[SUCESSO] Sinal sonoro emitido!\n");
    } else {
        printf("[ERRO] Falha ao emitir sinal. Codigo: %d\n", resultado);
    }

}

static void imprimirCupomDemo(void)
{
    int resultado;
    
    limparTela();
    printf("========== CUPOM DEMONSTRATIVO ==========\n\n");
    
    if (!g_conectada) {
        printf("[ERRO] Impressora nao conectada!\n");

        return;
    }
    
    printf("Imprimindo cupom demonstrativo...\n\n");
    
    // Cabecalho
    ImpressaoTexto("LOJA EXEMPLO LTDA", 1, 0, 1);
    ImpressaoTexto("Rua das Flores, 123 - Centro", 1, 0, 0);
    ImpressaoTexto("CNPJ: 00.000.000/0001-00", 1, 0, 0);
    AvancaPapel(2);
    
    ImpressaoTexto("====== CUPOM FISCAL ======", 1, 0, 0);
    AvancaPapel(2);
    
    // Itens
    ImpressaoTexto("Item  Descricao      Qtd   Valor", 0, 0, 0);
    ImpressaoTexto("1     Produto A      2     R$ 10,00", 0, 0, 0);
    ImpressaoTexto("2     Produto B      1     R$ 25,50", 0, 0, 0);
    ImpressaoTexto("3     Produto C      3     R$ 15,00", 0, 0, 0);
    AvancaPapel(2);
    
    // Total
    ImpressaoTexto("TOTAL:         R$ 50,50", 2, 0, 1);
    AvancaPapel(2);
    
    // QR Code
    ImpressaoTexto("Consulte pela chave:", 1, 0, 0);
    ImpressaoQRCode("https://exemplo.com/nfce/12345", 6, 4);
    AvancaPapel(2);
    
    // Codigo de barras
    ImpressaoCodigoBarras(8, "{A012345678912", 80, 2, 3);
    AvancaPapel(3);
    
    // Rodape
    ImpressaoTexto("Obrigado pela preferencia!", 1, 0, 0);
    ImpressaoTexto("Volte sempre!", 1, 0, 0);
    AvancaPapel(4);
    
    // Corte e sinal sonoro
    Corte(1);
    SinalSonoro(2, 100, 200);
    
    printf("[SUCESSO] Cupom demonstrativo impresso!\n");

}

/* ======================= FunÃ§Ã£o principal ======================= */
int main(void)
{
    int opcao = 0;
    
    printf("Iniciando sistema PDV Elgin...\n");
    printf("Carregando biblioteca E1_Impressora01.dll...\n\n");
    
    if (!carregarFuncoes()) {
        printf("\n[ERRO FATAL] Nao foi possivel carregar a DLL!\n");
        printf("Verifique se E1_Impressora01.dll esta no diretorio.\n");

        return 1;
    }
    
    printf("[OK] Biblioteca carregada com sucesso!\n");

    
    // Loop principal do menu
    while (1) {
        limparTela();
        exibirCabecalho();
        exibirMenu();
        
        if (scanf("%d", &opcao) != 1) {
            flush_entrada();
            printf("\n[ERRO] Opcao invalida!\n");
  
            continue;
        }
        flush_entrada();
        
        switch (opcao) {
            case 1:
                configurarConexao();
                break;
                
            case 2:
                abrirConexao();
                break;
                
            case 3:
                fecharConexao();
                break;
                
            case 4:
                imprimirTexto();
                break;
                
            case 5:
                imprimirQRCode();
                break;
                
            case 6:
                imprimirCodigoBarras();
                break;
                
            case 7:
                imprimirXMLSAT();
                break;
                
            case 8:
                imprimirXMLCancelamentoSAT();
                break;
                
            case 9:
                abrirGavetaElginOpc();
                break;
                
            case 10:
                abrirGavetaOpc();
                break;
                
            case 11:
                emitirSinalSonoro();
                break;
                
            case 12:
                imprimirCupomDemo();
                break;
                
            case 0:
                limparTela();
                printf("\n");
                printf("========================================\n");
                printf("    Encerrando Sistema PDV Elgin...    \n");
                printf("========================================\n\n");
                
                if (g_conectada) {
                    printf("Fechando conexao com impressora...\n");
                    fecharConexao();
                }
                
                printf("Liberando recursos...\n");
                liberarBiblioteca();
                
                printf("\n[OK] Sistema encerrado com sucesso!\n");
                printf("Obrigado por utilizar o sistema!\n\n");
                return 0;
                
            default:
                printf("\n[ERRO] Opcao invalida! Tente novamente.\n");

                break;
        }
    }
    
    return 0;
}

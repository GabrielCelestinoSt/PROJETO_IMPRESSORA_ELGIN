#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/* ======================= Config DLL ======================= */
static HMODULE g_hDll = NULL;

/* Convenção de chamada (Windows): __stdcall */
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

/* ======================= Configuração ======================= */
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

// função de sistema que permite o usuário digitar na mesma linha que o código 
// Lê o comando até encontrar uma quebra de linha('\n') ou o fim do arquivo ('End Of File (EOF)')
static void flush_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

// função de sistema que limpa a tela
// ela executa o comando 'system("cls")'
static void limparTela(void) {
    system("cls");
}

/* ======================= Funções para manipular a DLL ======================= */
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

/* ======================= Funções implementadas ======================= */

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

static void configurarConexao(void)
{
    limparTela();
    printf("========== CONFIGURACAO DE CONEXAO ==========\n\n");
    
    printf("Tipo de conexao:\n");
    printf("  1 - USB\n");
    printf("  5 - Rede (IP)\n");
    printf("Escolha: ");
    scanf("%d", &g_tipo);
    flush_entrada();
    
    printf("\nModelo da impressora (i7/i8/i9): ");
    fgets(g_modelo, sizeof(g_modelo), stdin);
    g_modelo[strcspn(g_modelo, "\n")] = '\0';
    
    if (g_tipo == 1) {
        strcpy(g_conexao, "USB");
        g_parametro = 0;
    } else if (g_tipo == 5) {
        printf("\nEndereco IP da impressora: ");
        fgets(g_conexao, sizeof(g_conexao), stdin);
        g_conexao[strcspn(g_conexao, "\n")] = '\0';
        
        printf("Porta TCP/IP (ex: 9100): ");
        scanf("%d", &g_parametro);
        flush_entrada();
    }
    
    printf("\n[OK] Configuracao salva!\n");
    printf("  Tipo: %d\n", g_tipo);
    printf("  Modelo: %s\n", g_modelo);
    printf("  Conexao: %s\n", g_conexao);
    printf("  Parametro: %d\n", g_parametro);

}

static void abrirConexao(void)
{
    limparTela();
    printf("========== ABRINDO CONEXAO ==========\n\n");
    
    if (g_conectada) {
        printf("[AVISO] Ja existe uma conexao ativa!\n");

        return;
    }
    
    printf("Conectando na impressora...\n");
    printf("  Tipo: %d\n", g_tipo);
    printf("  Modelo: %s\n", g_modelo);
    printf("  Conexao: %s\n", g_conexao);
    printf("  Parametro: %d\n\n", g_parametro);
    
    int resultado = AbreConexaoImpressora(g_tipo, g_modelo, g_conexao, g_parametro);
    
    if (resultado == 0) {
        printf("[SUCESSO] Conexao estabelecida!\n");
        g_conectada = 1;
        
        // Inicializa a impressora
        InicializaImpressora();
        printf("[OK] Impressora inicializada.\n");
    } else {
        printf("[ERRO] Falha ao conectar. Codigo: %d\n", resultado);
        g_conectada = 0;
    }

}

static void fecharConexao(void)
{
    limparTela();
    printf("========== FECHANDO CONEXAO ==========\n\n");
    
    if (!g_conectada) {
        printf("[AVISO] Nenhuma conexao ativa!\n");

        return;
    }
    
    printf("Encerrando conexao com a impressora...\n");
    
    int resultado = FechaConexaoImpressora();
    
    if (resultado == 0) {
        printf("[SUCESSO] Conexao encerrada!\n");
        g_conectada = 0;
    } else {
        printf("[ERRO] Falha ao fechar conexao. Codigo: %d\n", resultado);
    }

}

static void imprimirTexto(void)
{
    char texto[500];
    int resultado;
    
    limparTela();
    printf("========== IMPRESSAO DE TEXTO ==========\n\n");
    
    // Primeiro ele checa se a impressora tá conectada.
    // Se não tiver, já reclama e cai fora da função.
    if (!g_conectada) {
        printf("[ERRO] Impressora nao conectada!\n");

        return;
    }
    // Pede pro usuário digitar o texto que quer imprimir.
    printf("Digite o texto a ser impresso: ");
    fgets(texto, sizeof(texto), stdin);
    texto[strcspn(texto, "\n")] = '\0'; // Remove o ENTER do final
    
    printf("\nImprimindo...\n");
    
   // Manda o texto pra impressora. aqui ele define:
    // posicao = 1 (centralizado), estilo = 0 (normal), tamanho = 0 (normal)
    resultado = ImpressaoTexto(texto, 1, 0, 0);
    // Se deu tudo certo (resultado == 0), ele avança o papel e corta.
    if (resultado == 0) {
        // Avança papel e corta
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
    // Verifica se a impressora está conectada
    if (!g_conectada) {
        printf("[ERRO] Impressora nao conectada!\n");

        return;
    }
    
    printf("Abrindo arquivo: ./XMLSAT.xml\n");
    // Abrir arquivo se 'XMLSAT.xml' for encontrado na pasta
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
    
    // Aloca memoria
    conteudo = (char *)malloc(tamanho + 1);
    if (!conteudo) {
        printf("[ERRO] Falha ao alocar memoria!\n");
        fclose(arquivo);

        return;
    }
    // lê o conteudo
    fread(conteudo, 1, tamanho, arquivo);
    conteudo[tamanho] = '\0';
    fclose(arquivo);
    
    printf("Arquivo carregado (%ld bytes)\n", tamanho);
    printf("Imprimindo XML SAT...\n\n");
    
    // Imprime XML SAT (conteudo, param=0:padrao)
    resultado = ImprimeXMLSAT(conteudo, 0);
    
    free(conteudo);
    
    if (resultado == 0) { // Se 0 - Sucesso | Se !=0 - Falha
        AvancaPapel(5); // Avança o papelzinho
        Corte(1); // Corta o papelzinho
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
    //Verificar se a impressora está conectada
    if (!g_conectada) {
        printf("[ERRO] Impressora nao conectada!\n");

        return;
    }
    
    printf("Abrindo arquivo: ./CANC_SAT.xml\n");
    
    arquivo = fopen("./CANC_SAT.xml", "rb");
    if (!arquivo) { // Verificar se "CANC_SAT.xml" está na pasta
        printf("[ERRO] Arquivo CANC_SAT.xml nao encontrado!\n");
        printf("Certifique-se de que o arquivo existe no diretorio atual.\n");

        return;
    }
    
    // Obtem tamanho do arquivo
    fseek(arquivo, 0, SEEK_END);
    tamanho = ftell(arquivo);
    fseek(arquivo, 0, SEEK_SET);
    
    // Aloca memoria
    conteudo = (char *)malloc(tamanho + 1);
    if (!conteudo) {
        printf("[ERRO] Falha ao alocar memoria!\n");
        fclose(arquivo);

        return;
    }
    // lê o conteudo
    fread(conteudo, 1, tamanho, arquivo);
    conteudo[tamanho] = '\0';
    fclose(arquivo);
    
    printf("Arquivo carregado (%ld bytes)\n", tamanho);
    printf("Imprimindo XML de Cancelamento...\n\n");
    
    // Imprime XML Cancelamento (conteudo, assinatura, param=0:padrao)
    resultado = ImprimeXMLCancelamentoSAT(conteudo, assinatura, 0); // Imprime
    
    free(conteudo);
    
    if (resultado == 0) { // Se 0 - sucesso | se !0 - falha
        AvancaPapel(5); // Avançar o papelzinho
        Corte(1); // Corta o papelzinho
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
    // Criação da variável "resultado" para armazenar o retorno da DLL
    int resultado;
    
    // Chama a função de limparTela
    limparTela();
    printf("========== ABRIR GAVETA ELGIN ==========\n\n");
    

    // Teste condicional para verificar se a impressora está conectada
    // "g_conectada" é uma flag global que mostra o status da conexão  
    if (!g_conectada) {
        printf("[ERRO] Impressora nao conectada!\n");

        return;
    }
    
    printf("Abrindo gaveta Elgin...\n");
    printf("Parametros: pino=1, tempoInicio=50ms, tempoFim=50ms\n\n");
    
    // parte que chama a função já criada na DLL 
    // Conforme especificado no template: pino=1, ti=50, tf=50
    resultado = AbreGavetaElgin(1, 50, 50);


    // Condicional para informar o usuário se a operação foi bem sucedida ou não
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
    // Verifica se a impressora está conectada
    if (!g_conectada) {
        printf("[ERRO] Impressora nao conectada!\n");

        return;
    }
    
    printf("Abrindo gaveta padrao...\n");
    printf("Parametros: pino=1, tempoInicio=5ms, tempoFim=10ms\n\n");
    
    // Conforme especificado no template: pino=1, ti=5, tf=10
    resultado = AbreGaveta(1, 5, 10); //Abrindo gaveta com os parametros
    
    if (resultado == 0) { // resultado == 0 | abrir gaveta
        printf("[SUCESSO] Gaveta aberta!\n");
    } else { // resultado != 0 | mostrar código de erro
        printf("[ERRO] Falha ao abrir gaveta. Codigo: %d\n", resultado);
    }

}

static void emitirSinalSonoro(void)
{
    int resultado;
    
    limparTela();
    printf("========== SINAL SONORO ==========\n\n");
    // Verifica se a impressora está conectada
    if (!g_conectada) {
        printf("[ERRO] Impressora nao conectada!\n");

        return;
    }
    
    printf("Emitindo sinal sonoro...\n");
    printf("Parametros: 4 beeps, 50ms ligado, 5ms intervalo\n\n");
    
    // Conforme especificado no template: qtd=4, tempoInicio=50, tempoFim=5
    resultado = SinalSonoro(4, 50, 5); // Emitir sinal sonoro (beep) a partir dos parametros 
    
    if (resultado == 0) { // Emitir sinal sonoro
        printf("[SUCESSO] Sinal sonoro emitido!\n");
    } else { // resultado != | Mostrar código da falha
        printf("[ERRO] Falha ao emitir sinal. Codigo: %d\n", resultado);
    }

}


/* ======================= Função principal ======================= */
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


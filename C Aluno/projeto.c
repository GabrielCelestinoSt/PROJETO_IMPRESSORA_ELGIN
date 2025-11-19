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
            fprintf(stderr, "Falha ao resolver símbolo %s (erro=%lu)\n",         \
                    #name, GetLastError());                                      \
            return 0;                                                            \
        }                                                                        \
    } while (0)

static void flush_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}
/* ======================= Funções para manipular CMD ======================= */

void limparTela(void) {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pausar(void) {
    printf("Pressione ENTER para continuar...");
    flush_entrada(); // Garante que a entrada está limpa
    getchar();
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

/* ======================= Funções a serem implementadas pelos alunos ======================= */

static int exibirMenu()
{
    int EscolhaOpcao;

    printf("========ESCOLHE UMA OPCAO========\n");
    printf("1 - Configurar Conexao\n");
    printf("2 - Abrir Conexao\n");
    printf("3 - Impressao Texto\n");
    printf("4 - Impressao QRCode\n");
    printf("5 - Impressao Cod Barras\n");
    printf("6 - Impressao XML SAT\n");
    printf("7 - Impressao XML Canc SAT\n");
    printf("8 - Abrir Gaveta Elgin\n");
    printf("9 - Abrir Gaveta\n");
    printf("10 - Sinal Sonoro\n");
    printf("0 - Fechar Conexao e Sair\n");
    scanf("%d" , &EscolhaOpcao);

    return EscolhaOpcao;
}

static void configurarConexao(void)
{
    int tipo;
    const char *modelo; //endereço na memoria 
    const char *conexao;
    int parametro;
    char resp;
    
    char modelo_buf[50]; // para armazenar o ponteiro 
    char conexao_buf[100];

   do {
        printf("--------------------------------------------\n");
        printf("| Valor | Referencia                       |\n");
        printf("--------------------------------------------\n");
        printf("|   1   | USB                              |\n");
        printf("|   2   | RS232                            |\n");
        printf("|   3   | TCP/IP                           |\n");
        printf("|   4   | Bluetooth                        |\n");
        printf("|   5   | Impressoras acopladas (Android)  |\n");
        printf("--------------------------------------------\n");
        printf("Digite o tipo de comunicacao: ");
        scanf("%d", &g_tipo);

        if (g_tipo < 1 || g_tipo > 5)
            printf("\nTipo invalido! Digite novamente.\n");

    } while (g_tipo < 1 || g_tipo > 5);


    if (g_tipo != 5){
        int modeloValido = 0; // variavel para fazer a validação do modelo digitado 
        do {
            printf("-----------------------------------\n");
            printf("|              Modelos            |\n");
            printf("-----------------------------------\n");
            printf("| i7                              |\n");
            printf("| i7 Plus                         |\n");
            printf("| i8                              |\n");
            printf("| i9                              |\n");
            printf("| ix                              |\n");
            printf("| Fitpos                          |\n");
            printf("| BK-T681                         |\n");
            printf("| MP-4200 (Para modelos TH e ADV) |\n");
            printf("| MP-4200 HS                      |\n");
            printf("| MK                              |\n");
            printf("| MP-2800                         |\n");
            printf("-----------------------------------\n");
            printf("Digite o modelo exatamente como na tabela: ");
            scanf(" %[^\n]", g_modelo);

            if (strcmp(g_modelo, "i7") == 0 || //fazendo a validação dos modelos 
                strcmp(g_modelo, "i7 Plus") == 0 || //comparando os caracteres de cada string um a um e retornando um valor inteiro
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
                modeloValido = 1;
            }
            else
            {
                printf("\nModelo invalido! Digite novamente.\n");
            }
        } while (!modeloValido);
    }
    else
    {
        strcpy(g_modelo, ""); // copia a string na memoria aonde ela é a origem para outra que seriaa o destino 
    }


    switch (g_tipo) // copia a string na memoria aonde ela é a origem para outra que seriaa o destino 
    {
        case 1: 
		strcpy(g_conexao, "USB"); 
		break;
        case 2: 
		strcpy(g_conexao, "COM2");
		 break;
        case 3: 
		strcpy(g_conexao, "192.168.0.20"); 
		break;
        case 4: 
		strcpy(g_conexao, "AA:BB:CC:DD:EE:FF"); 
		break;
        case 5: 
		strcpy(g_conexao, ""); 
		break;
    }

    
    if (g_tipo != 5)
    {
        printf("\nConexao padrao: %s\n", g_conexao);
        printf("Deseja alterar? (s/n): ");
        scanf(" %c", &resp);

        if (resp == 's' || resp == 'S')
        {
            printf("Digite a nova conexao: ");
            scanf(" %[^\n]", g_conexao);
        }
    }


    if (g_tipo == 2)
    {
        printf("\nDigite o baudrate: ");
        scanf("%d", &g_parametro);
    }
    else if (g_tipo == 3)
    {
        printf("\nDigite a porta TCP ***: ");
        scanf("%d", &g_parametro);
    }
    else
    {
        g_parametro = 0;
    }


    printf("\nResumo da configuracao:\n");
    printf("--------------------------------------------\n");
    printf("Tipo:       %d\n", g_tipo);
    printf("Modelo:     %s\n", g_tipo == 5 ? "(vazio)" : g_modelo);
    printf("Conexao:    %s\n", g_conexao[0] ? g_conexao : "(vazio)");
    printf("Parametro:  %d\n", g_parametro);
    printf("--------------------------------------------\n");
}



static void abrirConexao(void)
{
    printf("\nAbrindo Conexao...\n");

    int retorno = AbreConexaoImpressora(g_tipo, g_modelo, g_conexao, g_parametro); // esta chamando as variaveis globais 

    if (retorno == 0) {
        printf("Conexao aberta com sucesso!\n");
        g_conectada = 1;
        return;
    }

    printf("\nErro ao abrir conexao! Codigo: %d\n", retorno);
    printf("Configuracao incorreta ou dispositivo nao encontrado.\n");

    char opcao;
    printf("Deseja configurar a conexao? (s/n): ");
    scanf(" %c", &opcao);

    if (opcao == 's' || opcao == 'S') {

        configurarConexao();

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
 
    // TODO: chamar AbreConexaoImpressora e validar retorno
}

static void fecharConexao(void)
{
    if (!g_conectada) {
        printf("Nenhuma conexao aberta para ser fechada.\n");
        return;
    }

    printf("\nFechando conexao...\n");
    int retorno = FechaConexaoImpressora();

    if (retorno == 0) {
        printf("Conexao fechada com sucesso!\n");
        g_conectada = 0;
    } else {
        printf("Falha ao fechar conexao. Codigo: %d\n", retorno);
    }
}

static void imprimirTexto(void)
{
  if (!g_conectada) {  //Verifica se a impressora est? conectada
        printf("ERRO: Nao e possivel imprimir. A impressora nao esta conectada.\n");
        return;
    }

    char texto[512];

    printf("\nDigite o texto que deseja imprimir:\n> "); //L? o texto digitado pelo usu?rio
    flush_entrada();
    fgets(texto, sizeof(texto), stdin);

    // Remove quebra de linha autom?tica
    texto[strcspn(texto, "\n")] = '\0';

    int ret = ImpressaoTexto(texto, 0, 0, 0); //Envia o texto diretamente para a impressora com ImpressaoTexto()
    if (ret != 0) {
        printf("Erro ao imprimir o texto! Codigo retornado: %d\n", ret);
        return;
    }

    AvancaPapel(10);  // empurra o papel para frente
    Corte(0);          // 0 = corte total

    printf("\nTexto impresso com sucesso!\n");
    
    // TODO: solicitar texto do usu?rio e chamar ImpressaoTexto
    // incluir AvancaPapel e Corte no final
}

static void imprimirQRCode(void)
{
    char buffer[1024];

    flush_entrada(); // limpa qualquer '\n' remanescente
    printf("Digite o link para o QRCode (apenas o link). Pressione ENTER para cancelar: ");

    if (!fgets(buffer, sizeof(buffer), stdin)) {
        // erro de leitura -> retornar ao menu
        return;
    }

    // remover newline final
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
        len--;
    }

    // Se o usuário apertou ENTER sem digitar nada, len == 0 -> não imprime nada, volta ao menu
    if (len == 0) {
        printf("Entrada vazia. Nenhum QRCode será impresso. Retornando ao menu...\n");
        return;
    }

    // Aqui nós **não** chamamos ImpressaoQRCode imediatamente.
    // Em vez disso, gravamos os dados pendentes em arquivo para que outra rotina
    // (por exemplo, a implementação de imprimirXMLSAT) possa ler e, então, efetuar a impressão
    // com os parâmetros exigidos: tamanho = 6, taxaErro = 4 (máxima).
    FILE *f = fopen("./pending_qrcode.txt", "w");
    if (!f) {
        printf("Erro ao gravar pending_qrcode.txt\n");
        return;
    }

    // Formato: primeira linha -> link; segunda -> tamanho; terceira -> taxaErro
    fprintf(f, "%s\n", buffer);
    fprintf(f, "%d\n", 6);  // tamanho fixo = 6
    fprintf(f, "%d\n", 4);  // taxa de erro máxima (conforme convenção usada no placeholder)
    fclose(f);

    printf("QRCode salvo como pendente (./pending_qrcode.txt). Será impresso quando imprimirXMLSAT for executado.\n");
}

/* =========================
   Função: imprimirCodigoBarras
     * Quando usuário escolher a função 5, perguntar SOMENTE pelos números do código de barras.
     * Se o usuário apenas pressionar ENTER (entrada vazia), NÃO imprimir nada e retornar ao menu.
     * Caso o usuário informe dados, NÃO executar a impressão agora, salvar em arquivo para impressão posterior.
     * Usa os parâmetros padrão sugeridos no placeholder (tipo=8, largura=100, altura=2, posTexto=3).
     * Aqui salvamos em `./pending_barcode.txt` as linhas: numeros, tipo(8), largura(100), altura(2), posTexto(3).
   ========================= */
static void imprimirCodigoBarras(void)
{
    char buffer[512];

    flush_entrada(); // limpa qualquer '\n' remanescente
    printf("Digite os NUMEROS do Codigo de Barras (somente digitos). Pressione ENTER para cancelar: ");

    if (!fgets(buffer, sizeof(buffer), stdin)) {
        // erro de leitura -> retornar ao menu
        return;
    }

    // remover newline final
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
        len--;
    }

    // Se o usuário apertou ENTER sem digitar nada, len == 0 -> não imprime nada, volta ao menu
    if (len == 0) {
        printf("Entrada vazia. Nenhum Codigo de Barras será impresso. Retornando ao menu...\n");
        return;
    }

    // Salvar os parâmetros pendentes em arquivo para impressão posterior.
    // Usamos os valores sugeridos no placeholder: tipo=8, largura=100, altura=2, posTexto=3.
    FILE *f = fopen("./pending_barcode.txt", "w");
    if (!f) {
        printf("Erro ao gravar pending_barcode.txt\n");
        return;
    }

    // Formato: primeira linha -> numeros; segunda -> tipo; terceira -> largura; quarta -> altura; quinta -> posTexto
    fprintf(f, "%s\n", buffer);
    fprintf(f, "%d\n", 8);    // tipo sugerido
    fprintf(f, "%d\n", 100);  // largura/escala
    fprintf(f, "%d\n", 2);    // altura/param
    fprintf(f, "%d\n", 3);    // posicao do texto (exemplo)
    fclose(f);

    printf("Codigo de Barras salvo como pendente (./pending_barcode.txt). Será impresso quando imprimirXMLSAT for executado.\n");
}

static void imprimirXMLSAT(void)
{
    // TODO: ler o arquivo ./XMLSAT.xml e enviar via ImprimeXMLSAT
    // incluir AvancaPapel e Corte no final
}

static void imprimirXMLCancelamentoSAT(void)
{
    // TODO: ler o arquivo ./CANC_SAT.xml e chamar ImprimeXMLCancelamentoSAT
    // incluir AvancaPapel e Corte no final
    
	/*usar assinatura abaixo:
        "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZ"
        "jbh3btsZhF+sjHqEMR159i4agru9x6KsepK/q0E2e5xlU5cv3m1woYfgHyOkWDNc"
        "SdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQE"
        "Vd7u+TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6"
        "p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+ZfO5b/+vkyAHS6C"
        "YVFCDtYR9Hi5qgdk31v23w==";
        */
}

static void abrirGavetaElginOpc(void)
{
    int resultado;
    
    limparTela();
    printf("========== ABRIR GAVETA ELGIN ==========\n\n");
    
    if (!g_conectada) {
        printf("[ERRO] Impressora nao conectada!\n");
        pausar();
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
    // TODO: chamar AbreGaveta(1, 5, 10)
}

static void emitirSinalSonoro(void)
{
    // TODO: chamar SinalSonoro(4, 50, 5)
}

/* ======================= Função principal ======================= */
int main(void)
{
    if (!carregarFuncoes()) {
    	
    	printf("Erro!");
        return 1;
    }

    int opcao = 0;
    while (1) {
    	
    	opcao = exibirMenu();
    	
 	switch (opcao) {
        case 1:
        configurarConexao(); 
        break;

        case 2:
        abrirConexao(); 
        break;

        case 3:
        imprimirTexto(); 
        break;

    	case 4:
        imprimirQRCode(); 
        break;

        case 5:
        imprimirCodigoBarras(); 
        break;

        case 6:
        imprimirXMLSAT(); 
        break;

        case 7:
        imprimirXMLCancelamentoSAT(); 
        break;

        case 8:
        abrirGavetaElginOpc(); 
        break;

        case 9:
        abrirGavetaOpc(); 
        break;

        case 10:
        emitirSinalSonoro(); 
        break;

        case 0:
        fecharConexao();
        printf("Saindo do sistema...\n");
        return 0; 

        default:
        printf("Opcao Invalida!\n");
        break;
        }
    }

    return 0; 
}



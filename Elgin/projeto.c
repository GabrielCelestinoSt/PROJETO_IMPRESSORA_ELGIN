// only for implement in principal code
static void imprimirXMLCancelamentoSAT(void)
{
    if (!g_conectada) {
        printf("ERRO: Nao e possivel imprimir. A impressora nao esta conectada.\n");
        return;
    }

    if (!ImprimeXMLCancelamentoSAT || !AvancaPapel || !Corte) {
        printf("Funcoes de XML Cancelamento SAT nao estao carregadas.\n");
        return;
    }

    const char *filename = "./CANC_SAT.xml";
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Nao foi possivel abrir o arquivo %s\n", filename);
        return;
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (len <= 0) {
        fclose(f);
        printf("Arquivo vazio ou erro lendo.\n");
        return;
    }
    char *buffer = (char *)malloc((size_t)len + 1);
    if (!buffer) {
        fclose(f);
        printf("Memoria insuficiente.\n");
        return;
    }
    if (fread(buffer, 1, (size_t)len, f) != (size_t)len) {
        fclose(f);
        free(buffer);
        printf("Erro lendo arquivo.\n");
        return;
    }
    buffer[len] = '\0';
    fclose(f);

    const char *assinatura =
        "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZ"
        "jbh3btsZhF+sjHqEMR159i4agru9x6KsepK/q0E2e5xlU5cv3m1woYfgHyOkWDNc"
        "SdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQE"
        "Vd7u+TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6"
        "p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+ZfO5b/+vkyAHS6C"
        "YVFCDtYR9Hi5qgdk31v23w==";

    int ret = ImprimeXMLCancelamentoSAT(buffer, assinatura, 0);
    free(buffer);

    if (ret != 0) {
        printf("Erro ao imprimir XML Cancelamento SAT! Codigo retornado: %d\n", ret);
        return;
    }

    AvancaPapel(10);
    Corte(0);

    printf("XML Cancelamento SAT impresso com sucesso!\n");
}
// 
static void abrirGavetaOpc(void)
{
    if (!g_conectada) {
        printf("ERRO: Nao e possivel abrir gaveta. A impressora nao esta conectada.\n");
        return;
    }

    if (!AbreGaveta) {
        printf("Funcao AbreGaveta nao carregada.\n");
        return;
    }

    int ret = AbreGaveta(1, 50, 50);
    if (ret != 0) {
        printf("Erro ao abrir gaveta! Codigo: %d\n", ret);
    } else {
        printf("Gaveta aberta com sucesso!\n");
    }
}
//
static void emitirSinalSonoro(void)
{
    if (!g_conectada) {
        printf("ERRO: Nao e possivel emitir sinal. A impressora nao esta conectada.\n");
        return;
    }

    if (!SinalSonoro) {
        printf("Funcao SinalSonoro nao carregada.\n");
        return;
    }

    int ret = SinalSonoro(4, 50, 5);
    if (ret != 0) {
        printf("Erro ao emitir sinal sonoro! Codigo: %d\n", ret);
    } else {
        printf("Sinal sonoro emitido com sucesso!\n");
    }
}
//
static void imprimirXMLSAT(void)
{
    if (!g_conectada) {
        printf("ERRO: Nao e possivel imprimir. A impressora nao esta conectada.\n");
        return;
    }

    if (!ImprimeXMLSAT || !AvancaPapel || !Corte) {
        printf("Funcoes de XML SAT nao estao carregadas.\n");
        return;
    }

    const char *filename = "./XMLSAT.xml";
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Nao foi possivel abrir o arquivo %s\n", filename);
        return;
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (len <= 0) {
        fclose(f);
        printf("Arquivo vazio ou erro lendo.\n");
        return;
    }
    char *buffer = (char *)malloc((size_t)len + 1);
    if (!buffer) {
        fclose(f);
        printf("Memoria insuficiente.\n");
        return;
    }
    if (fread(buffer, 1, (size_t)len, f) != (size_t)len) {
        fclose(f);
        free(buffer);
        printf("Erro lendo arquivo.\n");
        return;
    }
    buffer[len] = '\0';
    fclose(f);

    int ret = ImprimeXMLSAT(buffer, 0);
    free(buffer);

    if (ret != 0) {
        printf("Erro ao imprimir XML SAT! Codigo retornado: %d\n", ret);
        return;
    }

    AvancaPapel(10);
    Corte(0);

    printf("XML SAT impresso com sucesso!\n");
}

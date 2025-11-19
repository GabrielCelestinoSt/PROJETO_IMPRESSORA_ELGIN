static void imprimirTexto(void)
{
  if (!g_conectada) {  //Verifica se a impressora está conectada
        printf("ERRO: Não é possível imprimir. A impressora não está conectada.\n");
        return;
    }

    char texto[512];

    printf("\nDigite o texto que deseja imprimir:\n> "); //Lê o texto digitado pelo usuário
    flush_entrada();
    fgets(texto, sizeof(texto), stdin);

    // Remove quebra de linha automática
    texto[strcspn(texto, "\n")] = '\0';

    int ret = ImpressaoTexto(texto, 0, 0, 0); //Envia o texto diretamente para a impressora com ImpressaoTexto()
    if (ret != 0) {
        printf("Erro ao imprimir o texto! Código retornado: %d\n", ret);
        return;
    }

    Corte(0);          // 0 = corte total

    printf("\nTexto impresso com sucesso!\n");
}



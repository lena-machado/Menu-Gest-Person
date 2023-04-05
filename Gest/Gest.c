#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

/*
Implemente uma aplicação completa denominada Gest.c que contenha um Menu com as seguintes opções:
	1. Inserir Registro
	2. Alterar Registro
	3. Apagar Registro
	4. Listar Registros
	5. Pesquisas
	0. Sair

A opção 5 deverá apresentar um novo SubMenu com as seguintes opções:
	1. Pesquisar por Intervalo de Idades
	2. Pesquisar por Nome
	0. Voltar

Implemente todas as funcionalidades presentes nas opções de cada um dos Menus.
*/
#define TAM_NOME 200
#define FICH "Dados.dat" // ficheiro com dados

#define OP_INSERIR     '1'
#define OP_ALTERAR     '2'
#define OP_APAGAR      '3'
#define OP_LISTAR      '4'
#define OP_PESQUISAR   '5'

#define OP_SAIR		   '0'

#define OP_PESQ_IDADE  '1'
#define OP_PESQ_NOME   '2'

FILE* fp; // variavel global pois é útil ao longo do progrma

char* MainMenu[] = {
		"1. Inserir Registo",
		"2. Alterar Registo",
		"3. Apagar Registo",
		"4. Listar Registos",
		"5. Pesquisas",
		"0. Sair",
	NULL // acabaram as opções
};

char* PesqMenu[] = {
	"1. Pesquisar por Intervalo de Idades",
	"2. Pesquisar por Nome",
	"0. Voltar",
	NULL // acabaram as opções
};

typedef struct {
	char nome[30 + 1];
	int idade;
	float salario;
	bool visivel; // indica se o registo está apagado ou não T - visivel / F - oculto
}PESSOA;

// prototipos das funçoes
void Message(char* msg);

// Lê os dados de um registo pelo utilizador
void Ler_Pessoa(PESSOA* p) {
	char _;
	_ = getchar();

	printf("\nNome:\t");
	gets_s(p->nome, 31);

	printf("Idade:\t");
	scanf_s("%d", &p->idade);

	printf("Salário:\t");
	scanf_s("%f", &p->salario);

	p->visivel = true;
	fflush(stdin);
}
// verifica se o ficheiro já existe

// Adicionar uma pessoa ao ficheiro
void Adiciona_Pessoa(PESSOA p) {
	fseek(fp, 0L, SEEK_END);
	if (fwrite(&p, sizeof(p), 1, fp) != 1)
		Message("Adicionar Pessoa: Falhou a escrita do Registo!");
	Message("Registo adicionado com sucesso!");
}


void Message(char* msg) {
	char _;
	printf(msg);
	fflush(stdin);
	_ = getchar();
	_ = getchar();
}
void Mostrar_Pessoa(PESSOA p) {
	printf("\nNome: %-30s\n", p.nome);
	printf("Idade: %3d\n", p.idade);
	printf("Salário: %10.2f euros\n", p.salario);
}

/*
Verifica se o ficheiro já existe.Se não existir, cria-o.
Se já existir, abre-o em modo Leitura e Escrita (r + b)
*/

void Inic() {
	fopen_s(&fp, FICH, "r+b"); // tentar abrir
	if (fp == NULL) {
		fopen_s(&fp, FICH, "w+b"); // criar ficheiro
		if (fp == NULL) {
			perror("ERRO FATAL: Impossivel criar ficheiro de dados!!\n");
			// OU fprintf(stderr, "ERRO FATAL: Impossivel criar ficheiro de dados\n");
			exit(1);
		}
	}
}

/*
* Faz um Menu Simples com as opções do vetor de Strings.
* Seleciona a Opção, usando o 1º caracter de cada string.
* Devolve o primeiro caracter da opção.
*/

char Menu(char* opcoes[]) {
	int i;
	char ch;

	while (1) {
		system("cls"); // limpar o ecrã

		for (i = 0; opcoes[i] != NULL; i++)
			printf("\t\t%s\n\n", opcoes[i]);

		printf("\n\tOpção: ");
		ch = getchar();
		fflush(stdin);

		for (i = 0; opcoes[i] != NULL; i++)
			if (ch == opcoes[i][0])
				return ch;
	}
}

void Inserir_Pessoa() {
	PESSOA x;
	Ler_Pessoa(&x);
	Adiciona_Pessoa(x);
}
void Alterar_Pessoa() {
	PESSOA x;
	long long n_reg;

	printf("Qual o nº do registo: ");
	scanf_s("%lld", &n_reg);
	fflush(stdin);

	if (fseek(fp, (long)((n_reg - 1) * sizeof(PESSOA)), SEEK_SET) != 0) {
		Message("Registo Inexistente ou Problemas de posicionamento!!");
		return;
	}
	if (fread(&x, sizeof(PESSOA), 1, fp) != 1) {
		Message("Problemas na Leitura do Registo!!");
		return;
	}
	if (!x.visivel) {
		Message("Um Registo apagado não pode ser alterado!!\n");
		return;
	}

	printf("\n\nDados Atuais:\n\n");
	Mostrar_Pessoa(x);
	printf("\nNovos Dados:\n\n");
	Ler_Pessoa(&x);

	// Recuar um registo no ficheiro
	fseek(fp, -(long)sizeof(PESSOA), SEEK_CUR);
	// Reescrever o registo
	fwrite(&x, sizeof(PESSOA), 1, fp);
	fflush(fp); // Despejar os dados no disco rigido
	Message("Registo alterado com sucesso!");
}

void Apagar_Pessoa() {
	PESSOA x;
	long long n_reg;
	char rsp, _;

	printf("Qual o nº do registo: ");
	scanf_s("%lld", &n_reg);
	_ = getchar();

	if (fseek(fp, (long)((n_reg - 1) * sizeof(PESSOA)), SEEK_SET) != 0) {
		Message("Registo Inexistente ou Problemas de posicionamento!!");
		return;
	}
	if (fread(&x, sizeof(PESSOA), 1, fp) != 1) {
		Message("Problemas na Leitura do Registo!!");
		return;
	}
	if (!x.visivel) {
		Message("Registo já está Apagado!!\n");
		return;
	}

	printf("\n\nDados Atuais:\n\n");
	Mostrar_Pessoa(x);

	printf("Quer mesmo apagar o registo? (s/n) ? ");
	rsp = getchar();
	if (toupper(rsp) != 'S')
		return;
	x.visivel = false;

	// Recuar um registo no ficheiro
	fseek(fp, -(long)sizeof(PESSOA), SEEK_CUR);
	// Reescrever o registo
	fwrite(&x, sizeof(PESSOA), 1, fp);
	fflush(fp); // Despejar os dados no disco rigido
	Message("Registo eliminado com sucesso!!");
}
void Listar() {
	PESSOA reg;
	long n_reg = 0, n_linhas = 0;
	rewind(fp);

	while (1) {
		if (fread(&reg, sizeof(reg), 1, fp) != 1)
			break;
		n_reg++;
		if (reg.visivel == false)
			continue;
		n_linhas++;
		printf("%d: ", n_reg);
		Mostrar_Pessoa(reg);

		if (n_linhas % 20 == 0)
			Message("Pressione <ENTER> para continuar ...");
	}
	Message("Pressione <ENTER> para continuar ...");
}
void Pesquisar_Idades(int inicio, int fim) {
	PESSOA reg;
	rewind(fp);

	while (fread(&reg, sizeof(PESSOA), 1, fp))
		if (reg.visivel && reg.idade >= inicio && reg.idade <= fim)
			Mostrar_Pessoa(reg);
	Message("\nPressione <ENTER> para continuar...");
}
//void Pesquisar_Nome(char* s) {
//	
//	PESSOA reg;
//	rewind(fp);
//
//	while (fread(&reg, sizeof(PESSOA), 1, fp))
//		if (reg.visivel && strstr(reg.nome, s ))
//			Mostrar_Pessoa(reg);
//
//	Message("\nPressione <ENTER> para continuar...");
//}
void Pesquisar_Nome(char* s) {

	PESSOA reg;
	rewind(fp);
	char minus[50];			// Converter o nome no registro para letras minúsculas

	for (int i = 0; s[i]; i++) {	// Converter a string "s" para letras minúsculas
		s[i] = tolower(s[i]);
	}
	while (fread(&reg, sizeof(PESSOA), 1, fp)) {
		strcpy_s(minus,sizeof(minus), reg.nome);
		for (int i = 0; minus[i]; i++) {
			minus[i] = tolower(minus[i]);
		}
		if (reg.visivel && strstr(minus, s)) {
			Mostrar_Pessoa(reg);
		}
	}

	Message("\nPressione <ENTER> para continuar...");
}
int main() {
	setlocale(LC_ALL, "Portuguese, Portugal");

	char opcao;

	Inic();

	while ((opcao = Menu(MainMenu)) != OP_SAIR)
		switch (opcao) {
		case OP_INSERIR:
			Inserir_Pessoa();
			break;

		case OP_ALTERAR:
			Alterar_Pessoa();
			break;

		case OP_APAGAR:
			Apagar_Pessoa();
			break;

		case OP_LISTAR:
			Listar();
			break;

		case OP_PESQUISAR:
			while ((opcao = Menu(PesqMenu)) != OP_SAIR)
				switch (opcao) {
				case OP_PESQ_IDADE: {
					int n1, n2;
					printf("\nQual o intervalo de idades? ");
					scanf_s("%d%d", &n1, &n2);
					fflush(stdin);
					Pesquisar_Idades(n1, n2);
					break;
				}
				case OP_PESQ_NOME:
				{
					char string[BUFSIZ + 1], _;
					printf("\nQual o nome a procurar: ");
					_ = getchar();
					gets_s(string, BUFSIZ + 1);
					fflush(stdin);
					Pesquisar_Nome(string);
					break;
				}
				}
		}
	return 0;
}
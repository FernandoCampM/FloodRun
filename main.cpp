// =====================================================================
//  FloodRun — GS 2026.2 CitizenTech (FIAP)
//  Disciplina: Data Structures and Algorithms
//
//  Mapa colaborativo de enchentes: cidadãos registram ocorrências,
//  o sistema estima o risco por região a partir do histórico de chuvas
//  e calcula a rota mais segura (a pé ou de carro).
//
//  ATENÇÃO: distâncias e histórico de chuvas são SIMULADOS (uso acadêmico).
//
//  Compilar: g++ -Wall -Wextra -std=c++17 -o floodrun main.cpp
//  Rodar:    .\floodrun.exe
// =====================================================================

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdlib>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

using namespace std;

// ---------------------------------------------------------------------
//  Constantes e dados fixos
// ---------------------------------------------------------------------

const string NOME_APP = "FloodRun";

const int N = 10;               // quantidade de regiões (vértices do mapa)
const int NUM_CATEGORIAS = 3;

// Índices das regiões: nomes legíveis em vez de "números mágicos"
const int FREGUESIA    = 0;
const int LIMAO        = 1;
const int CASA_VERDE   = 2;
const int BRASILANDIA  = 3;
const int PIRITUBA     = 4;
const int SANTANA      = 5;
const int VILA_MARIA   = 6;
const int LAPA         = 7;
const int BARRA_FUNDA  = 8;
const int CACHOEIRINHA = 9;

// Vetor de nomes: o índice da região é a posição neste vetor
const string regioes[N] = {
    "Freguesia do Ó", "Limão", "Casa Verde", "Brasilândia", "Pirituba",
    "Santana", "Vila Maria", "Lapa", "Barra Funda", "Cachoeirinha"
};

const int LEVE = 0;
const int INTRANSITAVEL = 1;
const int CORRENTEZA = 2;

const string categorias[NUM_CATEGORIAS] = {
    "Alagamento leve", "Via intransitável", "Enchente c/ correnteza"
};

// Matrizes de adjacência (km). 0 = não existe ligação direta.
double dist_carro[N][N];
double dist_pe[N][N];

// Um registro feito por um cidadão
struct Ocorrencia {
    int regiao;        // posição no vetor regioes[]
    int dia, mes, ano;
    double chuva_mm;   // chuva registrada no dia
    int categoria;     // LEVE, INTRANSITAVEL ou CORRENTEZA
    bool alagou;       // a região alagou de fato?
};

// ---------------------------------------------------------------------
//  Utilitários de texto (acentos ocupam mais de 1 byte em UTF-8)
// ---------------------------------------------------------------------

// Conta caracteres visíveis: em UTF-8, bytes 10xxxxxx são continuação
int larguraVisivel(const string& texto) {
    int largura = 0;
    for (size_t i = 0; i < texto.size(); i++) {
        if ((static_cast<unsigned char>(texto[i]) & 0xC0) != 0x80) {
            largura++;
        }
    }
    return largura;
}

// Completa com espaços para alinhar colunas mesmo com acentos
string coluna(const string& texto, int largura) {
    string resultado = texto;
    for (int i = larguraVisivel(texto); i < largura; i++) {
        resultado += ' ';
    }
    return resultado;
}

string aparar(const string& texto) {
    size_t inicio = 0;
    size_t fim = texto.size();
    while (inicio < fim && (texto[inicio] == ' ' || texto[inicio] == '\t' || texto[inicio] == '\r')) inicio++;
    while (fim > inicio && (texto[fim - 1] == ' ' || texto[fim - 1] == '\t' || texto[fim - 1] == '\r')) fim--;
    return texto.substr(inicio, fim - inicio);
}

string formatarData(int dia, int mes, int ano) {
    string d = (dia < 10 ? "0" : "") + to_string(dia);
    string m = (mes < 10 ? "0" : "") + to_string(mes);
    return d + "/" + m + "/" + to_string(ano);
}

string formatarDecimal(double valor, int casas) {
    // Arredonda somando meia casa e depois corta o texto (valores >= 0)
    double meiaCasa = 0.5;
    for (int i = 0; i < casas; i++) meiaCasa = meiaCasa / 10;
    string texto = to_string(valor + meiaCasa);   // ex.: "45.550000"
    size_t ponto = texto.find('.');
    if (ponto == string::npos) return texto;
    if (casas == 0) return texto.substr(0, ponto);
    return texto.substr(0, ponto + 1 + casas);
}

void linhaSeparadora(int tamanho) {
    for (int i = 0; i < tamanho; i++) cout << '-';
    cout << '\n';
}

// ---------------------------------------------------------------------
//  Leitura validada (nunca trava com letra onde se espera número)
// ---------------------------------------------------------------------

// Converte texto em inteiro, caractere por caractere
bool textoParaInteiro(const string& texto, int& valor) {
    if (texto.empty() || texto.size() > 9) return false;   // 9 dígitos: evita estouro
    int resultado = 0;
    for (size_t i = 0; i < texto.size(); i++) {
        if (texto[i] < '0' || texto[i] > '9') return false;
        resultado = resultado * 10 + (texto[i] - '0');
    }
    valor = resultado;
    return true;
}

// Aceita "45", "45.5" ou "45,5" (vírgula, como o brasileiro digita)
bool textoParaDecimal(const string& texto, double& valor) {
    if (texto.empty() || texto.size() > 12) return false;
    double inteira = 0, fracao = 0, escala = 1;
    bool temSeparador = false, temDigito = false;
    for (size_t i = 0; i < texto.size(); i++) {
        char c = texto[i];
        if (c >= '0' && c <= '9') {
            temDigito = true;
            if (!temSeparador) {
                inteira = inteira * 10 + (c - '0');
            } else {
                escala = escala / 10;
                fracao = fracao + (c - '0') * escala;
            }
        } else if ((c == '.' || c == ',') && !temSeparador) {
            temSeparador = true;
        } else {
            return false;
        }
    }
    if (!temDigito) return false;
    valor = inteira + fracao;
    return true;
}

// Lê uma linha; se a entrada acabar (Ctrl+Z ou arquivo de teste), encerra limpo
string lerLinha(const string& mensagem) {
    string linha;
    cout << mensagem;
    if (!getline(cin, linha)) {
        cout << "\n\nEntrada encerrada. Até logo!\n";
        exit(0);
    }
    return aparar(linha);
}

int lerInteiro(const string& mensagem, int minimo, int maximo) {
    while (true) {
        string linha = lerLinha(mensagem);
        int valor;
        if (!textoParaInteiro(linha, valor)) {
            cout << "  ! Valor inválido: digite um número inteiro.\n";
        } else if (valor < minimo || valor > maximo) {
            cout << "  ! Fora do intervalo: digite de " << minimo << " a " << maximo << ".\n";
        } else {
            return valor;
        }
    }
}

double lerDecimal(const string& mensagem, double minimo, double maximo) {
    while (true) {
        string linha = lerLinha(mensagem);
        double valor;
        if (!textoParaDecimal(linha, valor)) {
            cout << "  ! Valor inválido: digite um número (ex.: 45 ou 45,5).\n";
        } else if (valor < minimo || valor > maximo) {
            cout << "  ! Fora do intervalo: digite de " << minimo << " a " << maximo << ".\n";
        } else {
            return valor;
        }
    }
}

int escolherRegiao(const string& mensagem) {
    cout << "\n";
    for (int i = 0; i < N; i++) {
        cout << "  " << setw(2) << (i + 1) << ") " << regioes[i] << "\n";
    }
    return lerInteiro(mensagem, 1, N) - 1;   // usuário vê 1..N, vetor usa 0..N-1
}

int escolherCategoria(const string& mensagem) {
    cout << "\n";
    for (int i = 0; i < NUM_CATEGORIAS; i++) {
        cout << "  " << (i + 1) << ") " << categorias[i] << "\n";
    }
    return lerInteiro(mensagem, 1, NUM_CATEGORIAS) - 1;
}

bool anoBissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

int diasNoMes(int mes, int ano) {
    const int dias[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (mes == 2 && anoBissexto(ano)) return 29;
    return dias[mes - 1];
}

// ---------------------------------------------------------------------
//  Carga inicial (dados simulados em memória)
// ---------------------------------------------------------------------

// Liga duas regiões nos dois sentidos (rua de mão dupla)
void ligar(int a, int b, double km, bool somenteAPe) {
    dist_pe[a][b] = km;
    dist_pe[b][a] = km;
    if (!somenteAPe) {
        dist_carro[a][b] = km;
        dist_carro[b][a] = km;
    }
}

void carregarMapa() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            dist_carro[i][j] = 0;
            dist_pe[i][j] = 0;
        }
    }
    // Ruas e avenidas (carro e pé)
    ligar(PIRITUBA,     BRASILANDIA,  4.0, false);
    ligar(PIRITUBA,     FREGUESIA,    3.5, false);
    ligar(PIRITUBA,     LAPA,         5.0, false);
    ligar(BRASILANDIA,  FREGUESIA,    3.0, false);
    ligar(BRASILANDIA,  CACHOEIRINHA, 3.5, false);
    ligar(FREGUESIA,    LIMAO,        2.5, false);
    ligar(FREGUESIA,    LAPA,         4.0, false);
    ligar(FREGUESIA,    CACHOEIRINHA, 3.0, false);
    ligar(CACHOEIRINHA, CASA_VERDE,   3.5, false);
    ligar(CACHOEIRINHA, LIMAO,        2.5, false);
    ligar(LIMAO,        CASA_VERDE,   2.0, false);
    ligar(LIMAO,        BARRA_FUNDA,  3.5, false);
    ligar(LAPA,         BARRA_FUNDA,  3.0, false);
    ligar(CASA_VERDE,   SANTANA,      3.0, false);
    ligar(CASA_VERDE,   BARRA_FUNDA,  4.0, false);
    ligar(SANTANA,      VILA_MARIA,   4.0, false);
    ligar(BARRA_FUNDA,  SANTANA,      5.0, false);
    // Ligações SÓ A PÉ (carro não passa)
    ligar(CACHOEIRINHA, SANTANA,      4.0, true);   // escadaria / trilha do Horto
    ligar(CASA_VERDE,   VILA_MARIA,   4.5, true);   // passarela de pedestres
}

void adicionar(vector<Ocorrencia>& lista, int regiao, int dia, int mes, int ano,
               double chuva, int categoria, bool alagou) {
    Ocorrencia o;
    o.regiao = regiao;
    o.dia = dia;
    o.mes = mes;
    o.ano = ano;
    o.chuva_mm = chuva;
    o.categoria = categoria;
    o.alagou = alagou;
    lista.push_back(o);
}

void carregarOcorrencias(vector<Ocorrencia>& lista) {
    // Limão: baixada perto do Tietê, alaga com pouca chuva
    adicionar(lista, LIMAO, 12, 1, 2024, 45, LEVE, true);
    adicionar(lista, LIMAO, 3, 2, 2024, 30, LEVE, false);
    adicionar(lista, LIMAO, 20, 11, 2024, 52, INTRANSITAVEL, true);
    adicionar(lista, LIMAO, 8, 1, 2025, 38, LEVE, false);
    adicionar(lista, LIMAO, 25, 2, 2025, 75, CORRENTEZA, true);
    adicionar(lista, LIMAO, 14, 1, 2026, 60, INTRANSITAVEL, true);
    // Casa Verde
    adicionar(lista, CASA_VERDE, 12, 1, 2024, 50, INTRANSITAVEL, true);
    adicionar(lista, CASA_VERDE, 9, 3, 2024, 35, LEVE, false);
    adicionar(lista, CASA_VERDE, 25, 2, 2025, 80, CORRENTEZA, true);
    adicionar(lista, CASA_VERDE, 14, 1, 2026, 58, INTRANSITAVEL, true);
    // Barra Funda
    adicionar(lista, BARRA_FUNDA, 3, 2, 2024, 40, LEVE, false);
    adicionar(lista, BARRA_FUNDA, 25, 2, 2025, 70, INTRANSITAVEL, true);
    adicionar(lista, BARRA_FUNDA, 10, 12, 2025, 55, LEVE, false);
    adicionar(lista, BARRA_FUNDA, 2, 2, 2026, 90, CORRENTEZA, true);
    // Lapa
    adicionar(lista, LAPA, 9, 3, 2024, 45, LEVE, false);
    adicionar(lista, LAPA, 18, 12, 2024, 50, LEVE, false);
    adicionar(lista, LAPA, 2, 2, 2026, 85, INTRANSITAVEL, true);
    // Santana
    adicionar(lista, SANTANA, 20, 11, 2024, 70, LEVE, false);
    adicionar(lista, SANTANA, 2, 2, 2026, 90, INTRANSITAVEL, true);
    // Cachoeirinha
    adicionar(lista, CACHOEIRINHA, 8, 1, 2025, 40, LEVE, false);
    adicionar(lista, CACHOEIRINHA, 25, 2, 2025, 65, LEVE, false);
    adicionar(lista, CACHOEIRINHA, 2, 2, 2026, 95, INTRANSITAVEL, true);
    // Freguesia do Ó
    adicionar(lista, FREGUESIA, 3, 2, 2024, 48, LEVE, false);
    adicionar(lista, FREGUESIA, 18, 12, 2024, 50, LEVE, false);
    adicionar(lista, FREGUESIA, 25, 2, 2025, 72, INTRANSITAVEL, true);
    // Brasilândia: córregos em morro, enchente com correnteza
    adicionar(lista, BRASILANDIA, 12, 1, 2024, 50, LEVE, false);
    adicionar(lista, BRASILANDIA, 20, 11, 2024, 65, CORRENTEZA, true);
    adicionar(lista, BRASILANDIA, 14, 1, 2026, 78, CORRENTEZA, true);
    // Pirituba
    adicionar(lista, PIRITUBA, 9, 3, 2024, 44, LEVE, false);
    adicionar(lista, PIRITUBA, 10, 12, 2025, 60, LEVE, false);
    adicionar(lista, PIRITUBA, 2, 2, 2026, 88, INTRANSITAVEL, true);
    // Vila Maria
    adicionar(lista, VILA_MARIA, 3, 2, 2024, 40, LEVE, false);
    adicionar(lista, VILA_MARIA, 18, 12, 2024, 62, INTRANSITAVEL, true);
    adicionar(lista, VILA_MARIA, 10, 12, 2025, 70, INTRANSITAVEL, true);
    adicionar(lista, VILA_MARIA, 2, 2, 2026, 95, CORRENTEZA, true);
}

// ---------------------------------------------------------------------
//  Impressão de ocorrências
// ---------------------------------------------------------------------

void cabecalhoTabela() {
    cout << coluna("#", 4) << coluna("Região", 16) << coluna("Data", 12)
         << coluna("Chuva", 10) << coluna("Categoria", 24) << "Alagou\n";
    linhaSeparadora(72);
}

void imprimirOcorrencia(const Ocorrencia& o, int numero) {
    cout << coluna(to_string(numero), 4)
         << coluna(regioes[o.regiao], 16)
         << coluna(formatarData(o.dia, o.mes, o.ano), 12)
         << coluna(formatarDecimal(o.chuva_mm, 1) + " mm", 10)
         << coluna(categorias[o.categoria], 24)
         << (o.alagou ? "sim" : "não") << "\n";
}

// ---------------------------------------------------------------------
//  1) Cadastrar ocorrência
// ---------------------------------------------------------------------

void cadastrarOcorrencia(vector<Ocorrencia>& lista) {
    cout << "\n=== CADASTRAR OCORRÊNCIA ===\n";
    Ocorrencia o;
    o.regiao = escolherRegiao("Região (1-" + to_string(N) + "): ");
    o.ano = lerInteiro("Ano (2000-2026): ", 2000, 2026);
    o.mes = lerInteiro("Mês (1-12): ", 1, 12);
    int maxDia = diasNoMes(o.mes, o.ano);   // valida 31/02, 31/04 etc.
    o.dia = lerInteiro("Dia (1-" + to_string(maxDia) + "): ", 1, maxDia);
    o.chuva_mm = lerDecimal("Chuva no dia em mm (0-500): ", 0, 500);
    o.categoria = escolherCategoria("Categoria (1-3): ");

    // Via intransitável e correnteza já são alagamento; só o "leve" precisa perguntar
    if (o.categoria == LEVE) {
        int resp = lerInteiro("A região chegou a alagar? (1 = sim, 2 = não): ", 1, 2);
        o.alagou = (resp == 1);
    } else {
        o.alagou = true;
    }

    lista.push_back(o);
    cout << "\nOcorrência registrada com sucesso! Total agora: " << lista.size() << "\n\n";
    cabecalhoTabela();
    imprimirOcorrencia(o, static_cast<int>(lista.size()));
}

// ---------------------------------------------------------------------
//  2) Listar todas
// ---------------------------------------------------------------------

void listarTodas(const vector<Ocorrencia>& lista) {
    cout << "\n=== TODAS AS OCORRÊNCIAS (" << lista.size() << ") ===\n\n";
    if (lista.empty()) {
        cout << "Nenhuma ocorrência cadastrada.\n";
        return;
    }
    cabecalhoTabela();
    for (size_t i = 0; i < lista.size(); i++) {
        imprimirOcorrencia(lista[i], static_cast<int>(i + 1));
    }
}

// ---------------------------------------------------------------------
//  3) Buscar (busca sequencial: olha um por um, do início ao fim)
// ---------------------------------------------------------------------

void buscarPorRegiao(const vector<Ocorrencia>& lista, int regiao) {
    cout << "\nOcorrências em " << regioes[regiao] << ":\n\n";
    int encontradas = 0;                         // contador
    for (size_t i = 0; i < lista.size(); i++) {
        if (lista[i].regiao == regiao) {
            if (encontradas == 0) cabecalhoTabela();
            imprimirOcorrencia(lista[i], static_cast<int>(i + 1));
            encontradas++;
        }
    }
    if (encontradas == 0) {
        cout << "Nenhuma ocorrência encontrada nessa região.\n";
    } else {
        cout << "\n" << encontradas << " ocorrência(s) encontrada(s), de "
             << lista.size() << " comparadas.\n";
    }
}

void buscarPorCategoria(const vector<Ocorrencia>& lista, int categoria) {
    cout << "\nOcorrências da categoria \"" << categorias[categoria] << "\":\n\n";
    int encontradas = 0;
    for (size_t i = 0; i < lista.size(); i++) {
        if (lista[i].categoria == categoria) {
            if (encontradas == 0) cabecalhoTabela();
            imprimirOcorrencia(lista[i], static_cast<int>(i + 1));
            encontradas++;
        }
    }
    if (encontradas == 0) {
        cout << "Nenhuma ocorrência encontrada nessa categoria.\n";
    } else {
        cout << "\n" << encontradas << " ocorrência(s) encontrada(s), de "
             << lista.size() << " comparadas.\n";
    }
}

void menuBusca(const vector<Ocorrencia>& lista) {
    cout << "\n=== BUSCAR ===\n";
    cout << "  1) Por região\n";
    cout << "  2) Por categoria\n";
    int tipo = lerInteiro("Opção: ", 1, 2);
    if (tipo == 1) {
        buscarPorRegiao(lista, escolherRegiao("Região (1-" + to_string(N) + "): "));
    } else {
        buscarPorCategoria(lista, escolherCategoria("Categoria (1-3): "));
    }
}

// ---------------------------------------------------------------------
//  4) Estatísticas (contadores e acumuladores)
// ---------------------------------------------------------------------

void estatisticas(const vector<Ocorrencia>& lista) {
    cout << "\n=== ESTATÍSTICAS ===\n";
    if (lista.empty()) {
        cout << "Nenhuma ocorrência cadastrada.\n";
        return;
    }

    int totalRegiao[N];               // contador por região
    int enchentesRegiao[N];           // contador de alagamentos por região
    int totalCategoria[NUM_CATEGORIAS];
    for (int i = 0; i < N; i++) {
        totalRegiao[i] = 0;
        enchentesRegiao[i] = 0;
    }
    for (int c = 0; c < NUM_CATEGORIAS; c++) totalCategoria[c] = 0;

    double somaChuvaEnchente = 0;     // acumulador
    int qtdEnchentes = 0;             // contador

    // Uma única passada pelo vetor alimenta todos os contadores
    for (size_t i = 0; i < lista.size(); i++) {
        totalRegiao[lista[i].regiao]++;
        totalCategoria[lista[i].categoria]++;
        if (lista[i].alagou) {
            enchentesRegiao[lista[i].regiao]++;
            somaChuvaEnchente += lista[i].chuva_mm;
            qtdEnchentes++;
        }
    }

    cout << "\nTotal de ocorrências: " << lista.size()
         << "  (alagou: " << qtdEnchentes << " | não alagou: "
         << (static_cast<int>(lista.size()) - qtdEnchentes) << ")\n";

    cout << "\nPor região:\n";
    cout << "  " << coluna("Região", 16) << coluna("Registros", 11) << "Alagamentos\n";
    for (int i = 0; i < N; i++) {
        cout << "  " << coluna(regioes[i], 16) << coluna(to_string(totalRegiao[i]), 11)
             << enchentesRegiao[i] << "\n";
    }

    cout << "\nPor categoria:\n";
    for (int c = 0; c < NUM_CATEGORIAS; c++) {
        cout << "  " << coluna(categorias[c], 24) << totalCategoria[c] << "\n";
    }

    cout << "\nChuva média que causou enchente: ";
    if (qtdEnchentes > 0) {
        cout << formatarDecimal(somaChuvaEnchente / qtdEnchentes, 1) << " mm"
             << "  (soma " << formatarDecimal(somaChuvaEnchente, 1) << " mm / "
             << qtdEnchentes << " enchentes)\n";
    } else {
        cout << "sem enchentes registradas\n";
    }

    // Região mais crítica: busca do maior valor no vetor de contadores
    int maisCritica = 0;
    for (int i = 1; i < N; i++) {
        if (enchentesRegiao[i] > enchentesRegiao[maisCritica]) {
            maisCritica = i;
        }
    }
    if (enchentesRegiao[maisCritica] > 0) {
        cout << "Região mais crítica: " << regioes[maisCritica] << " ("
             << enchentesRegiao[maisCritica] << " alagamentos)\n";
    } else {
        cout << "Região mais crítica: nenhuma (sem alagamentos)\n";
    }
}

// ---------------------------------------------------------------------
//  5) Ordenação (três algoritmos diferentes, escritos à mão)
// ---------------------------------------------------------------------

// Ranking de regiões por nº de enchentes — SELECTION SORT
void rankingRegioes(const vector<Ocorrencia>& lista) {
    int indice[N];       // indice[k] = qual região está na posição k do ranking
    int enchentes[N];
    for (int i = 0; i < N; i++) {
        indice[i] = i;
        enchentes[i] = 0;
    }
    for (size_t i = 0; i < lista.size(); i++) {
        if (lista[i].alagou) enchentes[lista[i].regiao]++;
    }

    // A cada passo, escolhe a região com MAIS enchentes entre as que faltam
    for (int i = 0; i < N - 1; i++) {
        int maior = i;
        for (int j = i + 1; j < N; j++) {
            if (enchentes[indice[j]] > enchentes[indice[maior]]) {
                maior = j;
            }
        }
        if (maior != i) {                 // troca de posição
            int temp = indice[i];
            indice[i] = indice[maior];
            indice[maior] = temp;
        }
    }

    cout << "\nRanking de regiões por nº de alagamentos (selection sort):\n\n";
    for (int k = 0; k < N; k++) {
        cout << "  " << setw(2) << (k + 1) << "º  " << coluna(regioes[indice[k]], 16)
             << setw(2) << enchentes[indice[k]] << "  ";
        for (int b = 0; b < enchentes[indice[k]]; b++) cout << "█";   // barrinha visual
        cout << "\n";
    }
}

// Ocorrências da maior para a menor chuva — BUBBLE SORT
void ordenarPorChuva(vector<Ocorrencia> copia) {   // cópia: não bagunça a lista original
    int n = static_cast<int>(copia.size());
    for (int passada = 0; passada < n - 1; passada++) {
        bool trocou = false;
        // Compara vizinhos; a menor chuva vai "afundando" para o fim
        for (int j = 0; j < n - 1 - passada; j++) {
            if (copia[j].chuva_mm < copia[j + 1].chuva_mm) {
                Ocorrencia temp = copia[j];
                copia[j] = copia[j + 1];
                copia[j + 1] = temp;
                trocou = true;
            }
        }
        if (!trocou) break;               // já está ordenado: para antes
    }
    cout << "\nOcorrências por chuva, da maior para a menor (bubble sort):\n\n";
    cabecalhoTabela();
    for (int i = 0; i < n; i++) imprimirOcorrencia(copia[i], i + 1);
}

// Transforma a data em um número comparável: 14/01/2026 -> 20260114
int dataComoNumero(const Ocorrencia& o) {
    return o.ano * 10000 + o.mes * 100 + o.dia;
}

// Ocorrências da mais antiga para a mais recente — INSERTION SORT
void ordenarPorData(vector<Ocorrencia> copia) {
    int n = static_cast<int>(copia.size());
    for (int i = 1; i < n; i++) {
        Ocorrencia atual = copia[i];
        int j = i - 1;
        // Empurra para a direita as mais recentes, abrindo espaço para "atual"
        while (j >= 0 && dataComoNumero(copia[j]) > dataComoNumero(atual)) {
            copia[j + 1] = copia[j];
            j--;
        }
        copia[j + 1] = atual;
    }
    cout << "\nOcorrências por data, da mais antiga para a mais recente (insertion sort):\n\n";
    cabecalhoTabela();
    for (int i = 0; i < n; i++) imprimirOcorrencia(copia[i], i + 1);
}

void menuOrdenacao(const vector<Ocorrencia>& lista) {
    cout << "\n=== LISTAR ORDENADO ===\n";
    cout << "  1) Ranking de regiões por nº de alagamentos\n";
    cout << "  2) Ocorrências por chuva (maior primeiro)\n";
    cout << "  3) Ocorrências por data (mais antiga primeiro)\n";
    int tipo = lerInteiro("Opção: ", 1, 3);
    if (lista.empty()) {
        cout << "Nenhuma ocorrência cadastrada.\n";
        return;
    }
    if (tipo == 1) rankingRegioes(lista);
    else if (tipo == 2) ordenarPorChuva(lista);
    else ordenarPorData(lista);
}

// ---------------------------------------------------------------------
//  Menu principal
// ---------------------------------------------------------------------

void mostrarMenu() {
    cout << "\n";
    linhaSeparadora(44);
    cout << "  " << NOME_APP << " — menu principal\n";
    linhaSeparadora(44);
    cout << "  1) Cadastrar ocorrência\n";
    cout << "  2) Listar todas\n";
    cout << "  3) Buscar (por categoria ou região)\n";
    cout << "  4) Estatísticas\n";
    cout << "  5) Listar ordenado\n";
    cout << "  0) Sair\n";
    linhaSeparadora(44);
}

int main() {
#ifdef _WIN32
    // Faz o console do Windows mostrar acentos corretamente
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    vector<Ocorrencia> ocorrencias;
    carregarMapa();
    carregarOcorrencias(ocorrencias);

    cout << "==============================================\n";
    cout << "  " << NOME_APP << " — mapa colaborativo de enchentes\n";
    cout << "  Zona Norte de São Paulo\n";
    cout << "  Aviso: distâncias e histórico são SIMULADOS.\n";
    cout << "==============================================\n";
    cout << ocorrencias.size() << " ocorrências pré-carregadas.\n";

    int opcao;
    do {
        mostrarMenu();
        opcao = lerInteiro("Escolha: ", 0, 5);
        switch (opcao) {
            case 1: cadastrarOcorrencia(ocorrencias); break;
            case 2: listarTodas(ocorrencias); break;
            case 3: menuBusca(ocorrencias); break;
            case 4: estatisticas(ocorrencias); break;
            case 5: menuOrdenacao(ocorrencias); break;
            case 0: cout << "\nObrigado por usar o " << NOME_APP << ". Fique seguro!\n"; break;
        }
    } while (opcao != 0);

    return 0;
}

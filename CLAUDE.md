# FloodRun — GS 2026.2 CitizenTech (FIAP) · Data Structures and Algorithms

> Nome do projeto: **FloodRun**. Se mudar, troque aqui e na constante `NOME_APP` do código.

## Contexto

Sou aluno de Ciência da Computação na FIAP. Este é o projeto da Global Solution 2026.2
(tema CitizenTech) da disciplina **Data Structures and Algorithms**. **Entrega: 19/11 às 23:59.**

Eu preciso **entender e apresentar** o trabalho (vídeo e apresentação em grupo). Por isso,
explicar o que foi feito importa tanto quanto o código funcionar.

### O enunciado (resumo do PDF `GS2026.23.pdf`, se estiver na pasta, leia)
Tema: "Mapa Colaborativo de Problemas Urbanos". Sistema onde cidadãos registram e consultam
ocorrências urbanas. Requisitos mínimos:
- Cadastrar ocorrências, com **categoria** e **bairro/região**
- Listar todas as ocorrências
- Buscar por categoria ou por região
- Gerar estatísticas simples
- Apresentar os registros ordenados por um critério definido pela equipe

Conteúdos que **precisam aparecer claramente**: algoritmos, variáveis e tipos, estruturas de
decisão, estruturas de repetição, funções, **vetores**, **busca sequencial**, **algoritmos de
ordenação**, **contadores e acumuladores**.

Entregáveis:
1. Sistema funcional (console basta; interface avançada não é exigida)
2. Relatório técnico de 2 a 4 páginas: problema, justificativa, estrutura dos dados,
   algoritmos, funcionalidades, desafios, benefícios para a comunidade
3. Diagrama de Atividades UML do fluxo principal (registro → consulta → análise)
4. Vídeo de até 3 minutos: problema, proposta, funcionalidades, demonstração

Avaliação: 40% funcionamento correto · 30% aplicação dos conteúdos (vetores, busca, ordenação,
funções, controle) · 15% organização e boas práticas · 10% documentação · 5% vídeo.

Regra geral da GS (página da FIAP): o projeto deve **identificar os ODS** a que se conecta e
explicar como contribui. Para este projeto: **ODS 11** (principal), **ODS 9** e **ODS 13**
(ação contra a mudança do clima). O relatório deve ter essa seção.

## A nossa solução

Problema escolhido: **enchentes**. O app:
1. Recebe **ocorrências de enchente reportadas por cidadãos** (região, data, chuva em mm,
   categoria). Isso cumpre o "mapa colaborativo" do enunciado.
2. Usa o **histórico de chuvas e enchentes** para **estimar o risco** de cada região dada
   uma previsão de chuva.
3. Calcula a **rota mais segura** entre duas regiões, **a pé ou de carro**, desviando das
   regiões com risco, como um "Google Maps contra enchentes".

```
 [Cidadão reporta enchente] ──► vetor de ocorrências (histórico)
                                   │
 [Previsão de chuva em mm] ────► risco por região (BAIXO / MÉDIO / ALTO)
                                   │
 [Origem, destino, pé/carro] ──► Dijkstra com pesos ajustados pelo risco
                                   │
                                   ▼
                     Rota mais segura + regiões evitadas
```

## Especificação técnica

### Linguagem e ambiente
- **C++17**, programa de console, um arquivo só: `main.cpp` (divida só se eu pedir).
- Ambiente: **Windows + PowerShell**, compilador `g++` (MSYS2/MinGW ou w64devkit).
- Compilar sem warnings: `g++ -Wall -Wextra -std=c++17 -o floodrun main.cpp`
- Rodar: `.\floodrun.exe`
- Acentos no console do Windows: configure UTF-8 no início do `main`
  (ex.: `SetConsoleOutputCP(CP_UTF8)` com `#ifdef _WIN32`), mantendo o código portável.
- Se o `g++` não estiver disponível, avise. Não finja que compilou.

### O que pode e o que não pode
- Pode usar o que for necessário do C++: `struct`, `std::string`, `std::vector`,
  `cin`/`cout`/`getline`. Código simples, sem classes/POO, sem templates, sem truques.
- **Escrito à mão, sem atalho da STL** (é o que o professor avalia):
  - busca sequencial (for + if), sem `std::find`/`find_if`
  - ordenação (bubble, selection ou insertion sort), sem `std::sort`
  - estatísticas com contadores e acumuladores, sem `std::count`/`accumulate`
  - no Dijkstra, o "menor não visitado" por busca sequencial, sem `priority_queue`
- Nada de `<algorithm>`. Sem arquivos externos nem banco: os dados ficam em memória,
  pré-carregados no início.
- Validar entradas do usuário (número fora do intervalo, texto onde se espera número).

### Dados
- **~10 regiões** com nomes de bairros da Zona Norte de SP (Freguesia do Ó, Limão,
  Casa Verde, Brasilândia, Pirituba, Santana, Vila Maria, Lapa, Barra Funda, Cachoeirinha).
- **Distâncias e histórico são simulados.** O programa e o relatório devem dizer isso.
- Mapa como **matriz de adjacência**: `dist_carro[N][N]` e `dist_pe[N][N]` (0 = sem ligação).
  Algumas ligações existem **só a pé** (escadaria, passarela).
- **~30 ocorrências pré-carregadas**: região, dia, mês, chuva (mm), categoria, alagou (sim/não).
- Categorias: alagamento leve · via intransitável · enchente com correnteza.

### Menu
1. **Cadastrar ocorrência**
2. **Listar todas**
3. **Buscar** por categoria ou por região (busca sequencial)
4. **Estatísticas**: total por região, total por categoria, chuva média que causou
   enchente, região mais crítica (contadores e acumuladores)
5. **Listar ordenado**: ranking de regiões por nº de enchentes; ocorrências por chuva ou data
6. **Estimar risco** para uma previsão de chuva (mm):
   - limiar = menor chuva do histórico que causou enchente na região (busca sequencial)
   - seguro = maior chuva que **não** causou enchente
   - previsão ≥ limiar → ALTO · previsão > seguro → MÉDIO · senão → BAIXO
   - região sem histórico de enchente → BAIXO
7. **Rota mais segura** (origem, destino, modo pé/carro, previsão de chuva):
   - Dijkstra com vetores `dist[]`, `visitado[]`, `anterior[]`
   - peso da aresta = distância × fator de risco da região de chegada
     - carro: BAIXO ×1 · MÉDIO ×3 · ALTO bloqueado
     - a pé: BAIXO ×1 · MÉDIO ×2 · ALTO bloqueado (e pode usar ligações só a pé)
   - mostrar caminho, distância real total e regiões evitadas
   - se não existir rota segura, avisar claramente
0. **Sair**

### Prioridade (prazo curto)
1. Primeiro as opções **1 a 5** (requisitos mínimos, garantem a nota base). Teste e faça commit.
2. Depois as opções **6 e 7** (o diferencial).
3. Se algo travar no Dijkstra, o projeto precisa continuar entregável sem ele.

## Como trabalhar comigo

- **Antes de codar, mostre o plano** (estruturas de dados, lista de funções, ordem) e espere
  eu aprovar.
- Implemente **uma funcionalidade por vez**. Depois de cada uma: compile, teste com uma
  entrada de exemplo e me explique em português simples:
  - o que a função faz
  - qual conteúdo da disciplina ela demonstra
  - como eu explicaria isso em 2 ou 3 frases na apresentação
- Use **desenhos ASCII** para explicar algoritmos (vetores antes e depois da ordenação,
  passos do Dijkstra etc.). Eu aprendo melhor assim.
- Comentários no código em português, curtos, explicando o **porquê**.

### Arquivos que você mantém atualizados
- `main.cpp`: o programa.
- `EXPLICACAO.md`: uma seção por função, com o que faz, passo a passo com ASCII e um
  resumo para a apresentação. É a base do relatório e do vídeo.
- `PROGRESSO.md`: o que está pronto e testado, o que falta, decisões tomadas.
  Atualize ao fim de cada funcionalidade.
- `ROTEIRO_TESTE.md`: entradas para demonstrar no vídeo, incluindo a mesma origem e
  destino dando **rotas diferentes a pé e de carro** com chuva forte.

### Ao iniciar uma sessão nova
Uso dois PCs com o projeto no GitHub. **Leia o `PROGRESSO.md` antes de qualquer coisa** e
continue de onde parou. Se não existir, comece pelo plano.

### Git
- Commits pequenos, mensagem em português (ex.: `feat: busca sequencial por região`),
  um por funcionalidade testada.
- Nunca commite o executável (`*.exe`). Crie um `.gitignore` com isso.

### Depois do código (se eu pedir)
- `relatorio.md` com as seções do enunciado + ODS, no máximo 4 páginas.
- Diagrama de atividades em PlantUML ou Mermaid (`diagrama.puml` ou `diagrama.md`).
- Roteiro do vídeo de 3 minutos com o que mostrar em cada trecho.

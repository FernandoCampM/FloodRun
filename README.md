# FloodRun

Mapa colaborativo de enchentes com **rota mais segura a pé ou de carro**, em C++ (console).
Global Solution 2026.2 · CitizenTech · FIAP · Data Structures and Algorithms.

Cidadãos registram ocorrências de enchente; o sistema usa esse histórico para estimar o risco de
cada região a partir da chuva prevista e calcula, com Dijkstra, o caminho que desvia das regiões
que devem alagar.

> Distâncias e histórico de chuvas são **simulados** (uso acadêmico).

## Como rodar
```
g++ -Wall -Wextra -std=c++17 -o floodrun main.cpp
.\floodrun.exe
```
No Windows, o compilador pode ser instalado com `winget install BrechtSanders.WinLibs.POSIX.UCRT`.

## Funcionalidades
1. Cadastrar ocorrência
2. Listar todas
3. Buscar por região ou categoria (busca sequencial)
4. Estatísticas (contadores e acumuladores)
5. Listar ordenado (selection, bubble e insertion sort)
6. Estimar risco para uma previsão de chuva
7. Rota mais segura (Dijkstra com peso ajustado pelo risco)

## Documentos
- [relatorio.md](relatorio.md): relatório técnico, com os ODS 11, 9 e 13
- [EXPLICACAO.md](EXPLICACAO.md): explicação de cada função, com desenhos ASCII
- [diagrama.md](diagrama.md) / [diagrama.puml](diagrama.puml): diagrama de atividades
- [ROTEIRO_TESTE.md](ROTEIRO_TESTE.md): entradas de teste e demonstração
- [PROGRESSO.md](PROGRESSO.md): andamento e decisões

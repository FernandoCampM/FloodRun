# PROGRESSO: FloodRun

Última atualização: 16/11/2026 · Entrega: 19/11 às 23:59

## Pronto e testado ✔
| Opção | Funcionalidade | Status |
|---|---|---|
| — | Base: dados simulados (10 regiões, 35 ocorrências, 2 matrizes), leitura validada, menu, UTF-8 | ✔ |
| 1 | Cadastrar ocorrência (valida data com bissexto, chuva com vírgula) | ✔ |
| 2 | Listar todas | ✔ |
| 3 | Buscar por região / categoria (busca sequencial) | ✔ |
| 4 | Estatísticas (contadores e acumuladores) | ✔ |
| 5 | Ordenação: ranking (selection), chuva (bubble), data (insertion) | ✔ |
| 6 | Estimar risco (limiar / seguro) | ✔ |
| 7 | Rota mais segura (Dijkstra, pé/carro, regiões evitadas, passo a passo) | ✔ |

Compila sem warnings com `g++ -Wall -Wextra -std=c++17 -o floodrun main.cpp`
(GCC 16.2, WinLibs, instalado via winget neste PC).

Documentos: `EXPLICACAO.md` ✔ · `ROTEIRO_TESTE.md` ✔ · `relatorio.md` ✔ (com ODS 11, 9 e 13) ·
`diagrama.puml` + `diagrama.md` (Mermaid) ✔ · `ROTEIRO_VIDEO.md` ✔

## Falta
- [x] Nomes e RMs no `relatorio.md` (Fernando Mota RM567451 · André da Silva Pinto RM559038)
- [x] Imagem do diagrama (`diagrama.png`) gerada e inserida no relatório
- [ ] Converter o relatório para PDF/Word e conferir se ficou com no máximo 4 páginas
- [ ] Gravar o vídeo seguindo o `ROTEIRO_VIDEO.md`
- [ ] Subir para o GitHub (repositório remoto ainda não configurado)

## Decisões tomadas
- **Ano na data**: a ocorrência guarda dia, mês **e ano**, senão a ordenação por data colocaria
  dezembro de 2024 depois de janeiro de 2026 de forma errada.
- **Categoria × alagou**: "via intransitável" e "correnteza" já implicam `alagou = sim`; só o
  "alagamento leve" pergunta.
- **Três ordenações diferentes** (selection, bubble, insertion) para mostrar mais conteúdo.
- **Ordenação em cópia** do vetor, para preservar a ordem de cadastro.
- **Região guardada como índice** (int) no vetor `regioes[]`; o usuário escolhe por número, o
  que evita erro de digitação e problema com acentos na entrada.
- **Risco MÉDIO quando não há registro "sem alagamento"**: `seguro = -1`, então qualquer previsão
  abaixo do limiar vira MÉDIO (postura conservadora).
- **Origem em ALTO**: permitida (a pessoa precisa sair de lá), com aviso. **Destino em ALTO**: sem rota.
- **Comparação com a rota normal**: rodamos o Dijkstra duas vezes (com e sem risco) para mostrar
  as regiões evitadas e o desvio em km.
- Dados de distância e histórico são **simulados** e montados para a demonstração funcionar:
  60 mm dá rotas diferentes a pé e de carro, e 75 mm deixa o carro sem rota.

## Para o outro PC
Instalar o compilador: `winget install BrechtSanders.WinLibs.POSIX.UCRT` (abra um terminal novo depois).

# FloodRun: explicação do código

Este documento serve de base para o relatório e para o vídeo. Cada seção traz o que a parte do
código faz, o passo a passo com desenho e um **resumo para a apresentação** (2 ou 3 frases).

> Distâncias e histórico de chuvas são **simulados** para fins acadêmicos.

---

## 0. Visão geral

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

| Conteúdo da disciplina | Onde aparece |
|---|---|
| Variáveis e tipos | `struct Ocorrencia` (int, double, bool, string) |
| Estruturas de decisão | `if/else` em todo lugar, `switch` no menu, `riscoRegiao` |
| Estruturas de repetição | `for`, `while`, `do-while` (menu, validação) |
| Funções | ~45 funções pequenas, com parâmetros por valor e por referência |
| Vetores | `regioes[]`, `vector<Ocorrencia>`, contadores `totalRegiao[]`, `dist[]`, `visitado[]`, `anterior[]` |
| Matriz | `dist_carro[N][N]` e `dist_pe[N][N]` (mapa) |
| Busca sequencial | `buscarPorRegiao`, `buscarPorCategoria`, `calcularLimiar`, `calcularSeguro`, `menorNaoVisitado` |
| Ordenação | selection sort (ranking), bubble sort (chuva), insertion sort (data) |
| Contadores e acumuladores | `estatisticas`, `estimarRisco`, `distanciaReal` |

---

## 1. Estruturas de dados

### O registro (`struct Ocorrencia`)
```
┌─────────────────────────────────────────────────────────────┐
│ regiao=1 │ dia=14 │ mes=1 │ ano=2026 │ chuva_mm=60.0 │ categoria=1 │ alagou=true │
└─────────────────────────────────────────────────────────────┘
   índice em regioes[]                         0 leve · 1 intransitável · 2 correnteza
```
A região é guardada como **número** (índice do vetor `regioes[]`). Comparar números é mais
simples e rápido que comparar textos, e o nome sai de `regioes[o.regiao]`.

### O vetor de ocorrências
```
 ocorrencias:  [0]      [1]      [2]   ...   [34]
              Limão    Limão    Limão        Vila Maria
              45 mm    30 mm    52 mm        95 mm
```
Usamos `vector<Ocorrencia>` porque o cadastro aumenta o tamanho (`push_back`).

### O mapa (matriz de adjacência)
Cada célula `[i][j]` guarda a distância em km entre a região i e a região j (0 = sem ligação).
São **duas** matrizes: `dist_carro` e `dist_pe`. A pé existem ligações extras.

```
              Pirituba
             /   |    \
          4.0   3.5    5.0
           /     |       \
   Brasilândia─3.0─Freguesia─4.0──Lapa
          \        |     \          |
          3.5     3.0    2.5       3.0
            \      |       \        |
          Cachoeirinha─2.5─Limão─3.5─Barra Funda
             │   \           |      /     |
             │    3.5       2.0   4.0    5.0
             │       \       |   /        |
        (a pé) 4.0   Casa Verde ──3.0── Santana ──4.0── Vila Maria
             │           ╲                │               ╱
             └────────────╲───────────────┘       (a pé) 4.5
                           ╲_____________________________╱

  ─── carro e a pé        (a pé) = só pedestre
  Só a pé: Cachoeirinha–Santana (escadaria/trilha) e Casa Verde–Vila Maria (passarela)
```

**Resumo para a apresentação:** "Cada ocorrência é um registro (`struct`) guardado num vetor.
O mapa da Zona Norte é uma matriz de adjacência: a linha e a coluna são regiões e o valor é a
distância. Temos uma matriz para carro e outra para pedestre, porque a pé dá para usar escadarias
e passarelas."

---

## 2. Leitura validada: `lerInteiro`, `lerDecimal`, `textoParaInteiro`

**O que faz:** lê a linha inteira como texto e só aceita se ela for um número dentro do intervalo.
Se não for, mostra o erro e pergunta de novo, dentro de um `while`.

```
 usuário digita "abc"  → textoParaInteiro: 'a' não é dígito → falso → "Valor inválido"
 usuário digita "9"    → é número, mas 9 > 7              → "Fora do intervalo"
 usuário digita "3"    → ok → retorna 3
```
A conversão é feita caractere por caractere: `resultado = resultado * 10 + (c - '0')`.
```
 "52":  0*10+5 = 5  →  5*10+2 = 52
```
`lerDecimal` aceita vírgula **ou** ponto (`47,5` e `47.5`). A data é validada com `diasNoMes`,
que considera ano bissexto, então 30/02 ou 29/02/2025 são recusados.

**Conteúdo:** repetição (`while`), decisão, funções, tipos.
**Resumo:** "O programa nunca trava com entrada errada: lemos tudo como texto, verificamos
caractere por caractere se é número e se está no intervalo, e repetimos a pergunta até vir um
valor válido."

---

## 3. Opção 1: `cadastrarOcorrencia`

**O que faz:** pergunta região, data, chuva e categoria, monta um `Ocorrencia` e coloca no fim do
vetor com `push_back`. Se a categoria for "via intransitável" ou "correnteza", `alagou` já é
`true`. Só no "alagamento leve" o programa pergunta se a região alagou.

```
 antes:  [0][1]...[34]            tamanho 35
 depois: [0][1]...[34][35]        tamanho 36   ← nova ocorrência no fim
```
**Conteúdo:** vetores, decisão, funções, validação.
**Resumo:** "É a parte colaborativa: qualquer cidadão registra uma enchente e ela entra no
histórico. Cada registro novo melhora a estimativa de risco."

---

## 4. Opção 2: `listarTodas`

**O que faz:** percorre o vetor do índice 0 ao último com `for` e imprime cada linha da tabela.
A função `coluna()` alinha a tabela mesmo com acentos (em UTF-8, "ã" ocupa 2 bytes, então
contamos só os bytes que começam um caractere).

**Conteúdo:** estrutura de repetição, vetores.
**Resumo:** "Um `for` passa por todas as posições do vetor e imprime cada ocorrência numa tabela."

---

## 5. Opção 3: busca sequencial (`buscarPorRegiao`, `buscarPorCategoria`)

**O que faz:** compara **um por um**, do primeiro ao último, se a região (ou categoria) do
registro é a procurada. Um **contador** guarda quantos foram encontrados.

```
 Procurando região = Limão (1)

 i:        0     1     2    ...   6         7  ...  34
 regiao:   1     1     1          1         2       6
           ✔     ✔     ✔          ✔         ✘       ✘
 encontradas: 1 → 2 → 3 → ... → 6
```
Se o contador termina em 0, aparece "Nenhuma ocorrência encontrada".

**Conteúdo:** busca sequencial, contador, `for` + `if`.
**Resumo:** "A busca sequencial olha cada registro do vetor, do início ao fim, e mostra os que
batem com o filtro. É simples e funciona com o vetor fora de ordem; o custo é proporcional ao
número de registros, O(n)."

---

## 6. Opção 4: `estatisticas` (contadores e acumuladores)

**O que faz:** **uma única passada** pelo vetor alimenta todos os contadores:
```
 para cada ocorrência o:
     totalRegiao[o.regiao]++          ← contador indexado pela região
     totalCategoria[o.categoria]++    ← contador indexado pela categoria
     se o.alagou:
         enchentesRegiao[o.regiao]++
         somaChuvaEnchente += o.chuva  ← ACUMULADOR
         qtdEnchentes++                ← CONTADOR
```
```
 totalRegiao:  [Freg][Limão][CVerde][Bras][Pirit][Sant][VMaria][Lapa][BFunda][Cach]
                 3     6      4       3     3      2     4       3     4       3
```
- Chuva média que causou enchente = `somaChuvaEnchente / qtdEnchentes` = 1380 / 19 = **72,6 mm**
- Região mais crítica = busca do **maior** valor em `enchentesRegiao[]` → **Limão (4)**

**Conteúdo:** contadores, acumuladores, vetor usado como "tabela de contagem".
**Resumo:** "Usamos vetores de contadores em que a posição é a região. Numa passada só contamos
tudo e somamos a chuva das enchentes num acumulador; a média é a soma dividida pela quantidade."

---

## 7. Opção 5: ordenação (três algoritmos)

Ordenamos uma **cópia** do vetor (passada por valor), para não bagunçar a ordem original.

### 7.1 Ranking de regiões: **Selection Sort**
A cada passo, **seleciona** a região com mais enchentes entre as que faltam e troca com a posição atual.
```
 enchentes:  Freg=1 Limão=4 CV=3 Bras=2 Pir=1 Sant=1 VM=3 Lapa=1 BF=2 Cach=1

 passo 0: maior entre todas = Limão(4)  → troca com posição 0
          [Limão | Freg  CV  Bras ...]
 passo 1: maior entre o resto = CV(3)   → troca com posição 1
          [Limão  CV | Freg  Bras ...]
 passo 2: maior entre o resto = VM(3)
          [Limão  CV  VM | ...]
 ...
 final:   Limão 4, Casa Verde 3, Vila Maria 3, Brasilândia 2, Barra Funda 2, ...
```
Na verdade ordenamos um vetor de **índices** (`indice[]`), não os nomes: trocar números é mais simples.

### 7.2 Ocorrências por chuva: **Bubble Sort** (decrescente)
Compara vizinhos e troca se estiverem fora de ordem. A menor chuva "afunda" para o fim a cada passada.
```
 [45, 30, 52, 38]
 passada 1: 45>30 ok | 30<52 troca → [45,52,30,38] | 30<38 troca → [45,52,38,30]
 passada 2: 45<52 troca → [52,45,38,30] | 45>38 ok
 passada 3: nenhuma troca → PARA (otimização com a flag "trocou")
```

### 7.3 Ocorrências por data: **Insertion Sort**
Como arrumar cartas na mão: pega o próximo e **insere** na posição certa entre os já ordenados.
A data vira número para comparar fácil: `14/01/2026 → 20260114`.
```
 [2024-02, 2024-01, 2025-03]
 pega 2024-01: é menor que 2024-02 → empurra 2024-02 para a direita → [2024-01, 2024-02, 2025-03]
 pega 2025-03: já é maior que todos → fica
```

**Conteúdo:** algoritmos de ordenação, vetores, repetições aninhadas.
**Resumo:** "Implementamos três ordenações à mão. Selection sort monta o ranking de regiões,
bubble sort ordena por chuva e insertion sort por data. Todos são O(n²), o que é suficiente para
dezenas de registros e fácil de explicar."

---

## 8. Opção 6: estimar risco (`calcularLimiar`, `calcularSeguro`, `riscoRegiao`)

**Ideia:** o histórico diz com quanta chuva cada região já alagou.

- **limiar** = **menor** chuva que já causou enchente na região (busca sequencial do mínimo)
- **seguro** = **maior** chuva que **não** causou enchente (busca sequencial do máximo)

```
 Limão no histórico:  30 não · 38 não · 45 SIM · 52 SIM · 60 SIM · 75 SIM
                              ↑ seguro = 38   ↑ limiar = 45

      0 ────────── 38 ───── 45 ─────────────────► mm
         BAIXO      │ MÉDIO  │        ALTO
                 seguro    limiar
```
Regra de decisão:
```
 se a região nunca alagou        → BAIXO
 senão, se previsão ≥ limiar      → ALTO   (já alagou com essa chuva ou menos)
 senão, se previsão > seguro      → MÉDIO  (zona incerta)
 senão                            → BAIXO
```
Com previsão de **60 mm**: Limão e Casa Verde ficam ALTO; Freguesia, Brasilândia, Vila Maria,
Lapa e Barra Funda ficam MÉDIO; Pirituba, Santana e Cachoeirinha ficam BAIXO.

**Conteúdo:** busca sequencial (mínimo e máximo com condição), estruturas de decisão encadeadas, contador por nível.
**Resumo:** "Para cada região, buscamos no histórico a menor chuva que já causou enchente e a
maior que não causou. Comparando a previsão com esses dois valores, classificamos o risco em
baixo, médio ou alto. Quanto mais gente reporta, mais preciso fica."

---

## 9. Opção 7: rota mais segura (Dijkstra)

### Ideia
É o "Google Maps contra enchentes". O Dijkstra acha o caminho de **menor custo**. A diferença
é que o custo não é só a distância:

```
 custo da aresta u→v = km × fator(risco de v)

                 carro    a pé
   BAIXO          ×1       ×1
   MÉDIO          ×3       ×2
   ALTO       bloqueado  bloqueado
```
Assim, uma rua que entra numa região MÉDIO "parece" 3× mais longa para o carro, e o algoritmo
prefere outro caminho se existir. Região ALTO nunca entra na rota.

### Os três vetores
```
 dist[v]      = menor custo conhecido da origem até v (começa em INFINITO, origem = 0)
 visitado[v]  = v já foi resolvido? (começa false)
 anterior[v]  = de onde eu vim para chegar em v (serve para remontar o caminho)
```

### O algoritmo
```
 repita:
   u = menorNaoVisitado(dist, visitado)     ← BUSCA SEQUENCIAL (sem priority_queue)
   se não há u: pare
   visitado[u] = true
   para cada vizinho v de u (matriz[u][v] ≠ 0):
       se v é ALTO: pule (bloqueado)
       novo = dist[u] + km × fator
       se novo < dist[v]: dist[v] = novo; anterior[v] = u   ← "relaxamento"
```

### Exemplo real do programa: Freguesia do Ó → Santana, **carro**, 60 mm
Limão e Casa Verde estão em ALTO (bloqueadas).
```
 Passo 1: visita Freguesia (0)
          Brasilândia = 3.0×3 = 9.0   Pirituba = 3.5×1 = 3.5
          Lapa = 4.0×3 = 12.0         Cachoeirinha = 3.0×1 = 3.0
          (Limão bloqueado)
 Passo 2: visita Cachoeirinha (3.0)  → vizinhos Limão e Casa Verde bloqueados;
                                        Santana só a pé, carro não passa
 Passo 3: visita Pirituba (3.5)      → Lapa via Pirituba = 3.5+15 = 18.5 > 12, não troca
 Passo 4: visita Brasilândia (9.0)
 Passo 5: visita Lapa (12.0)         → Barra Funda = 12 + 3×3 = 21.0
 Passo 6: visita Barra Funda (21.0)  → Santana = 21 + 5×1 = 26.0
 Passo 7: visita Santana (26.0) ✔

 dist:     Freg  Lim  CV   Bras Pir  Sant VM   Lapa BF   Cach
           0     ∞    ∞    9    3.5  26   38   12   21   3
 anterior: -     -    -    Freg Freg BF   Sant Freg Lapa Freg
```
### Remontando o caminho com `anterior[]` (de trás para frente)
```
 Santana ← Barra Funda ← Lapa ← Freguesia     (invertido)
 Freguesia → Lapa → Barra Funda → Santana     (na ordem certa)
 distância real = 4.0 + 3.0 + 5.0 = 12.0 km   (acumulador, sem os fatores)
```
### Comparação com a rota "normal"
Rodamos o Dijkstra de novo **sem** risco (todos os fatores 1, nada bloqueado):
```
 normal: Freguesia → Limão → Casa Verde → Santana   7.5 km
 segura: Freguesia → Lapa → Barra Funda → Santana  12.0 km
 Regiões evitadas: Limão (ALTO), Casa Verde (ALTO)  ·  Desvio: +4.5 km
```
**A pé**, com a mesma chuva, a escadaria Cachoeirinha–Santana permite
`Freguesia → Cachoeirinha → Santana` (7.0 km). **Mesma origem e destino, rotas diferentes.**

Com **75 mm** não existe rota de carro (Barra Funda, Casa Verde e Vila Maria, as únicas entradas
de carro em Santana, estão em ALTO), mas a pé a escadaria continua salvando.

**Conteúdo:** vetores, matriz, busca sequencial, repetição, decisão, funções, acumulador.
**Resumo:** "Usamos o algoritmo de Dijkstra, o mesmo princípio de apps de mapa. A diferença é
que o peso de cada rua é a distância multiplicada pelo risco de enchente da região de chegada, e
regiões com risco alto ficam bloqueadas. O app mostra a rota segura, a distância real e quais
regiões foram evitadas."

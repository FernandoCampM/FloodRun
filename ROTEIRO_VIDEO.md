# Roteiro do vídeo (até 3 minutos)

**Antes de gravar**
- Compile (`g++ -Wall -Wextra -std=c++17 -o floodrun main.cpp`) e abra o terminal em tela
  cheia, com fonte grande (Ctrl + roda do mouse).
- Rode `.\floodrun.exe` uma vez antes para ensaiar. Os números das regiões estão em
  [ROTEIRO_TESTE.md](ROTEIRO_TESTE.md).
- Grave a tela (OBS, Xbox Game Bar com Win+G ou Clipchamp) e narre por cima. Se ficar longo,
  corte as pausas na edição.

---

### 0:00 – 0:25 · O problema (câmera ou slide)
> "Todo verão as chuvas alagam a Zona Norte de São Paulo: Limão, Casa Verde, Barra Funda. Os apps
> de mapa mostram o caminho mais curto, que muitas vezes passa justamente pela rua que está
> enchendo. O nosso projeto, o FloodRun, conecta-se aos ODS 11, 9 e 13 da ONU: cidades
> resilientes e adaptação às mudanças climáticas."

**Tela:** slide com o título, uma foto de rua alagada e os ícones dos ODS 11, 9 e 13.

### 0:25 – 0:45 · A proposta
> "O cidadão registra as enchentes que vê. Com esse histórico, o sistema estima quais regiões
> vão alagar para a chuva prevista e calcula a rota mais segura, a pé ou de carro. Os dados são
> simulados para a demonstração."

**Tela:** o desenho em ASCII do fluxo (seção 0 de `EXPLICACAO.md`) ou o diagrama de atividades.

### 0:45 – 1:15 · Requisitos básicos
1. Opção `2`: mostra a tabela e rola rápido. *"35 ocorrências guardadas num vetor de structs."*
2. Opção `3` → `1` → `2`: busca por Limão. *"Busca sequencial: compara um por um."*
3. Opção `4`: *"Contadores e acumuladores: média de 72,6 mm, e a região mais crítica é o Limão."*
4. Opção `5` → `1`: ranking com barras. *"Ranking feito com selection sort; temos também bubble e insertion sort."*

### 1:15 – 1:35 · O cidadão colabora
Opção `1` → `2` (Limão) → `2026` → `3` → `10` → `40` → `2`.
> "Qualquer pessoa registra uma ocorrência, com validação de data, número e intervalo."

*(Opcional, se sobrar tempo: digite `abc` no menu para mostrar a validação.)*

### 1:35 – 1:50 · Risco por região
Opção `6` → `60`.
> "Para 60 mm de chuva, o sistema olha no histórico a menor chuva que já alagou cada região e
> a maior que não alagou. Limão e Casa Verde ficam em risco ALTO."

### 1:50 – 2:35 · ⭐ A rota segura (o diferencial)
1. Opção `7` → `1` (Freguesia) → `6` (Santana) → `2` (carro) → `60` → `1` (passo a passo).
   > "Usamos Dijkstra. O peso de cada rua é a distância vezes o risco da região, e as regiões em
   > risco alto ficam bloqueadas. A rota normal passaria por Limão e Casa Verde; o FloodRun
   > desvia pela Lapa: 4,5 km a mais, mas sem enchente."
2. Opção `7` → `1` → `6` → `1` (a pé) → `60` → `2`.
   > "A pé, a mesma viagem usa uma escadaria que o carro não pode usar: 7 km por Cachoeirinha."

### 2:35 – 2:50 · Chuva extrema
Opção `7` → `1` → `6` → `2` → `75` → `2`.
> "Com 75 mm não existe rota segura de carro, e o app avisa em vez de mandar o motorista para a
> água. A pé, ainda existe um caminho."

### 2:50 – 3:00 · Encerramento
> "FloodRun: informação colaborativa que vira decisão segura. Obrigado!"

**Tela:** opção `0` (mensagem "Fique seguro!") e um slide final com os nomes e RMs do grupo.

---

**Dica para dividir entre a dupla:** Fernando narra o problema, a proposta e os requisitos
básicos (0:00–1:35); André narra o risco, a rota segura e o encerramento (1:35–3:00).
Slide final: Fernando Mota – RM567451 · André da Silva Pinto – RM559038.

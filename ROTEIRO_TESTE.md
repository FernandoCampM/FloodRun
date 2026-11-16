# Roteiro de teste e demonstração (vídeo)

Compilar e rodar:
```
g++ -Wall -Wextra -std=c++17 -o floodrun main.cpp
.\floodrun.exe
```
Números das regiões no menu: 1 Freguesia do Ó · 2 Limão · 3 Casa Verde · 4 Brasilândia ·
5 Pirituba · 6 Santana · 7 Vila Maria · 8 Lapa · 9 Barra Funda · 10 Cachoeirinha

## A. Requisitos mínimos
| # | Digite | Esperado |
|---|---|---|
| A1 | `2` | Tabela com 35 ocorrências |
| A2 | `3` → `1` → `2` | 6 ocorrências em Limão, "de 35 comparadas" |
| A3 | `3` → `2` → `3` | 6 ocorrências "Enchente c/ correnteza" |
| A4 | `4` | Média 72,6 mm · região mais crítica: **Limão (4)** |
| A5 | `5` → `1` | Ranking com barras: Limão 4, Casa Verde 3, Vila Maria 3… |
| A6 | `5` → `2` | Da maior chuva (95 mm) para a menor (30 mm) |
| A7 | `5` → `3` | De 12/01/2024 até 02/02/2026 |

## B. Validação de entrada
| # | Digite | Esperado |
|---|---|---|
| B1 | `abc` no menu | "Valor inválido: digite um número inteiro" |
| B2 | `9` no menu | "Fora do intervalo: digite de 0 a 7" |
| B3 | Cadastro com ano `2025`, mês `2`, dia `29` | Recusa (fevereiro de 2025 tem 28 dias) |
| B4 | Chuva `47,5` | Aceita com vírgula |
| B5 | Rota com destino igual à origem | "O destino precisa ser diferente da origem" |

## C. Cadastro que muda o resultado (mostra o "colaborativo")
1. `6` → `40` → Limão aparece **MÉDIO** (40 > seguro 38, < limiar 45).
2. `1` → região `2` (Limão) → `2026` → `3` → `10` → chuva `40` → categoria `2` (intransitável).
3. `6` → `40` → agora Limão aparece **ALTO** (o novo limiar é 40).

## D. Risco e rota ⭐ (o diferencial)

### D1. Mapa de risco com chuva forte
`6` → `60` → 2 ALTO (Limão, Casa Verde), 5 MÉDIO, 3 BAIXO.

### D2. Mesma origem e destino, **pé × carro**, 60 mm ⭐
**De carro**: `7` → origem `1` (Freguesia) → destino `6` (Santana) → `2` (carro) → `60` → `1` (passo a passo)
```
✔ Freguesia do Ó → Lapa → Barra Funda → Santana    12.0 km
  Rota normal: Freguesia → Limão → Casa Verde → Santana (7.5 km)
  Regiões evitadas: Limão (ALTO), Casa Verde (ALTO) · Desvio +4.5 km
```
**A pé**: `7` → `1` → `6` → `1` (a pé) → `60` → `2`
```
✔ Freguesia do Ó → Cachoeirinha → Santana    7.0 km   (escadaria só para pedestres)
```

### D3. Sem chuva o carro passa por dentro
`7` → `1` → `6` → `2` → `20` → `2` → Freguesia → Limão → Casa Verde → Santana (7.5 km), nada evitado.

### D4. Chuva extrema: carro sem rota, a pé ainda dá ⭐
- `7` → `1` → `6` → `2` (carro) → `75` → `2` → **"NÃO existe rota segura de carro"** + dica de ir a pé
- `7` → `1` → `6` → `1` (a pé) → `75` → `2` → Freguesia → Cachoeirinha → Santana (7.0 km)
  (com aviso de que a origem está em risco ALTO)

### D5. Destino alagado
`7` → `2` (Limão) → `7` (Vila Maria) → `1` → `75` → `2` → "O destino Vila Maria está em risco ALTO… Não existe rota segura"

## Ordem sugerida para o vídeo (3 min)
1. (0:00–0:25) Problema: enchentes na Zona Norte, ODS 11/13.
2. (0:25–0:50) A1, A4, A5: histórico, estatísticas e ranking.
3. (0:50–1:10) A2 e cadastro rápido (C2): o cidadão colabora.
4. (1:10–1:30) D1: mapa de risco para 60 mm.
5. (1:30–2:30) D2 com passo a passo e depois a pé: **o diferencial**.
6. (2:30–3:00) D4: carro sem rota e a pé seguro; encerramento.

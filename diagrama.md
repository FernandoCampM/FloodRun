# Diagrama de Atividades: FloodRun

Fluxo principal: **registro → consulta → análise**. A versão em UML/PlantUML está em
[`diagrama.puml`](diagrama.puml). Para gerar a imagem, cole o
arquivo em <https://www.plantuml.com/plantuml>. Esta versão em Mermaid aparece direto no GitHub
e pode ser exportada como PNG em <https://mermaid.live>.

```mermaid
flowchart TD
    A([Início]) --> B[Carregar mapa e histórico simulado]
    B --> M{Escolher opção no menu}

    M -->|1 Cadastrar| C1[/Região, data, chuva, categoria/]
    C1 --> C2{Dados válidos?}
    C2 -->|não| C1
    C2 -->|sim| C3{Categoria = leve?}
    C3 -->|sim| C4[/Alagou? sim/não/]
    C3 -->|não| C5[alagou = sim]
    C4 --> C6[Adicionar ao vetor de ocorrências]
    C5 --> C6

    M -->|2 Listar / 3 Buscar| D1[Busca sequencial no vetor]
    D1 --> D2[Exibir resultados + contador]

    M -->|4 Estatísticas| E1[Contadores e acumuladores]
    E1 --> E2[Totais, chuva média, região mais crítica]

    M -->|5 Ordenar| F1[Selection / Bubble / Insertion sort]
    F1 --> F2[Exibir lista ordenada]

    M -->|6 Risco| G1[/Previsão de chuva mm/]
    G1 --> G2[Limiar e seguro por região]
    G2 --> G3[Classificar BAIXO / MÉDIO / ALTO]

    M -->|7 Rota| H1[/Origem, destino, pé ou carro, previsão/]
    H1 --> H2[Calcular risco de cada região]
    H2 --> H3{Destino em risco ALTO?}
    H3 -->|sim| H4[Avisar: sem rota segura]
    H3 -->|não| H5[Dijkstra: peso = km × fator, ALTO bloqueado]
    H5 --> H6{Destino alcançado?}
    H6 -->|não| H7[Avisar: sem rota segura, sugerir a pé]
    H6 -->|sim| H8[Exibir rota, km reais e regiões evitadas]

    C6 --> M
    D2 --> M
    E2 --> M
    F2 --> M
    G3 --> M
    H4 --> M
    H7 --> M
    H8 --> M

    M -->|0 Sair| Z([Fim])
```

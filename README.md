# Concorrencia1

Cada arquivo C eh um programa separado; nao existe um binario unico principal. Compile e execute o arquivo do exercicio que quiser rodar.

## porextenso.c (Trab 1)
- Compilar: `gcc -pthread porextenso.c -o porextenso`
- Executar: `./porextenso`
- Comportamento: 5 threads geradoras criam numeros aleatorios (0-999), a thread porExtenso converte cada numero para texto e retorna para impressao; encerra apos todas as iteracoes (LOOPS por thread).

## prodconsamb.c (Trab 2)
- Compilar: `gcc -pthread prodconsamb.c -o prodconsamb`
- Executar: `./prodconsamb <tambuffer> <loops> <produtores> <consumidores> <ambos>`
- Exemplo: `./prodconsamb 10 5 2 2 2`
- Comportamento: produtores colocam itens no buffer 1; threads "ambos" consomem do buffer 1 e produzem no buffer 2; consumidores consomem do buffer 2; o programa finaliza quando toda a producao foi escoada.

## leitorescritor.c
- Compilar: `gcc -pthread leitorescritor.c -o leitorescritor`
- Executar: `./leitorescritor`
- Comportamento: exemplo de controle de leitores-escritores usando pthreads.

Observacoes gerais:
- Requer POSIX threads (`-pthread`) para compilar.
- Sem dependencias externas adicionais.

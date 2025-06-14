# Atividade-n1at2
Atividade avaliativa individual da matéria de estrutura de dados - Universidade Católica de Brasília

O programa 1 vai realizar várias leituras de sensores e separar essas leituras em arquivos diferentes de acordo com cada sensor
O programa 2 irá realizar uma busca binária para encontrar a leitura mais próxima do tempo inserido
O programa 3 gera os arquivos no dados.txt com os sensores tendo 2000 linhas para cada sensor


Exemplos e ordem para compilação dos programas no terminal

Criar os arquivos executáveis:

gcc programa1.c -o programa1
gcc programa2.c -o programa2
gcc programa3.c -o programa3


Ordem de execução dos programas:

Programa 3 - ./programa3 2025-06-14 10:10:00 2025-06-15 10:10:00 sensorA int sensorB float sensorC bool sensorD string

Programa 1 - ./programa1 dados.txt

Programa 2 - ./programa2 sensorA 2025-06-12 15:50:00

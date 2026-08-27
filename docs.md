# Requisitos
- [ x ] 4kb RAM (4096 bytes de RAM)
- [ x ] tela 64x32 (aqui eu aumentei a escala por 10x pra nao ficar pequeno)
- [ ] SDL para denhar na tela
- [ x ] Conseguir ler arquivos binarios de programas CHIP-8
- [ ] Um contador de programas, frequentemente chamado apenas de "PC", que aponta para a instrução atual na memória
- [ ] Um registrador de índice de 16 bits chamado "I", usado para apontar locais na memória
- [ ] Uma pilha para endereços de 16 bits, usada para chamar sub-rotinas/funções e retornar delas
- [ ] Um temporizador de atraso de 8 bits que é decrementado a uma taxa de 60 Hz (60 vezes por segundo) até chegar a 0
- [ ] Um timer de som de 8 bits que funciona como o temporizador de atraso, mas que também emite um som de bip desde que não seja 0
- [ ] 16 registradores variáveis de uso geral de 8 bits (um byte) numerados por hexadecimal, ou seja, 0 a 15 em decimal, chamado por 0FV0VF.
  VF também é usado como registro de bandeiras; muitas instruções o definem para 1 ou 0 com base em alguma regra, por exemplo, usando-o como uma bandeira de carry

memória
0x000 ─┐
│ área reservada
0x1FF ─┘

0x200 ─┐
│ ROM / programa
│
0xFFF ─┘
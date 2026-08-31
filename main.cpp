#include <bitset>
#include <iomanip>
#include <iostream>
#include <SDL3/SDL.h>

/**
 * Para nao ficar tao pequeno, deixamos a escala da tela x10
 */
constexpr int SCREEN_WIDTH = 64;
constexpr int SCREEN_HEIGHT = 32;
constexpr int SCALE = 10;
constexpr int RAM_SIZE = 4096;

constexpr int LINHAS_FONTE = 16;
constexpr int COLUNAS_FONTE = 5;

struct {

   //uint8_t representa inteiros sem sinal de 8 bits (1 byte)
   //uint16_t representa inteiros sem sinal de 16 bits (2 bytes)

   uint8_t fontes[80] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
   };

   //memoria
   // Area reservada de 0x000-0x1FF
   uint8_t memoriaRam[RAM_SIZE];

   //registradores
   uint8_t V[16]; //16 registradores de 8 bits. V0, V1, V2, ... VE, VF
   uint16_t I = 0; // registrador de endereço. Usado para apontar locais na memória

   //program counter // programas chip-8 são carregados na memória a partir do endereço 0x200
   uint16_t PC = 0x200; //endereço da próxima instrução. Começa em 0x200 pois é onde a ROM inicia, ou seja, a primeira instrução dela

   uint16_t opcode; // sequencia de bits que representa uma instrução especifica (ex: aritimético, lógico, controle. Tais como: adição, comparação, subtração, etc)

   // struct {
   //    // identifica o tipo da instrução
   //    uint16_t tipoDaInstrucao;
   //
   //    // qual o registrador V que deve ser usado
   //    uint16_t registrador;
   //
   //    //qual o valor que vai ser armazenado nesse registrado
   //    uint16_t valor;
   // } DECODE_TESTE;

   uint8_t display[32][64];
   // timers;
   // teclado;

} Chip8;

//apenas reservando a memória
void reservaMemoria() {
   for (unsigned char& i : Chip8.memoriaRam) {
      i = 0;
   }
}

void adicionaFonteNaMemoria() {
   for (int i = 0; i < sizeof(Chip8.fontes); i++) {
      Chip8.memoriaRam[0x050 + i] = Chip8.fontes[i];
   }
}

void renderizaPixelsTeste(SDL_Renderer* renderer) {
   int posicao = 100;

   //DESENHA NA TELAQ
   for (int linha = 0; linha < 5; linha++) {
      uint8_t byteLinha = Chip8.fontes[linha];

      for (int coluna = 0; coluna < 4; coluna++) {

         //usamos o 0x80 pois ele representa 10000000. Deslocamos conforme a coluna.
         //Ex: se coluna = 0 -> 10000000 e então comparamos como o byteLinha
         //Ex: se bytelinha = 11110000 -> 11110000 & 10000000 = 10000000 (ou seja, resultado verdadeiro)
         //Os 1's do byte, representam ligado, ou seja, será onde o pixel vai aparecer
         //EX: se bytelina = 01000000 e coluna = 10000000 -> 01000000 & 10000000 = 00000000 (ou seja, resultado false, sem pixel para desenhar)
         bool pixelLigado = byteLinha & (0x80 >> coluna);
         if (pixelLigado) {
            SDL_FRect pixel = {
               static_cast<float>(100 + coluna * SCALE), //posição X que deve ser pintado
               static_cast<float>(100 + linha * SCALE), //posição Y que deve ser pintado
               static_cast<float>(SCALE), //largura que deve ser pintado
               static_cast<float>(SCALE) //altura que deve ser pintado
            };

            SDL_RenderFillRect(renderer, &pixel);
         }
      }
      //muda a cada caractere
      posicao++;

   }

}

void renderizaPixels(SDL_Renderer* renderer) {
   for (int linha = 0; linha < SCREEN_HEIGHT; linha++) {
      for (int coluna = 0; coluna < SCREEN_WIDTH; coluna++) {

         bool pixelLigado = Chip8.display[linha][coluna];
         if (pixelLigado) {
            SDL_FRect pixel = {
               static_cast<float>(coluna * SCALE), //posição X que deve ser pintado
               static_cast<float>(linha * SCALE), //posição Y que deve ser pintado
               static_cast<float>(SCALE), //largura que deve ser pintado
               static_cast<float>(SCALE) //altura que deve ser pintado
            };

            SDL_RenderFillRect(renderer, &pixel);
         }
      }
   }
}

void executaLimpezaDoDiplsay() {
   for (int linha = 0; linha < SCREEN_HEIGHT; linha++) {
      for (int coluna = 0; coluna < SCREEN_WIDTH; coluna++) {
         Chip8.display[linha][coluna] = 0;
      }
   }
}

/**
 * Carrega a ROM na "memoria" RAM
 */
void loadROM() {
   //1 - abre o arquivo
   //rb significa read binary
   FILE* rom = fopen("rom_teste/IBM Logo.ch8", "rb");
   // FILE* rom = fopen("rom_teste/Pong (1 player).ch8", "rb");

   //2 - verifica se encontrou o arquivo
   if (rom == nullptr) {
      throw std::runtime_error("Arquivo não existe");
   }

   //3 - Descrobrir tamanho do arquivo
   fseek(rom, 0, SEEK_END);
   long sizeRom = ftell(rom);
   //vai pro inicio do arquivo
   rewind(rom);

   //4 - verifica se a ROM cabe na memoria
   if (sizeRom > (RAM_SIZE - 0x200)) {
      throw std::runtime_error("Arquivo muito grande");
   }

   //5 - Le cada byte do arquivo e passa para a memoria ram
   for (int i = 0; i < sizeRom; i++) {
      int c = fgetc(rom); //TODO verificar outras maneiras de obter os bytes
      if (c == EOF) break;

      //ROM do programa começa em 0x200
      Chip8.memoriaRam[0x200 + i] = static_cast<uint8_t>(c);
   }

   fclose(rom);
}

/**
 * Etapa comumente chamada de FETCH: onde a CPU busca a próxima instrução da memória no PC
 */
void executarCicloFETCH() {

   uint8_t primeiroByte = Chip8.memoriaRam[Chip8.PC];

   uint8_t segundoByte = Chip8.memoriaRam[Chip8.PC + 1];

   //as instruções nas ROM's são sempre formadas por um conjunto de 2 bytes. Ex: 0x61 + 0x0A = 0x610A
   //"<<" significa shift left, desloca X bits à esquerda
   Chip8.opcode = (primeiroByte << 8) | segundoByte; //instrução montada

   //próxima instrução
   Chip8.PC += 2;
}

// /**
//  * Etapa em que a CPU pega o opcode que veio do FETCH e descobre/decifra o que ele significa. Ele interpreta o opcode,
//  * para que o EXECUTE saiba o que fazer
//  */
// void exemploDECODE_TESTE() {
//    //1 - descobrir a familia da instrução
//
//    uint16_t mascaraTipoInstrucao = 0xF000; //1111 0000 0000 0000
//    uint16_t mascaraParaValor = 0x00FF; //0000 0000 1111 1111
//    uint16_t *opcode = &Chip8.opcode;
//
//    /**
//     * Ex:
//     * 6 -> familia/tipo da instrução
//     * 1 -> registrador V
//     * 0A -> Valor para o registrador V
//     */
//
//
//    /**
//     *Ex:
//     * 0110 0001 0000 1010
//     * AND
//     * 1111 0000 0000 0000
//     * -------------------
//     * 0110 0000 0000 0000
//     */
//    Chip8.DECODE_TESTE.tipoDaInstrucao = *opcode & mascaraTipoInstrucao;
//    std::cout << "Tipo da instrução Binário: " << std::bitset<16>(mascaraTipoInstrucao) << std::endl;
//    std::cout << "Tipo da instrução Hexa: " << std::showbase << std::hex << Chip8.DECODE_TESTE.tipoDaInstrucao << std::endl;
//
//    /**
//     * Necessário deslocar 4 bits à direita para saber qual registrador vai obter a informação/valor.
//     * Do resultado, deslocamos 8 bits, para obter o valor único (é o registrador, porém com um valor simplificado)
//     */
//    Chip8.DECODE_TESTE.registrador = (*opcode & (mascaraTipoInstrucao >> 4)) >> 8;
//    std::cout << "Registrador Binário: " << std::bitset<16>(mascaraTipoInstrucao >> 4) << std::endl;
//    std::cout << "Registrador Hexa: " << std::showbase << std::hex << Chip8.DECODE_TESTE.registrador << std::endl;
//
//    /**
//     * Agora deslocamos 8 bits à direita para saber qual o valor que esse registrador deve receber
//     */
//    Chip8.DECODE_TESTE.valor = *opcode & mascaraParaValor;
//    std::cout << "Valor binário: " << std::bitset<16>(mascaraParaValor) << std::endl;
//    std::cout << "Valor Hexa: " << std::showbase << std::hex << Chip8.DECODE_TESTE.valor << std::endl;
// }

void executarCicloDECODEeEXECUTE() {
   uint16_t mascaraTipoInstrucao = 0xF000; //1111 0000 0000 0000
   uint16_t X;
   uint16_t Y;
   uint16_t N;
   uint16_t NN;
   uint16_t NNN;

   switch (Chip8.opcode & mascaraTipoInstrucao) {
      case 0x0000:
         if (Chip8.opcode == 0x00E0) {
            //CLEAR
            executaLimpezaDoDiplsay();
         }
         break;

      case 0x1000:
         //JUMP
         NNN = Chip8.opcode & 0x0FFF;
         Chip8.PC = NNN;
         break;
      case 0x3000:
         //Instrução de comparação/SKIP

         X = (Chip8.opcode & 0x0F00) >> 8;
         NN = Chip8.opcode & 0x00FF;

         //Se registrador V na posição X for igual ao valor NN, pulamos a instruçao de PC para a próxima instrução
         //Pula se igual
         if (Chip8.V[X] == NN) {
            Chip8.PC += 2; //pulamos uma instrução. Tem que ser += pq uma instrução no Chip8 ocupa 2 bytes
         }
         break;
      case 0x4000:
         //Instrução de comparação/SKIP
         //Basicamente quase a mesma coisa da instrução 3XNN (0x3000), porém só testamos o inverso

         X = (Chip8.opcode & 0x0F00) >> 8;
         NN = Chip8.opcode & 0x00FF;

         //Pula se diferente
         if (Chip8.V[X] != NN) {
            Chip8.PC += 2;
         }
         break;
      case 0x5000:

         if (Chip8.opcode & 0x000F) {
            //instrução que compara dois registradores e pula uma instrução se V[x] e V[y] forem iguais
            /**
             * 5XY0
             * 5 -> familia/tipo da instrução
             * X -> registrador V na posição X
             * Y -> Registrador V na posição Y
             * 0 -> confirma que é a instrução 5XY0
             */
            //Exemplo:
            // 0x5120
            // &
            // 0x0F00
            // =
            // 0x0100
            //
            // >> 8
            // =
            // 0x1
            X = (Chip8.opcode & 0x0F00) >> 8;
            Y = (Chip8.opcode & 0x00F0) >> 4;

            if (Chip8.V[X] == Chip8.V[Y]) {
               Chip8.PC += 2;
            }
         }
         break;
      case 0x9000:
         //9XY0
         //Faz o inverso da instrução 0x5000, se V[X] e V[Y] nao forem iguais, ai sim pula uma instrução
         X = (Chip8.opcode & 0x0F00) >> 8;
         Y = (Chip8.opcode & 0x00F0) >> 4;

         if (Chip8.V[X] != Y) {
            Chip8.PC += 2;
         }
         break;
      case 0x6000:
         //seta registrador V[X]
         /**
         * Ex:
         * 610A = 6XNN
         * 6 -> familia/tipo da instrução (familia aqui é o CASE do switch)
         * 1 -> registrador V
         * 0A -> Valor para o registrador V
         */
         X = (Chip8.opcode & 0x0F00) >> 8;
         NN = Chip8.opcode & 0x00FF;

         Chip8.V[X] = NN;
         break;

      case 0x7000:
         //Adiciona(Soma) valor no registrador V[X]
         X = (Chip8.opcode & 0x0F00) >> 8;
         NN = Chip8.opcode & 0x00FF;

         Chip8.V[X] += NN;
         break;
      case 0xA000:
         //seta registrador I
         NNN = Chip8.opcode & 0x0FFF;
         Chip8.I = NNN;
         break;
      case 0xD000:
         //DESENHA

         /**
          * DXYN
          * D -> familia/tipo da instrução (case do Switch)
          * X -> registrador V na posicao X
          * Y -> registrador V na posicao Y
          * N -> quantidades de linhhas a ser desenhada
          */
         //denhar -> A instrução de desenho vai usar os valores armazenados em: V[X], V[Y], 5 linhas
         //"Desenha um sprite na posição X"
         X = (Chip8.opcode & 0x0F00) >> 8;
         //"Na posição Y"
         Y = (Chip8.opcode & 0x00F0) >> 4;
         //"Com 5 linhas de altura"
         N = Chip8.opcode & 0x000F;

         uint8_t posX = Chip8.V[X];
         uint8_t posY = Chip8.V[Y];

         Chip8.V[15] = 0; //VF (ou V na posição 15) é o registrador usado como flag em várias instruções. No caso do DXYN, ele indica colisão.
         for (int linha = 0; linha < N; linha++) {
            if (linha + posY >= SCREEN_HEIGHT) break;

            uint16_t spriteByte = Chip8.memoriaRam[Chip8.I + linha];

            for (int coluna = 0; coluna < 8; coluna++) {
               if (coluna + posX >= SCREEN_WIDTH) break;

               if (spriteByte & (0x80 >> coluna)) { //Verifica se o pixel do sprite está ligado
                  if (Chip8.display[posY + linha][posX + coluna] == 1) {
                     Chip8.V[15] = 1;
                  }

                  Chip8.display[posY + linha][posX + coluna] ^= 1; //Se estiver, o pixel correspondente do display já está ligado?
               }

            }
         }
         break;
   }
}

int gameLoop() {

   if (!SDL_Init(SDL_INIT_VIDEO)) {
      SDL_Log("Erro ao inicializar SDL: %s", SDL_GetError());
      return 1;
   }

   SDL_Window* window = SDL_CreateWindow(
       "CHIP 8",
       SCREEN_WIDTH * SCALE,
       SCREEN_HEIGHT * SCALE,
       0
   );

   SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

   if (!window) {
      SDL_Log("erro ao inicializar SDL: %s", SDL_GetError());
      return 1;
   }

   bool running = true;

   while (running) {
      SDL_Event event;

      while (SDL_PollEvent(&event)) {
         if (event.type == SDL_EVENT_QUIT) {
            running = false;
         }
      }


      //Cor da tela
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

      // Preenche a janela com preto
      SDL_RenderClear(renderer);
      //cor da linha
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

      //DESENHA NA TELAQ
      // renderizaPixelsTeste(renderer);
      executarCicloFETCH();
      executarCicloDECODEeEXECUTE();
      renderizaPixels(renderer);


      // Mostra o frame
      SDL_RenderPresent(renderer);
   }

   SDL_DestroyWindow(window);
   SDL_Quit();

   return 0;
}


void initChip8() {
   reservaMemoria();
   loadROM();
   adicionaFonteNaMemoria();
   gameLoop();
}


int main() {
   initChip8();
   return 0;
}

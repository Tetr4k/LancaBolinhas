#include "PIG.h"

PIG_Evento evento;          //evento ser tratadoi a cada pssada do loop principal
PIG_Teclado meuTeclado;     //variável como mapeamento do teclado

struct Objeto{
    int x, y, desenho;
};

struct Fase{
    struct Objeto bola;
    vector<struct Objeto> alvos;
};

int main( int argc, char* args[] ){

    //criando o jogo (aplicação)
    CriaJogo("Lanca Bolinhas");
    SetTituloJanela("Carregando . . .");
    SetTamanhoJanela(720, 1280);

    //associando o teclado (basta uma única vez) com a variável meuTeclado
    meuTeclado = GetTeclado();

    //cria audios
    CarregaBackground("..//sounds//background.mp3");

    int somImpacto = CriaAudio("..//sounds//soundImpacto.mp3",0);
    SetVolume(somImpacto,25);

    //cria fundo
    int fundo = CriaSprite("..//img//fundo.png");//provisorio
    SetDimensoesSprite(fundo, 1280, 720);
    MoveSprite(fundo, 0, 0);

    //cria modelo da interface
    int interface = CriaSprite("..//img//interface.png");
    SetDimensoesSprite(interface, 1280, 100);
    MoveSprite(interface, 0, 0);

    //cria modelo dos alvos
    int *alvos = (int*) malloc(3*sizeof(int));
    alvos[0] = CriaObjeto("..//img//alvo.png");
    SetDimensoesObjeto(alvos[0], 50, 50);
    alvos[1] = CriaObjeto("..//img//alvo.png");
    SetDimensoesObjeto(alvos[1], 100, 100);
    alvos[2] = CriaObjeto("..//img//alvo.png");
    SetDimensoesObjeto(alvos[2], 200, 200);

    //cria modelo das bolas
    int *bolas = (int*) malloc(3*sizeof(int));
    bolas[0] = CriaObjeto("..//img//bola1.png");
    CarregaArquivoFramesObjeto(bolas[0], "..//img//bola1.txt");
    SetDimensoesObjeto(bolas[0], 48, 48);
    MudaFrameObjeto(bolas[0], 1);
    bolas[1] = CriaObjeto("..//img//bola2.png");
    CarregaArquivoFramesObjeto(bolas[1], "..//img//bola2.txt");
    SetDimensoesObjeto(bolas[1], 40, 40);
    MudaFrameObjeto(bolas[1], 1);
    bolas[2] = CriaObjeto("..//img//bola3.png");
    CarregaArquivoFramesObjeto(bolas[2], "..//img//bola3.txt");
    SetDimensoesObjeto(bolas[2], 40, 40);
    MudaFrameObjeto(bolas[2], 1);

    /*int seta = CriaSprite("..//img//seta.png");
    MoveSprite(seta,0,0);
    SetDimensoesSprite(seta, 626, 484);
    SetAnguloSprite(seta,15);*/

    struct Fase fase;
    int posX, posY, tipo;
    FILE *arq = fopen("../fases/fase1.txt", "r");
    fscanf(arq, "%d %d", &posY, &tipo);
    fase.bola.desenho = CriaObjeto(bolas[tipo]);
    fase.bola.x = 10;
    fase.bola.y = posY+100;

    struct Objeto alvo;
    while(!feof(arq)){
        fscanf(arq, "%d %d %d", &posX, &posY, &tipo);
        alvo.desenho = CriaObjeto(alvos[tipo]);
        alvo.x = posX;
        alvo.y = posY;
        fase.alvos.push_back(alvo);
    }
    fclose(arq);

    //loop principal do jogo

    //PlayBackground(); <-- descomentar

    SetTituloJanela("Lanca Bolinhas");

    while(JogoRodando()){

        //pega um evento que tenha ocorrido desde a última passada do loop
        evento = GetEvento();

        //aqui o evento deve ser tratado e as coisas devem ser atualizadas

        if(evento.teclado.tecla == PIG_TECLA_CIMA){
            //angulo++
        }
        if(evento.teclado.tecla == PIG_TECLA_BAIXO){
            //angulo--
        }

        //será feita aprepação do frame que será exibido na tela
        IniciaDesenho();

        //todas as chamadas de desenho devem ser feitas aqui na ordem desejada

        DesenhaSprite(fundo);
        DesenhaSprite(interface);

        MoveObjeto(fase.bola.desenho, fase.bola.x, fase.bola.y);
        DesenhaObjeto(fase.bola.desenho);


        for(int i=0; i<fase.alvos.size(); i++){
            MoveObjeto(fase.alvos[i].desenho, fase.alvos[i].x, fase.alvos[i].y);
            DesenhaObjeto(fase.alvos[i].desenho);
        }


        //DesenhaSprite(seta);
        //PlayAudio(somImpacto);

        //o frame totalmente pronto será mostrado na tela
        EncerraDesenho();
    }

    //o jogo será encerrado
    printf("fim");
    free(bolas);
    free(alvos);

    FinalizaJogo();

    return 0;
}


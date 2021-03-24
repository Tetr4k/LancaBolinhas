#include "PIG.h"

PIG_Evento evento;          //evento ser tratadoi a cada pssada do loop principal
PIG_Teclado meuTeclado;     //variável como mapeamento do teclado

struct Objeto{
    int x, y, desenho;
};

struct Fase{
    int angulo;
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

    //cria modelo dos alvos
    int *alvos = (int*) malloc(20*sizeof(int));
    for(int i=0; i<20; i++){
        alvos[i] = CriaObjeto("..//img//alvo.png");
        SetDimensoesObjeto(alvos[i], i*5+20, i*5+20);
    }

    //cria modelo das bolas
    int *bolas = (int*) malloc(3*sizeof(int));
    bolas[0] = CriaObjeto("..//img//bola1.png");
    bolas[1] = CriaObjeto("..//img//bola2.png");
    bolas[2] = CriaObjeto("..//img//bola3.png");
    SetDimensoesObjeto(bolas[0], 48, 48);
    SetDimensoesObjeto(bolas[1], 40, 40);
    SetDimensoesObjeto(bolas[2], 40, 40);
    CarregaArquivoFramesObjeto(bolas[0], "..//img//bola1.txt");
    CarregaArquivoFramesObjeto(bolas[1], "..//img//bola2.txt");
    CarregaArquivoFramesObjeto(bolas[2], "..//img//bola3.txt");
    MudaFrameObjeto(bolas[0], 1);
    MudaFrameObjeto(bolas[1], 1);
    MudaFrameObjeto(bolas[2], 1);


    /*
    int seta = CriaSprite("..//img//seta.png");
    MoveSprite(seta,0,0);
    SetDimensoesSprite(seta, 626, 484);
    SetAnguloSprite(seta,15);
    */

    struct Fase fase;
    fase.angulo = 0;
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

    PlayBackground();

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

        //fundo
        DesenhaSpriteSimples("..//img//fundo.png", 0, 0, 0);

        //interface
        DesenhaSpriteSimples("..//img//interface.png", 0, 0, 0);

        //posicao atual do objeto
        MoveObjeto(fase.bola.desenho, fase.bola.x, fase.bola.y);
        DesenhaObjeto(fase.bola.desenho);


        for(int i=0; i<fase.alvos.size(); i++){
            MoveObjeto(fase.alvos[i].desenho, fase.alvos[i].y, fase.alvos[i].x);
            DesenhaObjeto(fase.alvos[i].desenho);
        }

        //DesenhaSprite(seta);

        //DesenhaLinhasSequencia(20, 20, 400, 400, VERDE);
        //PlayAudio(somImpacto);

        //o frame totalmente pronto será mostrado na tela
        EncerraDesenho();
    }

    //o jogo será encerrado
    free(bolas);
    free(alvos);

    FinalizaJogo();

    return 0;
}


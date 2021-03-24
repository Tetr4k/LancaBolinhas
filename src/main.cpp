#include "PIG.h"

PIG_Evento evento;          //evento ser tratadoi a cada pssada do loop principal
PIG_Teclado meuTeclado;     //variável como mapeamento do teclado

int main( int argc, char* args[] ){

    //criando o jogo (aplicação)
    CriaJogo("Lanca Bolinhas");
    SetTituloJanela("Carregando . . .");
    SetTamanhoJanela(720, 1280);

    //associando o teclado (basta uma única vez) com a variável meuTeclado
    meuTeclado = GetTeclado();


    CarregaBackground("..//sounds//background.mp3");

    int somImpacto = CriaAudio("..//sounds//soundImpacto.mp3",0);
    SetVolume(somImpacto,25);

    int bola1 = CriaSprite("..//img//bola1.png");
    CarregaArquivoFramesSprite(bola1,"..//img//bola1.txt");
    MoveSprite(bola1,0,0);
    SetDimensoesSprite(bola1,48,48);
    MudaFrameSprite(bola1,1);

    /*
    int bola2 = CriaSprite(".//img//bola2.png");
    CarregaArquivoFramesSprite(bola2,"bola2.txt");
    MudaFrameSprite(bola2,1);

    int bola3 = CriaSprite(".//img//bola3.png");
    CarregaArquivoFramesSprite(bola3,"bola3.txt");
    MudaFrameSprite(bola3,1);
    */


    /*int seta = CriaSprite("..//img//seta.png");
    MoveSprite(seta,0,0);
    SetDimensoesSprite(seta, 626, 484);
    SetAnguloSprite(seta,15);*/

    //loop principal do jogo

    PlayBackground();

    SetTituloJanela("Lanca Bolinhas");

    while(JogoRodando()){

        //pega um evento que tenha ocorrido desde a última passada do loop
        evento = GetEvento();

        //aqui o evento deve ser tratado e as coisas devem ser atualizadas

        //será feita aprepação do frame que será exibido na tela
        IniciaDesenho();

        //todas as chamadas de desenho devem ser feitas aqui na ordem desejada
        DesenhaSprite(bola1);
        //DesenhaSprite(seta);
        //PlayAudio(somImpacto);

        //o frame totalmente pronto será mostrado na tela
        EncerraDesenho();
    }

    //o jogo será encerrado
    FinalizaJogo();

    return 0;
}


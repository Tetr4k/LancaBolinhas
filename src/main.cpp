#include "PIG.h"

PIG_Evento evento;          //evento ser tratadoi a cada pssada do loop principal
PIG_Teclado meuTeclado;     //variável como mapeamento do teclado

struct Objeto{
    double x, y, xi, yi, g;
    int desenho;
};

struct Fase{
    int angulo;
    struct Objeto bola;
    vector<int> alvos;
};

int main( int argc, char* args[] ){

    //criando o jogo (aplicação)
    CriaJogo("Lanca Bolinhas");
    SetTituloJanela("Carregando . . .");
    SetTamanhoJanela(720, 1280);

    //associando o teclado (basta uma única vez) com a variável meuTeclado
    meuTeclado = GetTeclado();


    CarregaBackground("..//sounds//background.mp3");//cria musica de fundo

    int somImpacto = CriaAudio("..//sounds//soundImpacto.mp3",0);//cria audio de impacto
    SetVolume(somImpacto, 1);//isso funciona?

    int *alvos = (int*) malloc(20*sizeof(int));//cria modelo dos alvos
    for(int i=0; i<20; i++){
        alvos[i] = CriaObjeto("..//img//alvo.png");
        int tam = i*5+20;
        SetDimensoesObjeto(alvos[i], tam, tam);
        SetPivoAbsolutoObjeto(alvos[i], tam/2, tam/2);
        DefineRaioColisaoObjeto(alvos[i], tam/4);
        DefineTipoColisaoObjeto(alvos[i], PIG_COLISAO_CIRCULAR);
    }

    int *bolas = (int*) malloc(3*sizeof(int));//cria modelo das bolas
    bolas[0] = CriaObjeto("..//img//bola1.png");
    bolas[1] = CriaObjeto("..//img//bola2.png");
    bolas[2] = CriaObjeto("..//img//bola3.png");
    SetDimensoesObjeto(bolas[0], 48, 48);
    SetDimensoesObjeto(bolas[1], 40, 40);
    SetDimensoesObjeto(bolas[2], 40, 40);
    SetPivoAbsolutoObjeto(bolas[0], 24, 24);
    SetPivoAbsolutoObjeto(bolas[1], 20, 20);
    SetPivoAbsolutoObjeto(bolas[2], 20, 20);
    DefineRaioColisaoObjeto(bolas[0], 24);
    DefineRaioColisaoObjeto(bolas[1], 20);
    DefineRaioColisaoObjeto(bolas[2], 20);
    DefineTipoColisaoObjeto(bolas[0], PIG_COLISAO_CIRCULAR);
    DefineTipoColisaoObjeto(bolas[1], PIG_COLISAO_CIRCULAR);
    DefineTipoColisaoObjeto(bolas[2], PIG_COLISAO_CIRCULAR);
    CarregaArquivoFramesObjeto(bolas[0], "..//img//bola1.txt");
    CarregaArquivoFramesObjeto(bolas[1], "..//img//bola2.txt");
    CarregaArquivoFramesObjeto(bolas[2], "..//img//bola3.txt");
    MudaFrameObjeto(bolas[0], 1);
    MudaFrameObjeto(bolas[1], 1);
    MudaFrameObjeto(bolas[2], 1);

    int seta = CriaSprite("..//img//seta.png", 0);//cria seta
    SetDimensoesSprite(seta, 60, 40);
    SetPivoAbsolutoSprite(seta, 30, 0);

    struct Fase fase;//leitura da fase
    int posX, posY, tipo;
    double yInicial;
    FILE *arq = fopen("../fases/fase1.txt", "r");
    fscanf(arq, "%d %d", &posY, &tipo);
    fase.bola.desenho = CriaObjeto(bolas[tipo]);
    fase.bola.g = 50*(tipo+1);
    fase.bola.x = fase.bola.xi = 10;
    fase.bola.y = fase.bola.yi = posY+100;
    fase.angulo = 0;
    while(!feof(arq)){
        fscanf(arq, "%d %d %d", &posX, &posY, &tipo);
        int alvo = CriaObjeto(alvos[tipo]);
        MoveObjeto(alvo, posY, posX);
        fase.alvos.push_back(alvo);
    }
    fclose(arq);

    MoveSprite(seta, fase.bola.x, fase.bola.y);
    SetPivoAbsolutoSprite(seta, fase.bola.x, fase.bola.y);
    //loop principal do jogo

    //PlayBackground(); <<<<<<<<<<<<<<<<<AD<AD<AD<AD<A<DA<D<AD<AD<AD-----------------------------------------------------

    SetTituloJanela("Lanca Bolinhas");

    int timer = CriaTimer(1);
    double t;

    while(JogoRodando()){

        //pega um evento que tenha ocorrido desde a última passada do loop
        evento = GetEvento();

        //aqui o evento deve ser tratado e as coisas devem ser atualizadas

        for(int i=0; i<fase.alvos.size(); i++) if(TestaColisaoObjetos(fase.bola.desenho, fase.alvos[i])){//teste de colisão
            PlayAudio(somImpacto);
            //quebra e fim de fase;
        }

        if(evento.tipoEvento == PIG_EVENTO_TECLADO){
            if(evento.teclado.tecla == PIG_TECLA_BARRAESPACO) DespausaTimer(timer);//verifica se espaço foi apertado para arremessar bolinha

            if(evento.teclado.acao == PIG_TECLA_PRESSIONADA){//ajusta angulo com setinhas
                if(evento.teclado.tecla == PIG_TECLA_CIMA)
                    if(fase.angulo<90) fase.angulo++;
                if(evento.teclado.tecla == PIG_TECLA_BAIXO)
                    if(fase.angulo>-90) fase.angulo--;
            }
        }

        t=TempoDecorrido(timer);
        fase.bola.y = fase.bola.yi + 250*sin(fase.angulo*M_PI/180.0)*t-fase.bola.g*pow(t, 2)/2;//calcula y
        fase.bola.x = fase.bola.xi + 250*cos(fase.angulo*M_PI/180.0)*t;//calcula x

        MoveObjeto(fase.bola.desenho, fase.bola.x, fase.bola.y);//atualiza posição do objeto

        if(fase.bola.y<0) break;//testa altura da bolinha

        //será feita aprepação do frame que será exibido na tela
        IniciaDesenho();

        //todas as chamadas de desenho devem ser feitas aqui na ordem desejada

        DesenhaSpriteSimples("..//img//fundo.png", 0, 0, 0);//fundo
        DesenhaSpriteSimples("..//img//interface.png", 0, 0, 0);//interface
        DesenhaObjeto(fase.bola.desenho);//bola
        for(int i=0; i<fase.alvos.size(); i++) DesenhaObjeto(fase.alvos[i]); //alvos


        SetAnguloSprite(seta, fase.angulo);//ajusta angulo da seta <----Não funciona
        DesenhaSprite(seta);//desenha a seta

        //DesenhaLinhasSequencia(20, 20, 400, 400, VERDE);

        EscreveInteiroCentralizado(fase.angulo, 640, 0, AZUL);//escreve o angulo na tela

        //o frame totalmente pronto será mostrado na tela
        EncerraDesenho();
    }
    //o jogo será encerrado
    FinalizaJogo();
    free(bolas);
    free(alvos);
    return 0;
}


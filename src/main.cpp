#include "PIG.h"

PIG_Evento evento;          //evento ser tratadoi a cada pssada do loop principal
PIG_Teclado meuTeclado;     //variável como mapeamento do teclado

#define COEFICIENTE_PESO 100
#define VELOCIDADE 500
#define D_ESQUERDA 20
#define D_BAIXO 100

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
    SetVolume(somImpacto, 1);

    int *alvos = (int*) malloc(20*sizeof(int));//cria modelo dos alvos
    for(int i=0; i<20; i++){
        alvos[i] = CriaObjeto("..//img//alvo.png", 0);
        int tam = i*5+20;
        SetDimensoesObjeto(alvos[i], tam, tam);
        SetPivoAbsolutoObjeto(alvos[i], tam/2, tam/2);
        DefineRaioColisaoObjeto(alvos[i], tam/4);
        DefineTipoColisaoObjeto(alvos[i], PIG_COLISAO_CIRCULAR);
    }

    int *bolas = (int*) malloc(3*sizeof(int));//cria modelo das bolas
    bolas[0] = CriaAnimacao("..//img//bola1.png", 0);
    bolas[1] = CriaAnimacao("..//img//bola2.png", 0);
    bolas[2] = CriaAnimacao("..//img//bola3.png", 0);
    SetDimensoesAnimacao(bolas[0], 48, 48);
    SetDimensoesAnimacao(bolas[1], 40, 40);
    SetDimensoesAnimacao(bolas[2], 40, 40);
    SetPivoAbsolutoAnimacao(bolas[0], 24, 24);
    SetPivoAbsolutoAnimacao(bolas[1], 20, 20);
    SetPivoAbsolutoAnimacao(bolas[2], 20, 20);
    DefineRaioColisaoAnimacao(bolas[0], 24);
    DefineRaioColisaoAnimacao(bolas[1], 20);
    DefineRaioColisaoAnimacao(bolas[2], 20);
    DefineTipoColisaoAnimacao(bolas[0], PIG_COLISAO_CIRCULAR);
    DefineTipoColisaoAnimacao(bolas[1], PIG_COLISAO_CIRCULAR);
    DefineTipoColisaoAnimacao(bolas[2], PIG_COLISAO_CIRCULAR);
    CarregaFramesPorLinhaAnimacao(bolas[0], 1, 1, 10);
    CarregaFramesPorLinhaAnimacao(bolas[1], 1, 1, 10);
    CarregaFramesPorLinhaAnimacao(bolas[2], 1, 1, 10);

    CriaModoAnimacao(bolas[0], 0, 1);//animação bola parada
    CriaModoAnimacao(bolas[1], 0, 1);
    CriaModoAnimacao(bolas[2], 0, 1);
    InsereFrameAnimacao(bolas[0], 0, 1, 0.05);
    InsereFrameAnimacao(bolas[1], 0, 1, 0.05);
    InsereFrameAnimacao(bolas[2], 0, 1, 0.05);
    MudaModoAnimacao(bolas[0], 0, 0);
    MudaModoAnimacao(bolas[1], 0, 0);
    MudaModoAnimacao(bolas[2], 0, 0);

    CriaModoAnimacao(bolas[0], 1, 1);//animação bola em movimento
    CriaModoAnimacao(bolas[1], 1, 1);
    CriaModoAnimacao(bolas[2], 1, 1);
    for(int i=0; i<10; i++){
        InsereFrameAnimacao(bolas[0], 1, i+1, 0.05);
        InsereFrameAnimacao(bolas[1], 1, i+1, 0.05);
        InsereFrameAnimacao(bolas[2], 1, i+1, 0.05);
    }

    int seta = CriaSprite("..//img//seta.png", 0);//cria seta
    SetDimensoesSprite(seta, 40, 120);
    SetPivoAbsolutoSprite(seta, 20, 0);

    struct Fase fase;//leitura da fase
    int posX, posY, tipo;
    double yInicial;
    FILE *arq = fopen("../fases/fase1.txt", "r");
    fscanf(arq, "%d %d", &posY, &tipo);
    fase.bola.desenho = CriaAnimacao(bolas[tipo], 0);
    fase.bola.g = COEFICIENTE_PESO*(tipo+1);
    fase.bola.x = fase.bola.xi = D_ESQUERDA;
    fase.bola.y = fase.bola.yi = posY+D_BAIXO;
    fase.angulo = 0;
    while(!feof(arq)){
        fscanf(arq, "%d %d %d", &posX, &posY, &tipo);
        int alvo = CriaObjeto(alvos[tipo], 0);
        MoveObjeto(alvo, posY, posX);
        fase.alvos.push_back(alvo);
    }
    fclose(arq);

    MoveSprite(seta, fase.bola.x, fase.bola.y);
    SetPivoAbsolutoSprite(seta, fase.bola.x, fase.bola.y);

    //loop principal do jogo

    PlayBackground();

    SetTituloJanela("Lanca Bolinhas");

    int timer = CriaTimer(1);
    double t;

    while(JogoRodando()){

        //pega um evento que tenha ocorrido desde a última passada do loop
        evento = GetEvento();

        //aqui o evento deve ser tratado e as coisas devem ser atualizadas

        for(int i=0; i<fase.alvos.size(); i++) if(TestaColisaoAnimacaoObjeto(fase.bola.desenho, fase.alvos[i])){//teste de colisão
            PlayAudio(somImpacto);
            //quebra e fim de fase;
        }

        if(evento.tipoEvento == PIG_EVENTO_TECLADO){
            if(evento.teclado.tecla == PIG_TECLA_BARRAESPACO){//verifica se espaço foi apertado para arremessar bolinha
                    DespausaTimer(timer);
                    MudaModoAnimacao(fase.bola.desenho, 1, 0);
            }

            if(evento.teclado.acao == PIG_TECLA_PRESSIONADA){//ajusta angulo usando as setas
                if(evento.teclado.tecla == PIG_TECLA_CIMA && fase.angulo<89) fase.angulo++;
                if(evento.teclado.tecla == PIG_TECLA_BAIXO && fase.angulo>-89) fase.angulo--;
            }
        }

        t=TempoDecorrido(timer);
        fase.bola.y = fase.bola.yi + VELOCIDADE*sin(fase.angulo*M_PI/180.0)*t-fase.bola.g*pow(t, 2)/2;//calcula y
        fase.bola.x = fase.bola.xi + VELOCIDADE*cos(fase.angulo*M_PI/180.0)*t;//calcula x

        MoveAnimacao(fase.bola.desenho, fase.bola.x, fase.bola.y);//atualiza posição do objeto

        if(fase.bola.y<0) break;//testa altura da bolinha

        //será feita aprepação do frame que será exibido na tela
        IniciaDesenho();

        //todas as chamadas de desenho devem ser feitas aqui na ordem desejada

        DesenhaSpriteSimples("..//img//fundo.png", 0, 0, 0);//fundo

        SetAnguloSprite(seta, fase.angulo);//ajusta angulo da seta <---- Não funciona
        DesenhaSprite(seta);//desenha a seta
        //DesenhaLinhasSequencia(20, 20, 400, 400, VERDE);

        for(int i=0; i<fase.alvos.size(); i++) DesenhaObjeto(fase.alvos[i]);//alvos
        DesenhaAnimacao(fase.bola.desenho);//bola

        DesenhaSpriteSimples("..//img//interface.png", 0, 0, 0);//interface
        EscreveInteiroCentralizado(fase.angulo, 640, 0, AZUL);//escreve o angulo na tela

        //Indicar que é angulo
        //Indicar que deve usar as setinhas para aumentar e diminuir
        //Indicar que deve usar espaço para lançar bola
        //Fixar camera na bola?
        //Passar de fase
        //Destruir alvo e gerar particulas

        //o frame totalmente pronto será mostrado na tela
        EncerraDesenho();
    }
    //o jogo será encerrado
    FinalizaJogo();
    free(bolas);
    free(alvos);
    return 0;
}


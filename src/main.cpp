#include "PIG.h"

PIG_Evento evento;          //evento ser tratadoi a cada pssada do loop principal
PIG_Teclado meuTeclado;     //vari�vel como mapeamento do teclado

#define C_PESO 100
#define VELOCIDADE 500
#define D_ESQUERDA 20
#define D_BAIXO 100

struct Objeto{
    double x, y, xi, yi, g;
    int desenho;
};

struct Trajeto{
    int x[32], y[32];
};

typedef struct{
    double angulo;
    Trajeto trajeto;
    struct Objeto bola;
    vector<int> alvos;
}Fase;

int *criaModeloAlvos(){//funcao para criar modelo dos alvos
    int *alvos = (int*) malloc(20*sizeof(int));
    for(int i=0; i<20; i++){
        alvos[i] = CriaObjeto("..//img//alvo.png", 0);
        int tam = i*5+20;
        SetDimensoesObjeto(alvos[i], tam, tam);
        SetPivoAbsolutoObjeto(alvos[i], tam/2, tam/2);
        DefineRaioColisaoObjeto(alvos[i], tam/4);
        DefineTipoColisaoObjeto(alvos[i], PIG_COLISAO_CIRCULAR);
    }
    return alvos;
}

int *criaModeloBolas(){//funcao para criar modelo das bolas
    int *bolas = (int*) malloc(3*sizeof(int));
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

    CriaModoAnimacao(bolas[0], 0, 1);//anima��o bola parada
    CriaModoAnimacao(bolas[1], 0, 1);
    CriaModoAnimacao(bolas[2], 0, 1);

    InsereFrameAnimacao(bolas[0], 0, 1, 0.05);
    InsereFrameAnimacao(bolas[1], 0, 1, 0.05);
    InsereFrameAnimacao(bolas[2], 0, 1, 0.05);

    MudaModoAnimacao(bolas[0], 0, 0);
    MudaModoAnimacao(bolas[1], 0, 0);
    MudaModoAnimacao(bolas[2], 0, 0);

    CriaModoAnimacao(bolas[0], 1, 1);//anima��o bola em movimento
    CriaModoAnimacao(bolas[1], 1, 1);
    CriaModoAnimacao(bolas[2], 1, 1);

    for(int i=0; i<10; i++){
        InsereFrameAnimacao(bolas[0], 1, i+1, 0.05);
        InsereFrameAnimacao(bolas[1], 1, i+1, 0.05);
        InsereFrameAnimacao(bolas[2], 1, i+1, 0.05);
    }

    return bolas;
}

int calculaX(Fase fase, double t){
    double radiano = fase.angulo*M_PI/180.0;
    return fase.bola.xi + VELOCIDADE*cos(radiano)*t;
}

int calculaY(Fase fase, double t){
    double radiano = fase.angulo*M_PI/180.0;
    return fase.bola.yi + VELOCIDADE*sin(radiano)*t-fase.bola.g*pow(t, 2)/2;
}

Trajeto calculaTrajeto(Fase fase){
    Trajeto aux;
    int alt, lag;
    GetDimensoesAnimacao(fase.bola.desenho, &alt, &lag);//pega dimens�o da bola para ajustar trajeto
    for(int i=0; i<32; i++){
        double radiano = fase.angulo*M_PI/180.0, t;
        aux.x[i] = 40*i+D_ESQUERDA;
        t = (aux.x[i]-fase.bola.xi)/(VELOCIDADE*cos(radiano));
        aux.x[i] +=lag/2;//ajusta x do trajeto
        aux.y[i] = calculaY(fase, t)+(alt/2);//calcula e ajusta y do trajeto
    }
    return aux;
}

int main( int argc, char* args[] ){

    //criando o jogo (aplica��o)
    CriaJogo("Lanca Bolinhas");
    SetTituloJanela("Carregando . . .");
    SetTamanhoJanela(720, 1280);

    //associando o teclado (basta uma �nica vez) com a vari�vel meuTeclado
    meuTeclado = GetTeclado();

    CarregaBackground("..//sounds//background.mp3");//cria musica de fundo

    int somImpacto = CriaAudio("..//sounds//soundImpacto.mp3",0);//cria audio de impacto
    SetVolume(somImpacto, 50);

    int *alvos = criaModeloAlvos();
    int *bolas = criaModeloBolas();

    Fase fase;//leitura da fase
    double yInicial;
    FILE *arq = fopen("../fases/fase1.txt", "r");
    int posX, posY, tipo;
    fscanf(arq, "%d %d", &posY, &tipo);
    fase.bola.desenho = CriaAnimacao(bolas[tipo], 0);
    fase.bola.g  = C_PESO*(tipo+1);
    fase.bola.xi = D_ESQUERDA;
    fase.bola.yi = posY+D_BAIXO;
    fase.bola.x  = fase.bola.xi;
    fase.bola.y  = fase.bola.yi;
    fase.angulo  = 30;
    while(!feof(arq)){
        fscanf(arq, "%d %d %d", &posX, &posY, &tipo);
        int alvo = CriaObjeto(alvos[tipo], 0);
        MoveObjeto(alvo, posY, posX);
        fase.alvos.push_back(alvo);
    }
    fclose(arq);

    GetDimensoesAnimacao(fase.bola.desenho, &posX, &posY);
    int seta = CriaSprite("..//img//seta.png", 0);//cria seta
    SetDimensoesSprite(seta, 20, 100);
    SetPivoAbsolutoSprite(seta, 10, 0);
    MoveSprite(seta, fase.bola.xi+posX/2-10, fase.bola.yi+posY/2);

    //loop principal do jogo

    PlayBackground();

    SetTituloJanela("Lanca Bolinhas");

    int timer = CriaTimer(1);
    double t=0;

    fase.trajeto = calculaTrajeto(fase);

    while(JogoRodando()){

        //pega um evento que tenha ocorrido desde a �ltima passada do loop
        evento = GetEvento();

        //aqui o evento deve ser tratado e as coisas devem ser atualizadas

        for(int i=0; i<fase.alvos.size(); i++) if(TestaColisaoAnimacaoObjeto(fase.bola.desenho, fase.alvos[i])){//teste de colis�o
            DestroiObjeto(fase.alvos[i]);
            fase.alvos.erase (fase.alvos.begin()+i);
            PlayAudio(somImpacto);
            //quebra e fim de fase;
        }

        if(evento.tipoEvento == PIG_EVENTO_TECLADO){
            if(evento.teclado.tecla == PIG_TECLA_BARRAESPACO){//verifica se espa�o foi apertado para arremessar bolinha
                    DespausaTimer(timer);
                    MudaModoAnimacao(fase.bola.desenho, 1, 0);
            }

            if(evento.teclado.acao == PIG_TECLA_PRESSIONADA && t==0){//ajusta angulo usando as setas
                if(evento.teclado.tecla == PIG_TECLA_CIMA && fase.angulo<89){
                    fase.angulo+=0.2;
                    fase.trajeto = calculaTrajeto(fase);
                }
                if(evento.teclado.tecla == PIG_TECLA_BAIXO && fase.angulo>-89){
                    fase.angulo-=0.2;
                    fase.trajeto = calculaTrajeto(fase);
                }
            }
        }


        if(fase.bola.y<D_BAIXO/2) {
            break;//testa altura da bolinha
        }
        else{
            t=TempoDecorrido(timer);
            fase.bola.y = calculaY(fase, t);////calcula y
            fase.bola.x = calculaX(fase, t);////calcula x
            MoveAnimacao(fase.bola.desenho, fase.bola.x, fase.bola.y);//atualiza posi��o do objeto
        }


        //ser� feita aprepa��o do frame que ser� exibido na tela
        IniciaDesenho();

        //todas as chamadas de desenho devem ser feitas aqui na ordem desejada

        DesenhaSpriteSimples("..//img//fundo.png", 0, 0, 0);//fundo

        if(t==0){
            DesenhaSprite(seta);//desenha a seta
            SetAnguloSprite(seta, fase.angulo);//ajusta angulo da seta
        };

        for(int i=0; i<fase.alvos.size(); i++) DesenhaObjeto(fase.alvos[i]);//desenha alvos
        DesenhaLinhasSequencia(fase.trajeto.x, fase.trajeto.y, 32, AZUL);//desenha trajeto
        DesenhaAnimacao(fase.bola.desenho);//desenha bola
        DesenhaSpriteSimples("..//img//interface.png", 0, 0, 0);//desenha fundo da interface
        EscreveDoubleCentralizado(fase.angulo, 1, 640, 0, AZUL);//escreve o angulo na interface

        //Indicar que � angulo
        //Indicar que deve usar as setinhas para aumentar e diminuir
        //Indicar que deve usar espa�o para lan�ar bola
        //Fixar camera na bola?
        //Passar de fase
        //Destruir alvo e gerar particulas

        //o frame totalmente pronto ser� mostrado na tela
        EncerraDesenho();
    }
    //o jogo ser� encerrado
    FinalizaJogo();
    free(bolas);
    free(alvos);
    return 0;
}

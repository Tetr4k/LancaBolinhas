#include "PIG.h"
#include <stack>

PIG_Evento evento;          //evento ser tratadoi a cada pssada do loop principal
PIG_Teclado meuTeclado;     //variável como mapeamento do teclado

#define T_LARGURA 1280
#define T_ALTURA 720
#define C_PESO 180
#define C_TAM_ALVO 5
#define C_ANGULO 16
#define D_BORDA 32
#define D_BAIXO 100
#define VELOCIDADE 640

int seta;
vector<int> bolas;
vector<int> alvos;

struct Bola{
    //x atual, y atual, x inicial, y inicial, gravidade sob a bola
    double x, y, xi, yi, g;
    //aparencia da bola
    int desenho;
};

struct Trajeto{
    //coordenadas dos vertices entre as linhas que formam a parabola do trajeto
    int x[32], y[32];
};

typedef struct Fase{
    //bola da fase
    struct Bola bola;
    //alvos da fase
    vector<int> alvos;
    //angulo da fase
    double angulo;
    //seta na fase
    int seta;
    //trajeto na fase
    Trajeto trajeto;
    //tempo na fase
    int timer;
    double tempo;
}Fase;

vector<int> criaModeloAlvos(){//funcao para criar modelo dos alvos
    int alvo, tam;
    vector<int> vetorAlvos;
    //carrega sprite do alvo e cria variações
    for(int i=0; i<20; i++){
        tam = i*C_TAM_ALVO+20;
        alvo = CriaObjeto(".//img//alvo.png", 0);
        SetDimensoesObjeto(alvo, tam, tam);
        SetPivoAbsolutoObjeto(alvo, tam/2, tam/2);
        DefineRaioColisaoObjeto(alvo, tam/4);
        DefineTipoColisaoObjeto(alvo, PIG_COLISAO_CIRCULAR);
        vetorAlvos.push_back(alvo);
    }
    return vetorAlvos;
}

vector<int> criaModeloBolas(){//funcao para criar modelo das bolas
    vector<int> vetorBolas;
    //carrega folhas de sprites das bolas
    int bola1 = CriaAnimacao(".//img//bola1.png", 0);
    int bola2 = CriaAnimacao(".//img//bola2.png", 0);
    int bola3 = CriaAnimacao(".//img//bola3.png", 0);
    //define a fisica da bola
    SetDimensoesAnimacao(bola1, 48, 48);
    SetDimensoesAnimacao(bola2, 40, 40);
    SetDimensoesAnimacao(bola3, 40, 40);
    SetPivoAbsolutoAnimacao(bola1, 24, 24);
    SetPivoAbsolutoAnimacao(bola2, 20, 20);
    SetPivoAbsolutoAnimacao(bola3, 20, 20);
    DefineRaioColisaoAnimacao(bola1, 24);
    DefineRaioColisaoAnimacao(bola2, 20);
    DefineRaioColisaoAnimacao(bola3, 20);
    DefineTipoColisaoAnimacao(bola1, PIG_COLISAO_CIRCULAR);
    DefineTipoColisaoAnimacao(bola2, PIG_COLISAO_CIRCULAR);
    DefineTipoColisaoAnimacao(bola3, PIG_COLISAO_CIRCULAR);
    //carrega animação da bola
    CarregaFramesPorLinhaAnimacao(bola1, 1, 1, 10);
    CarregaFramesPorLinhaAnimacao(bola2, 1, 1, 10);
    CarregaFramesPorLinhaAnimacao(bola3, 1, 1, 10);
    //define animação da bola parada
    CriaModoAnimacao(bola1, 0, 1);
    CriaModoAnimacao(bola2, 0, 1);
    CriaModoAnimacao(bola3, 0, 1);
    InsereFrameAnimacao(bola1, 0, 1, 0.05);
    InsereFrameAnimacao(bola2, 0, 1, 0.05);
    InsereFrameAnimacao(bola3, 0, 1, 0.05);
    MudaModoAnimacao(bola1, 0, 0);
    MudaModoAnimacao(bola2, 0, 0);
    MudaModoAnimacao(bola3, 0, 0);
    //define animação da bola em movimento
    CriaModoAnimacao(bola1, 1, 1);
    CriaModoAnimacao(bola2, 1, 1);
    CriaModoAnimacao(bola3, 1, 1);
    for(int i=0; i<10; i++){
        InsereFrameAnimacao(bola1, 1, i+1, 0.05);
        InsereFrameAnimacao(bola2, 1, i+1, 0.05);
        InsereFrameAnimacao(bola3, 1, i+1, 0.05);
    }
    //insere bolas no vetor
    vetorBolas.push_back(bola1);
    vetorBolas.push_back(bola2);
    vetorBolas.push_back(bola3);
    return vetorBolas;
}

int calculaX(Fase fase){
    //calcula x
    double radiano = fase.angulo*M_PI/180.0;
    return fase.bola.xi + VELOCIDADE*cos(radiano)*fase.tempo;
}

int calculaY(Fase fase){
    //calcula y
    double radiano = fase.angulo*M_PI/180.0;
    return fase.bola.yi + VELOCIDADE*sin(radiano)*fase.tempo-fase.bola.g*pow(fase.tempo, 2)/2;
}

Trajeto calculaTrajeto(Fase fase){//calcula trajeto da bola
    Trajeto aux;
    int alt, lag;
    //pega altura e largura da bola
    GetDimensoesAnimacao(fase.bola.desenho, &alt, &lag);
    for(int i=0; i<32; i++){
        double radiano = fase.angulo*M_PI/180.0, t;
        //calcula x do trajeto
        aux.x[i] = 40*i+D_BORDA;
        //calcula tempo a partir de x para calcular y
        t = (aux.x[i]-fase.bola.xi)/(VELOCIDADE*cos(radiano));
        //calcula y do trajeto e ajusta a partir da altura da bola
        aux.y[i] = fase.bola.yi + VELOCIDADE*sin(radiano)*t-fase.bola.g*pow(t, 2)/2+(alt/2);
        //ajusta x a partir da largura da bola;
        aux.x[i] +=lag/2;
    }
    return aux;
}

stack<string> preparaFases(){//le arquivo com o nome dos arquivos de fase
    //declara e abre o arquivo
    ifstream arq;
    arq.open("./fases/arquivosFases.txt", std::ifstream::in);
    //cria arquivo e pilha de arquivos
    string auxArquivo;
    stack<string> pilhaArquivos;
    //le arquivos de fases
    while(arq >> auxArquivo)pilhaArquivos.push(auxArquivo);
    //fecha arquivo
    arq.close();
    return pilhaArquivos;
}

Fase carregaFase(string arquivo){//cria um Fase a partir de um arquivo texto
    int posX, posY, tipo;
    //declara e abre o arquivo
    ifstream arq;
    arq.open(arquivo, std::ifstream::in);
    //lê posição e tipo da bola
    arq >> posY >> tipo;
    //cria a fase
    Fase auxFase;
    //inicia o angulo da fase
    auxFase.angulo = C_ANGULO*(tipo+1);
    //inicia o timer da fase
    auxFase.timer  = CriaTimer(1);
    auxFase.tempo  = 0;
    //cria bola da fase e inicia posição
    auxFase.bola.desenho = CriaAnimacao(bolas[tipo], 0);
    auxFase.bola.g  = C_PESO*(tipo+1);
    auxFase.bola.xi = D_BORDA;
    auxFase.bola.yi = posY+D_BAIXO+D_BORDA;
    auxFase.bola.x  = auxFase.bola.xi;
    auxFase.bola.y  = auxFase.bola.yi;
    //cria e posiciona uma seta na fase
    auxFase.seta = CriaSprite(seta, 0);
    GetDimensoesAnimacao(auxFase.bola.desenho, &posY, &posX);
    MoveSprite(auxFase.seta, auxFase.bola.xi+posX/2, auxFase.bola.yi+posY/2-10);
    SetAnguloSprite(auxFase.seta, auxFase.angulo);
    //cria o trajeto inicial da bolinha
    auxFase.trajeto = calculaTrajeto(auxFase);
    //cria todos os alvos de uma fase;
    while(arq >> posY >> posX >> tipo){
        int alvo = CriaObjeto(alvos[tipo], 0);
        MoveObjeto(alvo, posX, posY+D_BAIXO+D_BORDA);
        auxFase.alvos.push_back(alvo);
    }
    //fecha arquivo
    arq.close();
    return auxFase;
}

int main( int argc, char* args[] ){

    //criando o jogo (aplicação)
    CriaJogo("Lanca Bolinhas");
    SetTituloJanela("Lanca Bolinhas");
    SetTamanhoJanela(T_ALTURA, T_LARGURA);

    //associando o teclado (basta uma única vez) com a variável meuTeclado
    meuTeclado = GetTeclado();

    //cria musica de fundo
    CarregaBackground(".//sounds//background.mp3");

    //cria audio de impacto
    int somImpacto = CriaAudio(".//sounds//soundImpacto.mp3",0);
    SetVolume(somImpacto, 50);
    int somLancamento = CriaAudio(".//sounds//soundLancamento.mp3",0);
    SetVolume(somLancamento, 25);

    //cria modelos para bolas, alvos e setas
    alvos = criaModeloAlvos();
    bolas = criaModeloBolas();
    seta = CriaSprite(".//img//seta.png", 0);
    SetDimensoesSprite(seta, 20, 100);
    SetPivoAbsolutoSprite(seta, 0, 10);

    //carrega e inicia fases
    stack<string> fases = preparaFases();
    Fase fase = carregaFase(fases.top());

    //loop principal do jogo

    //toca musica de fundo
    PlayBackground();

    while(JogoRodando()){

        //pega um evento que tenha ocorrido desde a última passada do loop
        evento = GetEvento();

        //aqui o evento deve ser tratado e as coisas devem ser atualizadas

        //verifica posição da bola
        if(fase.bola.y<D_BAIXO/2) {
            //pausa efeitos sonoros
            StopAudio(somImpacto);//era pra funcionar?
            StopAudio(somLancamento);
        //verifica se a fase esta concluida
            if(fase.alvos.size()==0){
                //carrega nova fase
                fases.pop();
                fase = carregaFase(fases.top());
            }
            else fase = carregaFase(fases.top()); //reinicia fase
        }
        else{
            fase.tempo = TempoDecorrido(fase.timer);//calcula tempo
            fase.bola.y = calculaY(fase);////calcula y
            fase.bola.x = calculaX(fase);////calcula x
            MoveAnimacao(fase.bola.desenho, fase.bola.x, fase.bola.y);//atualiza posição do objeto
        }

        //verifica colisão da bola com cada alvo
        for(int i=0; i<fase.alvos.size(); i++) if(TestaColisaoAnimacaoObjeto(fase.bola.desenho, fase.alvos[i])){//teste de colisão
            //remove alvo
            fase.alvos.erase(fase.alvos.begin()+i);
            //gera particulas
            /*
                codigo para gerar particulas
            */
            PlayAudio(somImpacto);
        }
        //captura teclas apertadas
        if(evento.tipoEvento == PIG_EVENTO_TECLADO){
            if(evento.teclado.acao == PIG_TECLA_PRESSIONADA && fase.tempo==0){
                if(evento.teclado.tecla == PIG_TECLA_BARRAESPACO){//atira a bola usando espaço
                        DespausaTimer(fase.timer);
                        PlayAudio(somLancamento);
                        MudaModoAnimacao(fase.bola.desenho, 1, 0);
                }
                if(evento.teclado.tecla == PIG_TECLA_CIMA && fase.angulo<89.9){//aumenta angulo usando as seta para cima
                    fase.angulo+=0.2;
                    SetAnguloSprite(fase.seta, fase.angulo);//ajusta angulo da seta
                    fase.trajeto = calculaTrajeto(fase);//ajusta trajeto
                }
                if(evento.teclado.tecla == PIG_TECLA_BAIXO && fase.angulo>-89.9){//diminui angulo usando as seta para cima
                    fase.angulo-=0.2;
                    SetAnguloSprite(fase.seta, fase.angulo);//ajusta angulo da seta
                    fase.trajeto = calculaTrajeto(fase);//ajusta trajeto
                }
            }
        }

        //será feita a prepação do frame que será exibido na tela
        IniciaDesenho();

        //todas as chamadas de desenho devem ser feitas aqui na ordem desejada

        DesenhaSpriteSimples(".//img//fundo.png", 0, 0, 0);//fundo

        //desenha Fase
        for(std::vector<int>::iterator i=fase.alvos.begin(); i!=fase.alvos.end(); ++i) DesenhaObjeto(*i);//desenha alvos
        //verifica se a bola esta parada e desenha a seta e o trajeto da bola
        if(fase.tempo==0){
            DesenhaSprite(fase.seta);//desenha a seta
            DesenhaLinhasSequencia(fase.trajeto.x, fase.trajeto.y, 32, AZUL);//desenha trajeto
        };
        DesenhaAnimacao(fase.bola.desenho);//desenha bola
        //desenha interface
        DesenhaSpriteSimples(".//img//interface.png", 0, 0, 0);//desenha fundo da interface
        EscreveDoubleCentralizado(fase.angulo, 1, 640, 25, AZUL);//escreve o angulo na interface
        EscreveInteiroEsquerda(fase.alvos.size(), 0, 0, VERDE);
        EscreveDoubleDireita(GetFPS(), 1, 1280, 0, VERDE);

        //Indicar que é angulo
        //Indicar que deve usar as setinhas para aumentar e diminuir
        //Indicar que deve usar espaço para lançar bola
        //diminuir som de lançamento e botar fade out
        //Passar de fase
        //Destruir alvo e gerar particulas

        //o frame totalmente pronto será mostrado na tela
        EncerraDesenho();
    }
    //o jogo será encerrado
    FinalizaJogo();
    return 0;
}

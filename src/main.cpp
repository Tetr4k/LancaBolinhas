#include "PIG.h"
#include <stack>

PIG_Evento evento;          //evento ser tratadoi a cada pssada do loop principal
PIG_Teclado meuTeclado;     //vari�vel como mapeamento do teclado

#define T_LARGURA 1280 //LARGURA DA TELA
#define T_ALTURA 720   //ALTURA  DA TELA
#define D_BORDA 32     //Distancia da borda geral
#define D_BAIXO 100    //Distancia inferior
#define VELOCIDADE 640 //Velocidade das bolinhas
#define C_PESO 180     //Um coeficiente para o peso das bolinhas
#define C_TAM_ALVO 5   //Um coeficiente para o tamanho dos alvos
#define C_ANGULO 16    //Um coeficiente para o angulo inicial

int seta;
double tempo;
vector<int> bolas;
vector<int> alvos;

typedef struct Trajeto{
    int x[32], y[32];       //coordenadas dos vertices entre as linhas que formam a parabola do trajeto
}Trajeto;

typedef struct Bola{
    double x, y, xi, yi, g, angulo; //x atual, y atual, x inicial, y inicial, gravidade sob a bola e angulo da bola
    int desenho;                    //anima��o da bola
    Trajeto trajeto;                //trajeto da bola
}Bola;

typedef struct Fase{
    int seta, timer;        //seta da fase, cronometro da fase;
    Bola bola;              //bola da fase
    vector<int> alvos;      //alvos da fase
}Fase;

vector<int> criaModeloAlvos(){//funcao para criar modelo dos alvos
    int alvo, tam;
    vector<int> vetorAlvos;
    //carrega sprite do alvo e cria varia��es
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
    //carrega anima��o da bola
    CarregaFramesPorLinhaAnimacao(bola1, 1, 1, 10);
    CarregaFramesPorLinhaAnimacao(bola2, 1, 1, 10);
    CarregaFramesPorLinhaAnimacao(bola3, 1, 1, 10);
    //define anima��o da bola parada
    CriaModoAnimacao(bola1, 0, 1);
    CriaModoAnimacao(bola2, 0, 1);
    CriaModoAnimacao(bola3, 0, 1);
    InsereFrameAnimacao(bola1, 0, 1, 1);
    InsereFrameAnimacao(bola2, 0, 1, 1);
    InsereFrameAnimacao(bola3, 0, 1, 1);
    MudaModoAnimacao(bola1, 0, 0);
    MudaModoAnimacao(bola2, 0, 0);
    MudaModoAnimacao(bola3, 0, 0);
    //define anima��o da bola em movimento
    CriaModoAnimacao(bola1, 1, 1);
    CriaModoAnimacao(bola2, 1, 1);
    CriaModoAnimacao(bola3, 1, 1);
    for(int i=0; i<10; i++){
        InsereFrameAnimacao(bola1, 1, i+1, 0.04);
        InsereFrameAnimacao(bola2, 1, i+1, 0.06);
        InsereFrameAnimacao(bola3, 1, i+1, 0.08);
    }
    //insere bolas no vetor
    vetorBolas.push_back(bola1);
    vetorBolas.push_back(bola2);
    vetorBolas.push_back(bola3);
    return vetorBolas;
}

int calculaX(Bola bola){    //Função para calcular X
    double radiano = bola.angulo*M_PI/180.0;    //Converte angulo para radiano
    return bola.xi + VELOCIDADE*cos(radiano)*tempo;
}

int calculaY(Bola bola){    //Função para calcular Y
    double radiano = bola.angulo*M_PI/180.0;    //Converte angulo para radiano
    return bola.yi + VELOCIDADE*sin(radiano)*tempo-bola.g*pow(tempo, 2)/2;
}

Trajeto calculaTrajeto(Bola bola){//Função para calcular trajeto da bola
    Trajeto auxTrajeto;
    int alt, lag;
    GetDimensoesAnimacao(bola.desenho, &alt, &lag); //Pega altura e largura da bola
    for(int i=0; i<32; i++){
        double radiano = bola.angulo*M_PI/180.0, t;                                         //Converte angulo para radiano
        auxTrajeto.x[i] = 40*i+D_BORDA;                                                     //Calcula x do trajeto
        t = (auxTrajeto.x[i]-bola.xi)/(VELOCIDADE*cos(radiano));                            //Calcula tempo a partir de x para calcular y
        auxTrajeto.y[i] = bola.yi + VELOCIDADE*sin(radiano)*t-bola.g*pow(t, 2)/2+(alt/2);   //Calcula y do trajeto e ajusta a partir da altura da bola
        auxTrajeto.x[i] +=lag/2;                                                            //Ajusta x a partir da largura da bola;
    }
    return auxTrajeto;
}

stack<string> preparaFases(){//le arquivo com o nome dos arquivos de fase
    ifstream arq;
    arq.open("./fases/arquivosFases.txt", std::ifstream::in);
    //cria arquivo e pilha de arquivos
    string auxArquivo;
    stack<string> pilhaArquivos;
    while(arq >> auxArquivo) pilhaArquivos.push(auxArquivo); //le arquivos de fases
    arq.close();
    return pilhaArquivos;
}

Bola criaBola(int posY, int tipo){//Função para cria uma bola
    Bola auxBola;
    auxBola.desenho = CriaAnimacao(bolas[tipo], 0);     //Cria animação da bola
    auxBola.g       = C_PESO*(tipo+1);                  //Calcula gravidade na bola
    auxBola.angulo  = C_ANGULO*(tipo+1);                //Calcula angulo inicial da bola
    auxBola.xi      = D_BORDA;                          //Define posição inicial e atual da bola
    auxBola.x       = auxBola.xi;                       //***
    auxBola.yi      = posY+D_BAIXO+D_BORDA;             //***
    auxBola.y       = auxBola.yi;                       //***
    auxBola.trajeto = calculaTrajeto(auxBola);          //Calcula o trajeto inicial da bola
    SetAnguloAnimacao(auxBola.desenho, auxBola.angulo); //Ajusta angulo da animação //<<-----BUG GRAFICO?
    return auxBola;
}

Fase carregaFase(string arquivo){//cria um Fase a partir de um arquivo texto
    ifstream arq;
    arq.open(arquivo, std::ifstream::in);
    int posX, posY, tipo;
    arq >> posY >> tipo;                    //l� posi��o e tipo da bola
    Fase auxFase;                           //cria a fase
    tempo = 0;                              //reseta o tempo da fase
    auxFase.timer   = CriaTimer(1);         //inicia o timer da fase
    auxFase.bola    = criaBola(posY, tipo); //cria bola da fase
    auxFase.seta    = CriaSprite(seta, 0);  //cria seta da fase
    GetDimensoesAnimacao(auxFase.bola.desenho, &posY, &posX);
    MoveSprite(auxFase.seta, auxFase.bola.xi+posX/2, auxFase.bola.yi+posY/2-40);    //Posiciona seta
    SetAnguloSprite(auxFase.seta, auxFase.bola.angulo);                             //Ajusta angulo da seta
    //Cria os alvos da fase;
    while(arq >> posY >> posX >> tipo){
        int alvo = CriaObjeto(alvos[tipo], 0);
        MoveObjeto(alvo, posX, posY+D_BAIXO+D_BORDA);
        auxFase.alvos.push_back(alvo);
    }
    arq.close();
    return auxFase;
}


int main( int argc, char* args[] ){
    //criando o jogo (aplica��o)
    CriaJogo("Lanca Bolinhas");
    SetTituloJanela("Lanca Bolinhas");
    SetTamanhoJanela(T_ALTURA, T_LARGURA);
    //associando o teclado (basta uma �nica vez) com a vari�vel meuTeclado
    meuTeclado = GetTeclado();
    //cria fontes
    int fonteAngulo     = CriaFonteNormal(".//fontes//FredokaOne-Regular.ttf", 48, AZUL);
    int fonteAlvos      = CriaFonteNormal(".//fontes//FredokaOne-Regular.ttf", 32, VERMELHO);
    int fonteIndicacao  = CriaFonteNormal(".//fontes//FredokaOne-Regular.ttf", 16, AZUL);
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
    SetDimensoesSprite(seta, 80, 174);
    SetPivoAbsolutoSprite(seta, 0, 40);
    //Sprites de indicações
    int espaco = CriaSprite(".//img//espaco.png", 0);   //Indicação espaço
    SetDimensoesSprite(espaco, 25, 128);
    MoveSprite(espaco, 576, 70);
    int cima = CriaSprite(".//img//cima.png", 0);       //Indicação Cima
    SetDimensoesSprite(cima, 25, 25);
    MoveSprite(cima, 576, 40);
    int baixo = CriaSprite(".//img//baixo.png", 0);     //Indicação Baixo
    SetDimensoesSprite(baixo, 25, 25);
    MoveSprite(baixo, 576, 10);
    //carrega e inicia fases
    stack<string> fases = preparaFases();
    Fase fase = carregaFase(fases.top());
    //loop principal do jogo
    //toca musica de fundo
    PlayBackground();
    while(JogoRodando()){
        //pega um evento que tenha ocorrido desde a �ltima passada do loop
        evento = GetEvento();

        //aqui o evento deve ser tratado e as coisas devem ser atualizadas

        //captura teclas pressionadas

        if(evento.tipoEvento == PIG_EVENTO_TECLADO){
            if(evento.teclado.acao == PIG_TECLA_PRESSIONADA && tempo==0){
                if(evento.teclado.tecla == PIG_TECLA_BARRAESPACO){//atira a bola usando espaço
                    DespausaTimer(fase.timer);
                    PlayAudio(somLancamento);
                    MudaModoAnimacao(fase.bola.desenho, 1, 0);
                    SetColoracaoSprite(espaco, VERMELHO);   //ajusta coloração da tecla para espaço
                }
                if(evento.teclado.tecla == PIG_TECLA_CIMA && fase.bola.angulo<89.9){//aumenta angulo usando as seta para cima
                    fase.bola.angulo+=0.2;
                    SetAnguloAnimacao(fase.bola.desenho, fase.bola.angulo); //ajusta angulo da bola
                    SetAnguloSprite(fase.seta, fase.bola.angulo);           //ajusta angulo da seta
                    fase.bola.trajeto = calculaTrajeto(fase.bola);          //ajusta trajeto
                    SetColoracaoSprite(cima, VERMELHO);                     //ajusta coloração da tecla para cima
                }
                if(evento.teclado.tecla == PIG_TECLA_BAIXO && fase.bola.angulo>-89.9){//diminui angulo usando as seta para cima
                    fase.bola.angulo-=0.2;
                    SetAnguloAnimacao(fase.bola.desenho, fase.bola.angulo); //ajusta angulo da bola
                    SetAnguloSprite(fase.seta, fase.bola.angulo);           //ajusta angulo da seta
                    fase.bola.trajeto = calculaTrajeto(fase.bola);          //ajusta trajeto
                    SetColoracaoSprite(baixo, VERMELHO);                    //ajusta coloração da tecla para baixo
                }
            }
            else{
                SetColoracaoSprite(cima, BRANCO);   //retorna a coloração original
                SetColoracaoSprite(baixo, BRANCO);  //retorna a coloração original
            }
        }

        //verifica posi��o da bola
        if(fase.bola.y<D_BAIXO/2) {
            //pausa efeitos sonoros
            StopAudio(somImpacto);
            StopAudio(somLancamento);
            //verifica se a fase esta concluida
            if(fase.alvos.size()==0){
                //carrega nova fase
                fases.pop();
                fase = carregaFase(fases.top());
            }
            else fase = carregaFase(fases.top()); //reinicia fase

            SetColoracaoSprite(espaco, BRANCO); //retorna a coloração original
            Espera(250);
        }
        else{
            tempo = TempoDecorrido(fase.timer);//calcula tempo
            fase.bola.y = calculaY(fase.bola);////calcula y
            fase.bola.x = calculaX(fase.bola);////calcula x
            MoveAnimacao(fase.bola.desenho, fase.bola.x, fase.bola.y);//atualiza posi��o do objeto
        }

        for(std::vector<int>::iterator i=fase.alvos.begin(); i!=fase.alvos.end(); ++i) TrataAutomacaoObjeto(*i);
        TrataAutomacaoAnimacao(fase.bola.desenho);
        TrataAutomacaoSprite(fase.seta);

        //ser� feita a prepa��o do frame que ser� exibido na tela

        //verifica colis�o da bola com cada alvo
        for(int i=0; i<fase.alvos.size(); i++) if(TestaColisaoAnimacaoObjeto(fase.bola.desenho, fase.alvos[i])){//teste de colis�o
            //remove alvo
            fase.alvos.erase(fase.alvos.begin()+i);
            //gera particulas
            /*
                codigo para gerar particulas
            */
            PlayAudio(somImpacto);
        }

        IniciaDesenho();

        //todas as chamadas de desenho devem ser feitas aqui na ordem desejada

        DesenhaSpriteSimples(".//img//fundo.png", 0, 0, 0);//fundo

        //desenha Fase
        for(std::vector<int>::iterator i=fase.alvos.begin(); i!=fase.alvos.end(); ++i) DesenhaObjeto(*i);//desenha alvos
        //verifica se a bola esta parada e desenha a seta e o trajeto da bola
        if(tempo==0){
            DesenhaSprite(fase.seta);//desenha a seta
            DesenhaLinhasSequencia(fase.bola.trajeto.x, fase.bola.trajeto.y, 32, VERMELHO);//desenha trajeto
        };
        DesenhaAnimacao(fase.bola.desenho);//desenha bola
        //desenha interface
        DesenhaSpriteSimples(".//img//interface.png", 0, 0, 0);//desenha fundo da interface

        DesenhaSprite(espaco);
        EscreverCentralizada("Atirar", 640, 75, AZUL, fonteIndicacao);
        DesenhaSprite(cima);
        EscreverEsquerda(" Aumentar angulo", 600, 45, AZUL, fonteIndicacao);
        DesenhaSprite(baixo);
        EscreverEsquerda(" Diminuir angulo", 600, 15, AZUL, fonteIndicacao);

        EscreverEsquerda("Angulo: ", 8, 4, AZUL, fonteAngulo);
        EscreveDoubleDireita(fase.bola.angulo, 1, 304, 4, AZUL, fonteAngulo);//escreve o angulo na interface
        EscreverEsquerda(" graus", 304, 4, AZUL, fonteAngulo);

        //Escreve quantidade de alvos
        if(fase.alvos.size()>1){
            EscreveInteiroDireita(fase.alvos.size(), 1014, 4, VERMELHO, fonteAlvos);
            EscreverDireita(" alvos restantes!" , 1272, 4, VERMELHO, fonteAlvos);
        }
        else if(fase.alvos.size()==1){
            EscreveInteiroDireita(fase.alvos.size(), 1048, 4, VERMELHO, fonteAlvos);
            EscreverDireita(" alvo restante!" , 1272, 4, VERMELHO, fonteAlvos);
        }
        else EscreverDireita("Nenhum alvo restante!" , 1272, 4, VERMELHO, fonteAlvos);

        //Indicar que deve usar as setinhas para aumentar e diminuir
        //Indicar que deve usar espa�o para lan�ar bola
        //Passar de fase
        //Destruir alvo e gerar particulas

        //o frame totalmente pronto ser� mostrado na tela
        EncerraDesenho();
    }
    //o jogo ser� encerrado
    FinalizaJogo();
    return 0;
}

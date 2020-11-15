unsigned long tempo;
#define num_linhas 6
#define num_colunas 7
byte x_inicio = 5;
byte y_inicio = 1;
byte x_fim = 0;
byte y_fim = 6;

typedef struct cel{
  byte h;
  byte k;  
  byte tag; //0 novo, 1 aberto e 2 fechado
  byte c; //custo do caminho de y pra x 
  byte r; //custo do caminho de y pra x baseado no sensor
  byte indice; // 0 - não foi analisado, 1 - analisado, 2 - objeto
  byte nome;
  byte pai; //byte b; //pai, backpointer, ponteiro_tras
}CELULA;

CELULA matriz[6][7]; 
CELULA *lista_aberta = (CELULA*)malloc((num_linhas*num_colunas)*sizeof(CELULA));
byte it_aberta=0;
CELULA lista_fechada[num_linhas*num_colunas];
byte it_fechada=0;
byte primeiro=0;
short ultimo=-1;
byte x_atual;
byte y_atual;
char movimento_robo[num_linhas*num_colunas]; //'b' - baixo, 'c' - cima, 'd' - direita, 'e' - esquerda.
byte contador_movimento=0;
byte primeira_passada=0;

void iniciar_matriz(){
  byte i, j;
  byte cont=1;
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      if(((i==0 && i==3) && (j>=1 && j<=2)) || (i==4 && j==3)){
        matriz[i][j].h=0;
        matriz[i][j].k=0;
        matriz[i][j].tag=0;
        matriz[i][j].c=0;
        matriz[i][j].r=0;
        matriz[i][j].nome=cont++;
        matriz[i][j].indice=2; //objeto
        matriz[i][j].pai=0;
      }
      else{
        matriz[i][j].h=0;
        matriz[i][j].k=0;
        matriz[i][j].tag=0;
        matriz[i][j].c=0;
        matriz[i][j].r=0;        
        matriz[i][j].nome=cont++;
        matriz[i][j].indice=0;
        matriz[i][j].pai=0;
      }
    }
  }
}

void iniciar_lista_aberta(){
  byte i;
  for(i=0;i<num_linhas*num_colunas;i++){
    lista_aberta[i].h=0;
    lista_aberta[i].k=0;
    lista_aberta[i].tag=0;
    lista_aberta[i].c=0;
    lista_aberta[i].r=0; 
    lista_aberta[i].nome=0;
    lista_aberta[i].indice=0;
    lista_aberta[i].pai=0;
  }
}

void iniciar_lista_fechada(){
  byte i;
  for(i=0;i<num_linhas*num_colunas;i++){
    lista_fechada[i].h=0;
    lista_fechada[i].k=0;
    lista_fechada[i].tag=0;
    lista_fechada[i].c=0;
    lista_aberta[i].r=0; 
    lista_aberta[i].nome=0;
    lista_aberta[i].indice=0;
    lista_aberta[i].pai=0;
  }
} 

void mover(){
  short m;
   for(m=0;m<contador_movimento;m++){
    if(movimento_robo[m] == 'b'){
      Serial.println("baixo");
      //frente();
      //parar();
    }
    else{
      if(movimento_robo[m] == 'c'){
        Serial.println("cima");
        //tras();
        //parar();        
      }
      else{
        if(movimento_robo[m] == 'd'){
          Serial.println("direita");
          //direita();
          //parar();
        }
        else{
          if(movimento_robo[m] == 'e'){
            Serial.println("esquerda");
            //esquerda();
            //parar();
          }
          else{
            if(movimento_robo[m] == 'w'){
              Serial.println("superior direita");
              //direita(); //celula da direita depois pra celula de cima
              //parar(); 
              //tras();
              //parar();             
            }
            else{
              if(movimento_robo[m] == 'x'){
                Serial.println("inferior direita");
                //frente(); //celula da direita depois pra celula de baixo
                //parar();                 
                //direita(); 
                //parar();            
              }
              else{
                if(movimento_robo[m] == 'y'){
                  Serial.println("superior esquerda");
                  //esquerda(); //celula da esquerda depois pra celula de cima
                  //parar(); 
                  //tras();
                  //para();             
                }
                else{
                  if(movimento_robo[m] == 'z'){
                    Serial.println("inferior esquerda");
                    //esquerda();
                    //para();
                    //frente();
                    //para();                              
                  }
                }
              }
            }
          }
        }          
      }
    }
  }  
}

void melhor_caminho(byte x, byte y){
  byte i, j;

  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      if(matriz[x][y].pai==matriz[i][j].nome){ //se o nome da celula for igual ao pai da celula atual
        if((x-1)==i && y==j){
          movimento_robo[contador_movimento++]='c'; //***celula de cima, porém o robô irá fazer o caminho contrário(do início ao fim) por isso celula de baixo
          x=i;
          y=j;
          //Serial.println("Cima");
          if(x==x_fim && y==y_fim){
            //Serial.println("Mover");
            
            mover();
            //Serial.println("Fim 1");
            tempo = millis();
            //Serial.print("Tempo de execução em ms: ");
            Serial.println(tempo);
            delay(25000);    /////////////FIM
          }
          melhor_caminho(x, y);
        }
        else{
          if((x+1)==i && y==j){
            
            movimento_robo[contador_movimento++]='b';
            x=i;
            y=j;
            //Serial.println("Baixo");
            if(x==x_fim && y==y_fim){
              //Serial.println("Mover");
              
              mover();
              //Serial.println("Fim 2");
              tempo = millis();
              //Serial.print("Tempo de execução em ms: ");
              Serial.println(tempo);
              delay(25000);
            }
            melhor_caminho(x, y);
          }
          else{
            if(x==i && (y-1)==j){
              //back
              movimento_robo[contador_movimento++]='e'; //celula da esquerda, porém armazena direita
              x=i;
              y=j;
              //Serial.println("Esquerda");
              if(x==x_fim && y==y_fim){
                //Serial.println("Mover");             
                mover();
                //Serial.println("Fim 3");
                tempo = millis();
                //Serial.print("Tempo de execução em ms: ");
                Serial.println(tempo);
                delay(25000);

              }
              melhor_caminho(x, y);
            }
            else{
              if(x==i && (y+1)==j){
              movimento_robo[contador_movimento++]='d';
              x=i;
              y=j;
              //Serial.println("Direita");
              if(x==x_fim && y==y_fim){
                //Serial.println("Mover");
                mover();
                //Serial.println("Fim 4");
                tempo = millis();
                //Serial.print("Tempo de execução em ms: ");
                Serial.println(tempo);
                delay(25000);
              }
              melhor_caminho(x, y);    
            }
            else{
              if(i==(x_atual-1) && j==(y_atual+1)){ //celula superior direita
                movimento_robo[contador_movimento++]='w';
                x=i;
                y=j;
                //Serial.println("Direita");
                if(x==x_fim && y==y_fim){
                  //Serial.println("Mover");
                  mover();
                  //Serial.println("Fim 4");
                  tempo = millis();
                  //Serial.print("Tempo de execução em ms: ");
                  Serial.println(tempo);
                  delay(25000);
                }
                melhor_caminho(x, y);
              }
              else{
                if(i==(x_atual+1) && j==(y_atual+1)){ //celula inferior direita
                  movimento_robo[contador_movimento++]='x';
                  x=i;
                  y=j;
                  //Serial.println("Direita");
                  if(x==x_fim && y==y_fim){
                    //Serial.println("Mover");
                    mover();
                    //Serial.println("Fim 4");
                    tempo = millis();
                    //Serial.print("Tempo de execução em ms: ");
                    Serial.println(tempo);
                    delay(25000);
                  }
                  melhor_caminho(x, y);
                }
                else{
                  if(i==(x_atual-1) && j==(y_atual-1)){ //celula superior esquerda
                    movimento_robo[contador_movimento++]='y';
                    x=i;
                    y=j;
                    //Serial.println("Direita");
                    if(x==x_fim && y==y_fim){
                      //Serial.println("Mover");
                      mover();
                      //Serial.println("Fim 4");
                      tempo = millis();
                      //Serial.print("Tempo de execução em ms: ");
                      Serial.println(tempo);
                      delay(25000);
                    }
                    melhor_caminho(x, y);
                  }
                  else{
                    if(i==(x_atual+1) && j==(y_atual-1)){ //celula inferior esquerda
                      movimento_robo[contador_movimento++]='z';
                      x=i;
                      y=j;
                      //Serial.println("Direita");
                      if(x==x_fim && y==y_fim){
                        //Serial.println("Mover");
                        mover();
                        //Serial.println("Fim 4");
                        tempo = millis();
                        //Serial.print("Tempo de execução em ms: ");
                        Serial.println(tempo);
                        delay(25000);
                      }
                      melhor_caminho(x, y);
                    }                 
                  }                                                                              
                 }
                }
              }
            }
          }
        }       
      }
    }
  }
}

void detectar_vizinhos(byte x_atual, byte y_atual){
  byte i, j, n;
  //nao pode considerar objeto e quem esta na lista aberta e fechada
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      if(i==(x_atual-1) && j==y_atual && matriz[i][j].tag==0){ //se for um vizinho
        matriz[i][j].pai = matriz[x_atual][y_atual].nome;     

        if(matriz[i][j].indice==2){
          matriz[i][j].h = 255;
          matriz[i][j].k = 255;
          lista_aberta[it_aberta] = matriz[i][j];
          it_aberta++;
          ultimo++;
          matriz[i][j].tag=1;
        }
        else{
          matriz[i][j].h = matriz[x_atual][y_atual].h + 1;
          matriz[i][j].k = matriz[x_atual][y_atual].k + 1;
          lista_aberta[it_aberta] = matriz[i][j];
          it_aberta++;
          ultimo++;
          matriz[i][j].tag=1;
        }
    
      }
      if(i==x_atual && j==(y_atual+1) && matriz[i][j].tag==0){
        matriz[i][j].pai = matriz[x_atual][y_atual].nome;
       
        if(matriz[i][j].indice==2){
          matriz[i][j].h = 255;
          matriz[i][j].k = 255;
          lista_aberta[it_aberta] = matriz[i][j];
          it_aberta++;
          ultimo++;
          matriz[i][j].tag=1;
        }
        else{
          matriz[i][j].h = matriz[x_atual][y_atual].h + 1;
          matriz[i][j].k = matriz[x_atual][y_atual].k + 1;
          lista_aberta[it_aberta] = matriz[i][j];
          it_aberta++;
          ultimo++;
          matriz[i][j].tag=1;
        }
    
      }
      if(i==(x_atual+1) && j==y_atual && matriz[i][j].tag==0){
        matriz[i][j].pai = matriz[x_atual][y_atual].nome;
        
        if(matriz[i][j].indice==2){
          matriz[i][j].h = 255;
          matriz[i][j].k = 255;
          lista_aberta[it_aberta] = matriz[i][j];
          it_aberta++;
          ultimo++;
          matriz[i][j].tag=1;
        }
        else{
          matriz[i][j].h = matriz[x_atual][y_atual].h + 1;
          matriz[i][j].k = matriz[x_atual][y_atual].k + 1;
          lista_aberta[it_aberta] = matriz[i][j];
          it_aberta++;
          ultimo++;
          matriz[i][j].tag=1;
        }
     
      }
      if(i==x_atual && j==(y_atual-1) && matriz[i][j].tag==0){
        matriz[i][j].pai = matriz[x_atual][y_atual].nome;      

        if(matriz[i][j].indice==2){
          matriz[i][j].h = 255;
          matriz[i][j].k = 255;
          lista_aberta[it_aberta] = matriz[i][j];
          it_aberta++;
          ultimo++;
          matriz[i][j].tag=1;
        }
        else{
          matriz[i][j].h = matriz[x_atual][y_atual].h + 1;
          matriz[i][j].k = matriz[x_atual][y_atual].k + 1;
          lista_aberta[it_aberta] = matriz[i][j];
          it_aberta++;
          ultimo++;
          matriz[i][j].tag=1;
        }
      
      }

      
      if(i==(x_atual-1) && j==(y_atual+1) && matriz[i][j].tag==0){ //celula superior direita
        matriz[i][j].pai = matriz[x_atual][y_atual].nome;
        
        if(matriz[i][j].indice==2){
          matriz[i][j].h = 255;
          matriz[i][j].k = 255;
          lista_aberta[it_aberta] = matriz[i][j];
          it_aberta++;
          ultimo++;
          matriz[i][j].tag=1;
        }
        else{
          matriz[i][j].h = matriz[x_atual][y_atual].h + 1.4;
          matriz[i][j].k = matriz[x_atual][y_atual].k + 1.4;
          lista_aberta[it_aberta] = matriz[i][j];
          it_aberta++;
          ultimo++;
          matriz[i][j].tag=1;
        }
    
      }


      if(i==(x_atual+1) && j==(y_atual+1) && matriz[i][j].tag==0){ //celula inferior direita
        matriz[i][j].pai = matriz[x_atual][y_atual].nome;        

        if(matriz[i][j].indice==2){
          matriz[i][j].h = 255;
          matriz[i][j].k = 255;
          lista_aberta[it_aberta] = matriz[i][j];
          it_aberta++;
          ultimo++;
          matriz[i][j].tag=1;
        }
        else{
          matriz[i][j].h = matriz[x_atual][y_atual].h + 1.4;
          matriz[i][j].k = matriz[x_atual][y_atual].k + 1.4;
          lista_aberta[it_aberta] = matriz[i][j];
          it_aberta++;
          ultimo++;
          matriz[i][j].tag=1;
        }

      }

      if(i==(x_atual-1) && j==(y_atual-1) && matriz[i][j].tag==0){ //celula superior esquerda
        matriz[i][j].pai = matriz[x_atual][y_atual].nome;
        
        if(matriz[i][j].indice==2){
          matriz[i][j].h = 255;
          matriz[i][j].k = 255;
          lista_aberta[it_aberta] = matriz[i][j];
          it_aberta++;
          ultimo++;
          matriz[i][j].tag=1;
        }
        else{
          matriz[i][j].h = matriz[x_atual][y_atual].h + 1.4;
          matriz[i][j].k = matriz[x_atual][y_atual].k + 1.4;
          lista_aberta[it_aberta] = matriz[i][j];
          it_aberta++;
          ultimo++;
          matriz[i][j].tag=1;
        }
        
      }

      if(i==(x_atual+1) && j==(y_atual-1) && matriz[i][j].tag==0){ //celula inferior esquerda
        matriz[i][j].pai = matriz[x_atual][y_atual].nome;
        

        if(matriz[i][j].indice==2){
          matriz[i][j].h = 255;
          matriz[i][j].k = 255;
          lista_aberta[it_aberta] = matriz[i][j];
          it_aberta++;
          ultimo++;
          matriz[i][j].tag=1;
        }
        else{
          matriz[i][j].h = matriz[x_atual][y_atual].h + 1.4;
          matriz[i][j].k = matriz[x_atual][y_atual].k + 1.4;
          lista_aberta[it_aberta] = matriz[i][j];
          it_aberta++;
          ultimo++;
          matriz[i][j].tag=1;
        }
        
      }          
    }
  }
  if(x_atual == x_inicio && y_atual == y_inicio){
    melhor_caminho(x_atual, y_atual);
  }
}

void insertion_sort(byte primeiro, short ultimo){
  byte i, j;
  CELULA key;
  for (i = (primeiro+1); i <= ultimo; i++) {
    key = lista_aberta[i];
    j = i - 1;
    while (j >= primeiro && lista_aberta[j].k > key.k) {
        lista_aberta[j + 1] = lista_aberta[j];
        j = j - 1;
    }
    lista_aberta[j + 1] = key;
  }
}

void busca_celula_analisar(){
  byte i, j, contador;
  contador=0;
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      contador++;
      if(lista_aberta[primeiro].nome == contador){
        x_atual=i;
        y_atual=j;
      }     
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  pinMode(3, OUTPUT); //saída do arduino para a entrada do l293d
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  iniciar_matriz();
  iniciar_lista_aberta();
  iniciar_lista_fechada();
  x_atual=x_fim;
  y_atual=y_fim;
  if(primeira_passada == 1){
    lista_aberta[it_aberta] = matriz[x_atual][y_atual];
    it_aberta++;
    ultimo++;
    matriz[x_atual][y_atual].tag = 1;    
  }
  primeira_passada = 0;
  detectar_vizinhos(x_atual, y_atual);
  lista_fechada[it_fechada] = lista_aberta[primeiro];
  it_fechada++;
  primeiro++;
  insertion_sort(primeiro, ultimo);
  busca_celula_analisar();
  //posicao atual

  //quando encontra a posição inicial, funções insertion e o busca, ajustes de indice e tag
}

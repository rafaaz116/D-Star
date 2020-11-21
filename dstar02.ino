unsigned long tempo;
#define num_linhas 6
#define num_colunas 7
byte x_inicio = 5;
byte y_inicio = 1;
byte x_fim = 0;
byte y_fim = 6;

typedef struct cel{
  double h;
  double k;  
  byte tag; //0 novo, 1 aberto e 2 fechado
  //byte c; //custo do caminho de y pra x 
  //byte r; //custo do caminho de y pra x baseado no sensor
  byte indice; // 0 - não foi analisado, 1 - analisado, 2 - objeto
  byte nome;
  byte pai; //byte b; //pai, backpointer, ponteiro_tras
}CELULA;

CELULA matriz[6][7]; 
CELULA lista_aberta[num_linhas*num_colunas];
byte it_aberta=0;
CELULA lista_fechada[num_linhas*num_colunas];
byte it_fechada=0;
byte primeiro=0;
short ultimo=-1;
byte x_atual;
byte y_atual;
char movimento_robo[num_linhas*num_colunas]; //'b' - baixo, 'c' - cima, 'd' - direita, 'e' - esquerda.
byte contador_movimento=0;
byte primeira_passada=1;


void melhor_caminho(byte x, byte y);


void iniciar_matriz(){
  byte i, j;
  byte cont=1;
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      if(((i>=0 && i<=3) && (j>=1 && j<=2)) || (i==4 && j==3)){
        matriz[i][j].h=0;
        matriz[i][j].k=0;
        matriz[i][j].tag=0;
//        matriz[i][j].c=0;
//        matriz[i][j].r=0;
        matriz[i][j].nome=cont++;
        matriz[i][j].indice=2; //objeto
        matriz[i][j].pai=0;
      }
      else{
        matriz[i][j].h=0;
        matriz[i][j].k=0;
        matriz[i][j].tag=0;
//        matriz[i][j].c=0;
//        matriz[i][j].r=0;        
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
//    lista_aberta[i].c=0;
//    lista_aberta[i].r=0; 
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
//    lista_fechada[i].c=0;
//    lista_fechada[i].r=0; 
    lista_fechada[i].nome=0;
    lista_fechada[i].indice=0;
    lista_fechada[i].pai=0;
  }
} 

/*
void atualiza_matriz(int x, int y){
  int i, j;
  int x_pos_robo, y_pos_robo;
  
  matriz[x][y].h = 255; //a posição atual é um objeto (na )
  
  lista_aberta[it_aberta] = matriz[x][y];//add obstaculo e nós adjacentes na lista aberta e ordena-os.(FAZER UMA FUNÇÃO PARA ESSA PARTE QUE SE REPETE, VERIFICAR SE O NÓ ESTA NA LISTA ABERTA E NÃO ADD SE ELE JÁ ESTÁ)      
  it_aberta++;
  ultimo++;
  matriz[x][y].tag=1; //analisado de novo, ou seja permanece com 1.

  lista_aberta[it_aberta] = matriz[x-1][y];//add obstaculo e nós adjacentes na lista aberta e ordena-os.         
  it_aberta++;
  ultimo++;
  matriz[x][y].tag=1; //analisado de novo, ou seja permanece com 1.

  lista_aberta[it_aberta] = matriz[x][y+1];//add obstaculo e nós adjacentes na lista aberta e ordena-os.         
  it_aberta++;
  ultimo++;
  matriz[x][y].tag=1; //analisado de novo, ou seja permanece com 1.

  lista_aberta[it_aberta] = matriz[x+1][y];//add obstaculo e nós adjacentes na lista aberta e ordena-os.         
  it_aberta++;
  ultimo++;
  matriz[x][y].tag=1; //analisado de novo, ou seja permanece com 1.

  lista_aberta[it_aberta] = matriz[x][y-1];//add obstaculo e nós adjacentes na lista aberta e ordena-os.         
  it_aberta++;
  ultimo++;
  matriz[x][y].tag=1; //analisado de novo, ou seja permanece com 1.

    

  lista_aberta[it_aberta] = matriz[x-1][y+1];//add obstaculo e nós adjacentes na lista aberta e ordena-os.         
  it_aberta++;
  ultimo++;
  matriz[x][y].tag=1; //analisado de novo, ou seja permanece com 1.

  lista_aberta[it_aberta] = matriz[x+1][y+1];//add obstaculo e nós adjacentes na lista aberta e ordena-os.         
  it_aberta++;
  ultimo++;
  matriz[x][y].tag=1; //analisado de novo, ou seja permanece com 1.

  lista_aberta[it_aberta] = matriz[x-1][y-1];//add obstaculo e nós adjacentes na lista aberta e ordena-os.         
  it_aberta++;
  ultimo++;
  matriz[x][y].tag=1; //analisado de novo, ou seja permanece com 1.

  lista_aberta[it_aberta] = matriz[x+1][y-1];//add obstaculo e nós adjacentes na lista aberta e ordena-os.         
  it_aberta++;
  ultimo++;
  matriz[x][y].tag=1; //analisado de novo, ou seja permanece com 1.


  insertion_sort(primeiro, ultimo);

  //ele vai fazer este processo até que chegue em uma posição da lista aberta que seja o obstáculo, aí ele analisa quem aponta pra ele
  while(x!=x_atual && y!=y_atual){ //ANALISAR AONDE TINHA PARADO O X ATUAL E O Y ATUAL KKK, enquanto x do objeto for diferente do atual
    detectar_vizinhos(x_atual, y_atual);
    lista_fechada[it_fechada] = lista_aberta[primeiro];
    it_fechada++;
    primeiro++;
    insertion_sort(primeiro, ultimo);
    busca_celula_analisar();  
  }
  //se chegou no objeto, ele procura quem aponta pra ele.
    
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      if(matriz[x][y].nome == matriz[i][j].pai){ //celula atual.nome == posicao iterativa.pai, se tem uma celula vizinha que aponta pra o atual
        matriz[i][j].h = 255; //raise, elevação.
        x_pos_robo = i;
        y_pos_robo = j; 
      }
    }
  }

  while(x!=x_pos_robo && y!=y_pos_robo){ //ANALISAR AONDE TINHA PARADO O X ATUAL E O Y ATUAL KKK, enquanto x do objeto for diferente do atual
    detectar_vizinhos(x_atual, y_atual);
    lista_fechada[it_fechada] = lista_aberta[primeiro];
    it_fechada++;
    primeiro++;
    insertion_sort(primeiro, ultimo);
    busca_celula_analisar();  //posição da primeira celula da lista aberta!
  }
  //se for igual a posição que aponta para o obstáculo(é a posição que está o robô)
  //> (3,2)
  while(lista_aberta[primeiro].k < matriz[x_pos_robo][y_pos_robo].h){
    if(matriz[x_pos_robo][y_pos_robo].h > matriz[x_pos_robo][y_pos_robo].k){
      for(i=0;i<num_linhas;i++){
        for(j=0;j<num_colunas;j++){
          if((x_pos_robo-1)==i && (y_pos_robo==j)){ //é um vizinho
            if(matriz[i][j].h < matriz[x_pos_robo][y_pos_robo].k){ //se esse vizinho tem um h menor que o atual_robo.k
              matriz[x_pos_robo][y_pos_robo].pai =  matriz[i][j].nome; //pai aponta pra esse vizinho.
            }
            else{
              if(matriz[i][j].pai == matriz[x_pos_robo][y_pos_robo].nome){
                 matriz[i][j].h =  255;
                 lista_aberta[it_aberta] = matriz[i][j];
                 it_aberta++;
                 ultimo++;
                 matriz[i][j].tag=1;
              }
              else{ //adjacente que não aponta pra posição do robô (3,2)
              //permanece com o mesmo valor(LOWER) e só add apenas na lista aberta.
                if(matriz[i][j].h != matriz[i][j].k){
                  matriz[i][j].k = matriz[i][j].h; //k recebe o h novo;
                  lista_aberta[it_aberta] = matriz[i][j];
                }
                else{
                  lista_aberta[it_aberta] = matriz[i][j]; 
                }           
              }  
            }
          }
          if((x_pos_robo)==i && ((y_pos_robo+1)==j)){
            if(matriz[i][j].h < matriz[x_pos_robo][y_pos_robo].k){ //se esse vizinho tem um h menor que o atual_robo.k
              matriz[x_pos_robo][y_pos_robo].pai =  matriz[i][j].nome; //pai aponta pra esse vizinho.
            }
            else{
              if(matriz[i][j].pai == matriz[x_pos_robo][y_pos_robo].nome){
                 matriz[i][j].h =  255;
                 lista_aberta[it_aberta] = matriz[i][j];
                 it_aberta++;
                 ultimo++;
                 matriz[i][j].tag=1;
              }
              else{ //adjacente que não aponta pra posição do robô (3,2)
              //permanece com o mesmo valor(LOWER) e só add apenas na lista aberta.
                if(matriz[i][j].h != matriz[i][j].k){
                  matriz[i][j].k = matriz[i][j].h; //k recebe o h novo;
                  lista_aberta[it_aberta] = matriz[i][j];
                }
                else{
                  lista_aberta[it_aberta] = matriz[i][j]; 
                }           
              }  
            }        
          }
          if((x_pos_robo+1)==i && (y_pos_robo==j)){
            if(matriz[i][j].h < matriz[x_pos_robo][y_pos_robo].k){ //se esse vizinho tem um h menor que o atual_robo.k
              matriz[x_pos_robo][y_pos_robo].pai =  matriz[i][j].nome; //pai aponta pra esse vizinho.
            }
            else{
              if(matriz[i][j].pai == matriz[x_pos_robo][y_pos_robo].nome){
                 matriz[i][j].h =  255;
                 lista_aberta[it_aberta] = matriz[i][j];
                 it_aberta++;
                 ultimo++;
                 matriz[i][j].tag=1;
              }
              else{ //adjacente que não aponta pra posição do robô (3,2)
              //permanece com o mesmo valor(LOWER) e só add apenas na lista aberta.
                if(matriz[i][j].h != matriz[i][j].k){
                  matriz[i][j].k = matriz[i][j].h; //k recebe o h novo;
                  lista_aberta[it_aberta] = matriz[i][j];
                }
                else{
                  lista_aberta[it_aberta] = matriz[i][j]; 
                }                       }  
            }
          }
          if((x_pos_robo)==i && ((y_pos_robo-1)==j)){
            if(matriz[i][j].h < matriz[x_pos_robo][y_pos_robo].k){ //se esse vizinho tem um h menor que o atual_robo.k
              matriz[x_pos_robo][y_pos_robo].pai =  matriz[i][j].nome; //pai aponta pra esse vizinho.
            }
            else{
              if(matriz[i][j].pai == matriz[x_pos_robo][y_pos_robo].nome){
                 matriz[i][j].h =  255;
                 lista_aberta[it_aberta] = matriz[i][j];
                 it_aberta++;
                 ultimo++;
                 matriz[i][j].tag=1;
              }
              else{ //adjacente que não aponta pra posição do robô (3,2)
              //permanece com o mesmo valor(LOWER) e só add apenas na lista aberta.
                if(matriz[i][j].h != matriz[i][j].k){
                  matriz[i][j].k = matriz[i][j].h; //k recebe o h novo;
                  lista_aberta[it_aberta] = matriz[i][j];
                }
                else{
                  lista_aberta[it_aberta] = matriz[i][j]; 
                }                       }  
            }
          }
          if((x_pos_robo-1)==i && ((y_pos_robo+1)==j)){
            if(matriz[i][j].h < matriz[x_pos_robo][y_pos_robo].k){ //se esse vizinho tem um h menor que o atual_robo.k
              matriz[x_pos_robo][y_pos_robo].pai =  matriz[i][j].nome; //pai aponta pra esse vizinho.
            }
            else{
              if(matriz[i][j].pai == matriz[x_pos_robo][y_pos_robo].nome){
                 matriz[i][j].h =  255;
                 lista_aberta[it_aberta] = matriz[i][j];
                 it_aberta++;
                 ultimo++;
                 matriz[i][j].tag=1;
              }
              else{ //adjacente que não aponta pra posição do robô (3,2)
              //permanece com o mesmo valor(LOWER) e só add apenas na lista aberta.
                if(matriz[i][j].h != matriz[i][j].k){
                  matriz[i][j].k = matriz[i][j].h; //k recebe o h novo;
                  lista_aberta[it_aberta] = matriz[i][j];
                }
                else{
                  lista_aberta[it_aberta] = matriz[i][j]; 
                }                       }  
            }        
          }
          if((x_pos_robo+1)==i && ((y_pos_robo+1)==j)){
            if(matriz[i][j].h < matriz[x_pos_robo][y_pos_robo].k){ //se esse vizinho tem um h menor que o atual_robo.k
              matriz[x_pos_robo][y_pos_robo].pai =  matriz[i][j].nome; //pai aponta pra esse vizinho.
            }
            else{
              if(matriz[i][j].pai == matriz[x_pos_robo][y_pos_robo].nome){
                 matriz[i][j].h =  255;
                 lista_aberta[it_aberta] = matriz[i][j];
                 it_aberta++;
                 ultimo++;
                 matriz[i][j].tag=1;
              }
              else{ //adjacente que não aponta pra posição do robô (3,2)
              //permanece com o mesmo valor(LOWER) e só add apenas na lista aberta.
                if(matriz[i][j].h != matriz[i][j].k){
                  matriz[i][j].k = matriz[i][j].h; //k recebe o h novo;
                  lista_aberta[it_aberta] = matriz[i][j];
                }
                else{
                  lista_aberta[it_aberta] = matriz[i][j]; 
                }                       }  
            }
          }
          if((x_pos_robo-1)==i && ((y_pos_robo-1)==j)){
            if(matriz[i][j].h < matriz[x_pos_robo][y_pos_robo].k){ //se esse vizinho tem um h menor que o atual_robo.k
              matriz[x_pos_robo][y_pos_robo].pai =  matriz[i][j].nome; //pai aponta pra esse vizinho.
            }
            else{
              if(matriz[i][j].pai == matriz[x_pos_robo][y_pos_robo].nome){
                 matriz[i][j].h =  255;
                 lista_aberta[it_aberta] = matriz[i][j];
                 it_aberta++;
                 ultimo++;
                 matriz[i][j].tag=1;
              }
              else{ //adjacente que não aponta pra posição do robô (3,2)
              //permanece com o mesmo valor(LOWER) e só add apenas na lista aberta.
                if(matriz[i][j].h != matriz[i][j].k){
                  matriz[i][j].k = matriz[i][j].h; //k recebe o h novo;
                  lista_aberta[it_aberta] = matriz[i][j];
                }
                else{
                  lista_aberta[it_aberta] = matriz[i][j]; 
                }                       }  
            }
          }
          if((x_pos_robo+1)==i && ((y_pos_robo-1)==j)){
            if(matriz[i][j].h < matriz[x_pos_robo][y_pos_robo].k){ //se esse vizinho tem um h menor que o atual_robo.k
              matriz[x_pos_robo][y_pos_robo].pai =  matriz[i][j].nome; //pai aponta pra esse vizinho.
            }
            else{
              if(matriz[i][j].pai == matriz[x_pos_robo][y_pos_robo].nome){
                 matriz[i][j].h =  255;
                 lista_aberta[it_aberta] = matriz[i][j];
                 it_aberta++;
                 ultimo++;
                 matriz[i][j].tag=1;
              }
              else{ //adjacente que não aponta pra posição do robô (3,2)
              //permanece com o mesmo valor(LOWER) e só add apenas na lista aberta.
                if(matriz[i][j].h != matriz[i][j].k){
                  matriz[i][j].k = matriz[i][j].h; //k recebe o h novo;
                  lista_aberta[it_aberta] = matriz[i][j];
                }
                else{
                  lista_aberta[it_aberta] = matriz[i][j]; 
                }                       
              }  
            }
          }                                                
        } 
      } 
    }
    else{
      if(matriz[x_atual][y_atual].k == matriz[y_atual][y_atual].h){ // Como K=H
        //verifica os vizinhos
      for(i=0;i<num_linhas;i++){
        for(j=0;j<num_colunas;j++){
          if((x_pos_robo-1)==i && (y_pos_robo==j)){ //é um vizinho
            if(matriz[i][j].h == 255){ //se esse vizinho tem um h menor que o atual_robo.k
              matriz[i][j].pai =  matriz[x_atual][y_atual].nome; //vizinho aponta para o atual.
              matriz[i][j].h = matriz[x_atual][y_atual].h + 1; //LOWER
              matriz[i][j].k = matriz[i][j].h;
              lista_aberta[it_aberta] = matriz[i][j];
              it_aberta++;
              ultimo++;
              matriz[i][j].tag=1;
            }
          }
          if((x_pos_robo)==i && ((y_pos_robo+1)==j)){
            if(matriz[i][j].h == 255){ //se esse vizinho tem um h menor que o atual_robo.k
              matriz[i][j].pai =  matriz[x_atual][y_atual].nome; //vizinho aponta para o atual.
              matriz[i][j].h = matriz[x_atual][y_atual].h + 1; //LOWER
              matriz[i][j].k = matriz[i][j].h;
              lista_aberta[it_aberta] = matriz[i][j];
              it_aberta++;
              ultimo++;
              matriz[i][j].tag=1;
            }       
          }
          if((x_pos_robo+1)==i && (y_pos_robo==j)){
            if(matriz[i][j].h == 255){ //se esse vizinho tem um h menor que o atual_robo.k
              matriz[i][j].pai =  matriz[x_atual][y_atual].nome; //vizinho aponta para o atual.
              matriz[i][j].h = matriz[x_atual][y_atual].h + 1; //LOWER
              matriz[i][j].k = matriz[i][j].h;
              lista_aberta[it_aberta] = matriz[i][j];
              it_aberta++;
              ultimo++;
              matriz[i][j].tag=1;            
            }
          }
          if((x_pos_robo)==i && ((y_pos_robo-1)==j)){
            if(matriz[i][j].h == 255){ //se esse vizinho tem um h menor que o atual_robo.k
              matriz[i][j].pai =  matriz[x_atual][y_atual].nome; //vizinho aponta para o atual.
              matriz[i][j].h = matriz[x_atual][y_atual].h + 1; //LOWER
              matriz[i][j].k = matriz[i][j].h;
              lista_aberta[it_aberta] = matriz[i][j];
              it_aberta++;
              ultimo++;
              matriz[i][j].tag=1;            
            }
          }
          if((x_pos_robo-1)==i && ((y_pos_robo+1)==j)){
            if(matriz[i][j].h == 255){ //se esse vizinho tem um h menor que o atual_robo.k
              matriz[i][j].pai =  matriz[x_atual][y_atual].nome; //vizinho aponta para o atual.
              matriz[i][j].h = matriz[x_atual][y_atual].h + 1.4; //LOWER
              matriz[i][j].k = matriz[i][j].h;
              lista_aberta[it_aberta] = matriz[i][j];
              it_aberta++;
              ultimo++;
              matriz[i][j].tag=1;       
            }        
          }
          if((x_pos_robo+1)==i && ((y_pos_robo+1)==j)){
            if(matriz[i][j].h == 255){ //se esse vizinho tem um h menor que o atual_robo.k
              matriz[i][j].pai =  matriz[x_atual][y_atual].nome; ////vizinho aponta para o atual.
              matriz[i][j].h = matriz[x_atual][y_atual].h + 1.4; //LOWER
              matriz[i][j].k = matriz[i][j].h;
              lista_aberta[it_aberta] = matriz[i][j];
              it_aberta++;
              ultimo++;
              matriz[i][j].tag=1;            
            }
          }
          if((x_pos_robo-1)==i && ((y_pos_robo-1)==j)){
            if(matriz[i][j].h == 255){ //se esse vizinho tem um h menor que o atual_robo.k
              matriz[i][j].pai =  matriz[x_atual][y_atual].nome; ////vizinho aponta para o atual.
              matriz[i][j].h = matriz[x_atual][y_atual].h + 1.4; //LOWER
              matriz[i][j].k = matriz[i][j].h;
              lista_aberta[it_aberta] = matriz[i][j];
              it_aberta++;
              ultimo++;
              matriz[i][j].tag=1;            
            }
          }
          if((x_pos_robo+1)==i && ((y_pos_robo-1)==j)){
            if(matriz[i][j].h == 255){ //se esse vizinho tem um h menor que o atual_robo.k
              matriz[i][j].pai =  matriz[x_atual][y_atual].nome; //vizinho aponta para o atual.
              matriz[i][j].h = matriz[x_atual][y_atual].h + 1.4; //LOWER
              matriz[i][j].k = matriz[i][j].h;
              lista_aberta[it_aberta] = matriz[i][j];
              it_aberta++;
              ultimo++;
              matriz[i][j].tag=1;            
            }
          }                                                
        } 
      }
        //apenas alguns vizinhos apontam pra posição atual.
        //colaca-os na lista aberta 
        //lower.  
      }
    }
  }
  insertion_sort(primeiro, ultimo);
  busca_celula_analisar(); 
}
*/
void mover(byte x, byte y){
  byte m;
  //byte x_obstaculo, y_obstaculo;
   for(m=0;m<contador_movimento;m++){
    if(movimento_robo[m] == 'b'){
      Serial.println("baixo");
      //x = x + 1;    
      
      //frente();//SENSOR MEDE PRA VER SE TEM UM OBSTÁCULO, se tem pára e anda em um direção contrária, senão o robô permanece na mesma direção.
      //parar();
      
      //x = x-1;// SE TEM UM OBSTÁCULO, retorna a posição anterior
      //x_obstaculo = x+1; // salva a posição do obstáculo para enviar para a atualização da matriz.
      //y_obstaculo = y;
      //atualiza_matriz(x_obstaculo, y_obstaculo);
      //melhor_caminho(x_atual_antes do robo, y antes do obstaculo);
      //mover(x antes do obstaculo, y antes do obstaculo);
    }
    else{
      if(movimento_robo[m] == 'c'){
        Serial.println("cima");
        //x = x - 1;
        //tras();
        //parar();        
      }
      else{
        if(movimento_robo[m] == 'd'){
          Serial.println("direita");
          //y = y + 1;
          //direita();
          //parar();
        }
        else{
          if(movimento_robo[m] == 'e'){
            Serial.println("esquerda");
            //y = y - 1; ///////////////////////////////parei aqui!!!
            //atualiza_matriz(x, y); // envia como parâmentro o x e o y atuais.
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

  for(i=0;i<num_linhas;i++){ //percorre toda matriz.
    for(j=0;j<num_colunas;j++){
      if(matriz[x][y].pai==matriz[i][j].nome){ //se o nome da celula for igual ao pai da celula atual
        if((x-1)==i && y==j){
          movimento_robo[contador_movimento++]='c'; //***celula de cima, porém o robô irá fazer o caminho contrário(do início ao fim) por isso celula de baixo
          x=i;
          y=j;
          //Serial.println("Cima");
          if(x==x_fim && y==y_fim){
            //Serial.println("Mover");
            
            mover(x, y);
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
              
              mover(x, y);
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
                mover(x, y);
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
                mover(x, y);
                //Serial.println("Fim 4");
                tempo = millis();
                //Serial.print("Tempo de execução em ms: ");
                Serial.println(tempo);
                delay(25000);
              }
              melhor_caminho(x, y);    
            }
            else{
              if(i==(x-1) && j==(y+1)){ //celula superior direita
                movimento_robo[contador_movimento++]='w';
                x=i;
                y=j;
                Serial.println("Superior Direita");
                if(x==x_fim && y==y_fim){
                  //Serial.println("Mover");
                  mover(x, y);
                  //Serial.println("Fim 4");
                  tempo = millis();
                  //Serial.print("Tempo de execução em ms: ");
                  Serial.println(tempo);
                  delay(25000);
                }
                melhor_caminho(x, y);
              }
              else{
                if(i==(x+1) && j==(y+1)){ //celula inferior direita
                  movimento_robo[contador_movimento++]='x';
                  x=i;
                  y=j;
                  Serial.println("Inferior Direita");
                  if(x==x_fim && y==y_fim){
                    //Serial.println("Mover");
                    mover(x, y);
                    //Serial.println("Fim 4");
                    tempo = millis();
                    //Serial.print("Tempo de execução em ms: ");
                    Serial.println(tempo);
                    delay(25000);
                  }
                  melhor_caminho(x, y);
                }
                else{
                  if(i==(x-1) && j==(y-1)){ //celula superior esquerda
                    movimento_robo[contador_movimento++]='y';
                    x=i;
                    y=j;
                    //Serial.println("Superior Esquerda");
                    if(x==x_fim && y==y_fim){
                      //Serial.println("Mover");
                      mover(x, y);
                      //Serial.println("Fim 4");
                      tempo = millis();
                      //Serial.print("Tempo de execução em ms: ");
                      Serial.println(tempo);
                      delay(25000);
                    }
                    melhor_caminho(x, y);
                  }
                  else{
                    if(i==(x+1) && j==(y-1)){ //celula inferior esquerda
                      movimento_robo[contador_movimento++]='z';
                      x=i;
                      y=j;
                      //Serial.println("Inferior Esquerda");
                      if(x==x_fim && y==y_fim){
                        //Serial.println("Mover");
                        mover(x, y);
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
      if(x_atual==x_inicio && y_atual==y_inicio){
        Serial.println("Chama melhor caminho v"); //pq ta entrando aqui?
        //Serial.println(i); // ta dando bug quando comparo i e j
        //Serial.println(j); //bug
        melhor_caminho(x_atual, y_atual);
      }       
    }
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

void imprimir_matriz_indice(){
  byte i, j;
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      Serial.print(matriz[i][j].indice);
      Serial.print(' ');
    }
    Serial.println(" ");
  }
  Serial.println(" ");  
}

void imprimir_matriz_h(){
  byte i, j;
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      Serial.print(matriz[i][j].h);
      Serial.print(' ');
    }
    Serial.println(" ");
  }
  Serial.println(" ");  
}

void imprime_lista_aberta(byte primerio){
  byte i;
  for(i=primeiro;i<num_linhas*num_colunas;i++){
    Serial.print(lista_aberta[i].nome);
    Serial.print(" ");
  }
  Serial.println(" ");
}

void imprime_lista_fechada(){
  byte i;
  for(i=0;i<num_linhas*num_colunas;i++){
    Serial.print(lista_fechada[i].nome);
    Serial.print(" ");
  }
  Serial.println(" ");
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
  if(primeira_passada == 1){  
    iniciar_matriz();
    imprimir_matriz_indice(); //Impressão. 
    iniciar_lista_aberta();
    iniciar_lista_fechada();
    x_atual=x_fim;
    y_atual=y_fim;
  
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
  imprimir_matriz_h();//Impressão.

  //imprime_lista_aberta(primeiro);
  //imprime_lista_fechada();
  //Serial.println(x_atual);
  //Serial.println(y_atual);
  Serial.println("");

  //quando encontra a posição inicial, funções insertion e o busca, ajustes de indice e tag
}

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
CELULA *lista_aberta = (CELULA*)malloc(((num_linhas*num_colunas)+20)*sizeof(CELULA));
//CELULA lista_aberta[num_linhas*num_colunas+10];
byte it_aberta=0;
//CELULA *lista_fechada = (CELULA*)malloc(((num_linhas*num_colunas))*sizeof(CELULA));
CELULA lista_fechada[num_linhas*num_colunas];
byte it_fechada=0;
byte primeiro=0;
short ultimo=-1;
byte x_atual;
byte y_atual;
char movimento_robo[num_linhas*num_colunas]; //'b' baixo, 'c' cima, 'd' direita, 'e' esquerda, 'w' superior direita, 'x' inferior direita, 'y' superior esquerda, 'z' inferior esquerda.
byte contador_movimento=0;
byte primeira_passada=1;
byte x_robo; 
byte y_robo;
byte obstaculo_x;
byte obstaculo_y;
byte x_pos_robo;
byte y_pos_robo;

void iniciar_matriz(){
  byte i, j;
  byte cont=1;
  byte x_pos_robo, y_pos_robo;
  
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
  for(i=0;i<(num_linhas*num_colunas)+20;i++){
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

void add_lista_aberta(byte m, byte n){ //percorre lista aberta, para verificar se esse nó esta nela.
  byte i;
  byte flag = 0;
  for(i=primeiro;i<num_linhas*num_colunas;i++){
    if(matriz[m][n].nome != lista_aberta[i].nome){
      flag=0;
    }
    else{
      flag=1;
      lista_aberta[i] = matriz[m][n];
      i= num_linhas*num_colunas; //se tem um valor na lista aberta sai do for.
    }
  }

  if(flag == 0){
    //Serial.println("Este valor não está na lista aberta, portanto add ele.");
    lista_aberta[it_aberta] = matriz[m][n];//add obstaculo e nós adjacentes na lista aberta, para ordena-los depois.      
    it_aberta++;
    ultimo++;
    matriz[m][n].tag=1; //analisado de novo, ou seja permanece com 1.
  }
  else{
    //Serial.println("Este valor já esta na lista aberta, portanto não adiciona, apenas atualiza, se precisar");
  }
  
}

void atualiza_matriz(){
  byte i, j;
  
  matriz[x_atual][y_atual].h = 255; //a posição atual é um objeto
  matriz[x_atual][y_atual].indice = 2;

  //add na lista aberta o nó em que está o obstáculo e seus nós adjacentes.
  add_lista_aberta(x_atual, y_atual);
  add_lista_aberta(x_atual-1, y_atual);
  add_lista_aberta(x_atual, y_atual+1);
  add_lista_aberta(x_atual+1, y_atual);
  add_lista_aberta(x_atual, y_atual-1);
  add_lista_aberta(x_atual-1, y_atual+1);
  add_lista_aberta(x_atual+1, y_atual+1);
  add_lista_aberta(x_atual-1, y_atual-1);
  add_lista_aberta(x_atual+1, y_atual-1);

  //analisa lista aberta
  insertion_sort(primeiro, ultimo);
    
  busca_celula_analisar();
  Serial.println(matriz[x_atual][y_atual].nome);
  imprime_lista_aberta(primeiro); 
  
  /*
  imprimir_matriz_pai();
  //imprimir lista aberta;
  insertion_sort(primeiro, ultimo);

  while(lista_aberta[primeiro].nome != matriz[obstaculo_x][obstaculo_y].nome){ //enquanto o primeiro da lista aberta for diferente da posição do obstáculo, calcula os custos de adjacentes.
    detectar_vizinhos(); //como já está tudo calculado, ele não vai recalcular os custos.
    lista_fechada[it_fechada] = lista_aberta[primeiro];
    it_fechada++;
    primeiro++;
    insertion_sort(primeiro, ultimo);
    busca_celula_analisar();
  }
  
  //após chegar no nó obstáculo na lista aberta, procura por quem aponta pra ele.
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      if(matriz[x_atual][y_atual].nome == matriz[i][j].pai){ //celula atual.nome == posicao iterativa.pai, se tem uma celula vizinha que aponta pra o atual.
        matriz[i][j].h = 255; //raise, elevação.
        x_pos_robo = i; 
        y_pos_robo = j; 
        i = num_linhas; 
        j = num_colunas;       
      }
    }
  }
  
  while(lista_aberta[primeiro].nome != matriz[x_pos_robo][y_pos_robo].nome){ // repete o mesmo while, porém com outra posição atual, enquanto não chega na posição que aponta para o obstáculo(no caso a posição que está o robô).
    detectar_vizinhos();
    lista_fechada[it_fechada] = lista_aberta[primeiro];
    it_fechada++;
    primeiro++;
    insertion_sort(primeiro, ultimo);
    busca_celula_analisar();  //posição da primeira celula da lista aberta.
  }
  
  Serial.println("posição em que esta o robô:");
  Serial.println(x_atual);
  Serial.println(y_atual);
  
  //se for igual a posição que aponta para o obstáculo(é a posição que está o robô) 
  //> (3,2)
  */
  while((lista_aberta[primeiro].k+0.01) < matriz[x_robo][y_robo].h){
    Serial.print("Atual K: ");
    Serial.println(lista_aberta[primeiro].k+0.01);
    Serial.print("Robô H: ");
    Serial.println(matriz[x_robo][y_robo].h);
    
    if(matriz[x_atual][y_atual].h > matriz[x_atual][y_atual].k){

      for(i=0;i<num_linhas;i++){
        for(j=0;j<num_colunas;j++){

          if((x_atual-1)==i && y_atual==j && matriz[x_atual-1][y_atual].indice != 2){
            
            //if(matriz[i][j].h < matriz[x_atual][y_atual].k){ //se esse vizinho tem um h menor que o atual_robo.k
             // matriz[x_atual][y_atual].pai =  matriz[i][j].nome; //pai aponta pra esse vizinho.
            //}
            //else{
              if(matriz[i][j].pai == matriz[x_atual][y_atual].nome){
                 matriz[i][j].h =  255;

                 add_lista_aberta(x_atual-1, y_atual);
                 Serial.println("estado em RAISE");
              }
              else{
                  add_lista_aberta(x_atual-1, y_atual);
                  Serial.println("LOWER"); 
              }
           //}
          }
          
          if(x_atual==i && (y_atual+1)==j && matriz[x_atual][y_atual+1].indice != 2){ 
           // if(matriz[i][j].h < matriz[x_atual][y_atual].k){ //se esse vizinho tem um h menor que o atual_robo.k
              //matriz[x_atual][y_atual].pai =  matriz[i][j].nome; //pai aponta pra esse vizinho.
            //}
            //else{
              if(matriz[i][j].pai == matriz[x_atual][y_atual].nome){
                 matriz[i][j].h =  255;
                 add_lista_aberta(x_atual, y_atual+1);
                
                 Serial.println("estado em RAISE");
              }
              else{                 
                  add_lista_aberta(x_atual, y_atual+1);
                  Serial.println("LOWER"); 
              }
            //}
          } 
                        
          if((x_atual+1)==i && y_atual==j && matriz[x_atual+1][y_atual].indice != 2){
            
            //if(matriz[i][j].h < matriz[x_atual][y_atual].k){ //se esse vizinho tem um h menor que o atual_robo.k
             // matriz[x_atual][y_atual].pai =  matriz[i][j].nome; //pai aponta pra esse vizinho.
            //}
           // else{ 

              if(matriz[i][j].pai == matriz[x_atual][y_atual].nome){ 
                              
                 matriz[i][j].h =  255;            

                 add_lista_aberta(x_atual+1, y_atual);
                 imprime_lista_aberta(primeiro);
                 Serial.println("estado em RAISE");

              }
              else{              
                add_lista_aberta(x_atual+1, y_atual);
                Serial.println("LOWER"); 
              }                                                  
           // }
          }

          if((x_atual)==i && ((y_atual-1)==j) && matriz[x_atual][y_atual-1].indice != 2){
           // if(matriz[i][j].h < matriz[x_atual][y_atual].k){ //se esse vizinho tem um h menor que o atual_robo.k
              //matriz[x_atual][y_atual].pai =  matriz[i][j].nome; //pai aponta pra esse vizinho.
           // }
            //else{
              if(matriz[i][j].pai == matriz[x_atual][y_atual].nome){
                 matriz[i][j].h =  255;
                 add_lista_aberta(x_atual, y_atual-1);
  
                 Serial.println("estado em RAISE");
              }
              else{
                add_lista_aberta(x_atual, y_atual-1);
     
                Serial.println("LOWER");
              }                             
                
           // }
          }

          if((x_atual-1)==i && ((y_atual+1)==j) && matriz[x_atual-1][y_atual+1].indice != 2){
           // if(matriz[i][j].h < matriz[x_atual][y_atual].k){ //se esse vizinho tem um h menor que o atual_robo.k
             // matriz[x_atual][y_atual].pai =  matriz[i][j].nome; //pai aponta pra esse vizinho.
           // }
           // else{
              if(matriz[i][j].pai == matriz[x_atual][y_atual].nome){
                 matriz[i][j].h =  255;
  
                 add_lista_aberta(x_atual-1, y_atual+1);
                 Serial.println("estado em RAISE");
              }
              else{
                 
                add_lista_aberta(x_atual-1, y_atual+1);
                Serial.println("LOWER");
              }                  
           // }                               
          }

          if((x_atual+1)==i && ((y_atual+1)==j) && matriz[x_atual+1][y_atual+1].indice != 2){
           // if(matriz[i][j].h < matriz[x_atual][y_atual].k){ //se esse vizinho tem um h menor que o atual_robo.k
              //matriz[x_atual][y_atual].pai =  matriz[i][j].nome; //pai aponta pra esse vizinho.
           // }
           // else{
              if(matriz[i][j].pai == matriz[x_atual][y_atual].nome){
                 matriz[i][j].h =  255;
   
                 add_lista_aberta(x_atual+1, y_atual+1);
                 Serial.println("estado em RAISE");
              }
              else{
                add_lista_aberta(x_atual+1, y_atual+1);   
                Serial.println("LOWER"); 
              }                                                      
            //}
          }
          
          if((x_atual-1)==i && ((y_atual-1)==j) && matriz[x_atual-1][y_atual-1].indice != 2){
           // if(matriz[i][j].h < matriz[x_atual][y_atual].k){ //se esse vizinho tem um h menor que o atual_robo.k
             // matriz[x_atual][y_atual].pai =  matriz[i][j].nome; //pai aponta pra esse vizinho.
            //}
           // else{
              if(matriz[i][j].pai == matriz[x_atual][y_atual].nome){
                 matriz[i][j].h =  255;
           
                 add_lista_aberta(x_atual-1, y_atual-1);
                 Serial.println("estado em RAISE");
              }
              else{
                add_lista_aberta(x_atual-1, y_atual-1);    
                Serial.println("LOWER"); 
              }     
           // }
          }

          if((x_atual+1)==i && ((y_atual-1)==j) && matriz[x_atual+1][y_atual-1].indice != 2){
           //if(matriz[i][j].h < matriz[x_atual][y_atual].k){ //se esse vizinho tem um h menor que o atual_robo.k
             // matriz[x_atual][y_atual].pai =  matriz[i][j].nome; //pai aponta pra esse vizinho.
            //}
           // else{
              if(matriz[i][j].pai == matriz[x_atual][y_atual].nome){
                 matriz[i][j].h =  255;
                 add_lista_aberta(x_atual+1, y_atual-1);
       
                 Serial.println("estado em RAISE");
              }
              else{
                add_lista_aberta(x_atual+1, y_atual-1);       
                Serial.println("LOWER"); 
              }                                  
           // }
          }         
        }   
      }       
    }
    else{
      if(matriz[x_atual][y_atual].k == matriz[x_atual][y_atual].h){ // Como K=H
        //verifica os nós adjacentes
        for(i=0;i<num_linhas;i++){
          for(j=0;j<num_colunas;j++){
            
            if((x_atual-1)==i && (y_atual==j) && matriz[x_atual-1][y_atual].indice !=2){ 
              if(matriz[i][j].h == 255){ 
                matriz[i][j].pai =  matriz[x_atual][y_atual].nome; //vizinho aponta para o atual.
                matriz[i][j].h = matriz[x_atual][y_atual].h + 1; //LOWER
                matriz[i][j].k = matriz[i][j].h;
                add_lista_aberta(x_atual-1, y_atual);
              }
            }
            if((x_atual)==i && ((y_atual+1)==j) && matriz[x_atual][y_atual+1].indice!=2){
              if(matriz[i][j].h == 255){ 
                matriz[i][j].pai =  matriz[x_atual][y_atual].nome; //vizinho aponta para o atual.
                matriz[i][j].h = matriz[x_atual][y_atual].h + 1; //LOWER
                matriz[i][j].k = matriz[i][j].h;
                add_lista_aberta(x_atual, y_atual+1);              
              }       
            }
            if((x_atual+1)==i && (y_atual==j) && matriz[x_atual+1][y_atual].indice!=2){
              if(matriz[i][j].h == 255){
                matriz[i][j].pai =  matriz[x_atual][y_atual].nome; //vizinho aponta para o atual.
                matriz[i][j].h = matriz[x_atual][y_atual].h + 1; //LOWER
                matriz[i][j].k = matriz[i][j].h;
                add_lista_aberta(x_atual+1, y_atual);           
              }
            }
            if((x_atual)==i && ((y_atual-1)==j) && matriz[x_atual][y_atual-1].indice!=2){
              if(matriz[i][j].h == 255){ 
                matriz[i][j].pai =  matriz[x_atual][y_atual].nome; //vizinho aponta para o atual.
                matriz[i][j].h = matriz[x_atual][y_atual].h + 1; //LOWER
                matriz[i][j].k = matriz[i][j].h;
                add_lista_aberta(x_atual, y_atual-1);           
              }
            }
            if((x_atual-1)==i && ((y_atual+1)==j) && matriz[x_atual-1][y_atual+1].indice!=2){
              if(matriz[i][j].h == 255){ 
                matriz[i][j].pai =  matriz[x_atual][y_atual].nome; //vizinho aponta para o atual.
                matriz[i][j].h = matriz[x_atual][y_atual].h + 1.4; //LOWER
                matriz[i][j].k = matriz[i][j].h;
                add_lista_aberta(x_atual-1, y_atual+1);    
              }        
            }
            if((x_atual+1)==i && ((y_atual+1)==j) && matriz[x_atual+1][y_atual+1].indice!=2){
              if(matriz[i][j].h == 255){ 
                matriz[i][j].pai =  matriz[x_atual][y_atual].nome; ////vizinho aponta para o atual.
                matriz[i][j].h = matriz[x_atual][y_atual].h + 1.4; //LOWER
                matriz[i][j].k = matriz[i][j].h;
                add_lista_aberta(x_atual+1, y_atual+1);           
              }
            }
            if((x_atual-1)==i && ((y_atual-1)==j) && matriz[x_atual-1][y_atual-1].indice!=2){
              if(matriz[i][j].h == 255){ 
                matriz[i][j].pai =  matriz[x_atual][y_atual].nome; ////vizinho aponta para o atual.
                matriz[i][j].h = matriz[x_atual][y_atual].h + 1.4; //LOWER
                matriz[i][j].k = matriz[i][j].h;
                add_lista_aberta(x_atual-1, y_atual-1);           
              }
            }
            if((x_atual+1)==i && ((y_atual-1)==j) && matriz[x_atual+1][y_atual-1].indice!=2){
              if(matriz[i][j].h == 255){ 
                matriz[i][j].pai =  matriz[x_atual][y_atual].nome; //vizinho aponta para o atual.
                matriz[i][j].h = matriz[x_atual][y_atual].h + 1.4; //LOWER
                matriz[i][j].k = matriz[i][j].h;
                add_lista_aberta(x_atual+1, y_atual-1);
                           
              }
            }                                                
          } 
        }
      }
    }
      
    matriz[x_atual][y_atual].indice=4; //depois que recalculou tudos os nós adjacentes, indice recebe 4.
  
    imprimir_matriz_h();  
    lista_fechada[it_fechada] = lista_aberta[primeiro];
    it_fechada++;
    primeiro++;
    insertion_sort(primeiro, ultimo);
    
    busca_celula_analisar();

    //depois que calculou os custos dos vizinhos, esta posição não poderá mais ser primeira da lista aberta, pois este nó já foi analisado.  
       
    imprime_lista_aberta(primeiro); 
    delay(3000);
    while (matriz[x_atual][y_atual].indice == 4){
      if(matriz[x_atual][y_atual].indice == 4){
        //se sim retira da lista aberta, senão pode calcular os custos dos nós adjacentes.
        lista_fechada[it_fechada] = lista_aberta[primeiro];
        it_fechada++;
        primeiro++;
        //não precisa de insertionsort pois não a função não realizou nenhum cálculo a mais.
        busca_celula_analisar();
        Serial.println("posição INválida");
      }
      else{
        Serial.println("posição válida"); //primeira vez na primeira posição da lista aberta, pode calcular os custos dos nós adjacentes.
      }
    }
    imprime_lista_aberta(primeiro); 
    
  }                  
}

void mover(){
  byte m;

  /*Adicionei um obstáculo no ambiente, na posição (3,3).*/
  matriz[3][3].indice = 2;
  Serial.print("Posição obstáculo:");
  Serial.println(matriz[3][3].indice);
  
   for(m=0;m<contador_movimento;m++){
    if(movimento_robo[m] == 'b'){
      //antes de o robô ir pra posição superior direita, verifica se tem um obstáculo.
      if(matriz[x_atual+1][y_atual].indice != 2){ //se não tiver um objeto.
        Serial.println("nó de baixo, robô vai pra frente.");
        x_atual = x_atual + 1;
        y_atual = y_atual;
        //frente();
        //parar();            
      }
      else{ //se tem um obstáculo, precisa atualizar os valores da matriz, para fazer o desvio.
        Serial.println("Para o robô e atualiza a tabela.");            
        x_robo = x_atual; //salva a posição em que o robô achou o obstáculo.
        y_robo = y_atual;
        
        x_atual = x_atual - 1; //a posição atual é agora a posição que está o obstáculo.
        y_atual = y_atual + 1;

        //salva a posição do obstáculo, pois será usada posteriormente.
        obstaculo_x = x_atual;
        obstaculo_y = y_atual; 
        
        atualiza_matriz();  
               
        x_atual = x_robo;
        y_atual = y_robo;
        iniciar_movimento_robo();
        melhor_caminho();                      
      }                          
    }
    else{
      if(movimento_robo[m] == 'c'){
        if(matriz[x_atual-1][y_atual].indice != 2){ //se não tiver um objeto.
          Serial.println("Cima.");
          x_atual = x_atual - 1;
          y_atual = y_atual;
          //tras();
          //parar();            
        }
        else{ //se tem um obstáculo, precisa atualizar os valores da matriz, para fazer o desvio.
          Serial.println("Para o robô e atualiza a tabela.");               
          x_robo = x_atual; //salva a posição em que o robô achou o obstáculo.
          y_robo = y_atual;
          
          x_atual = x_atual - 1; //a posição atual é agora a posição que está o obstáculo.
          y_atual = y_atual + 1;

          //salva a posição do obstáculo, pois será usada posteriormente.
          obstaculo_x = x_atual;
          obstaculo_y = y_atual; 
          
          atualiza_matriz();  //envia como parâmetro a posição do robô.
               
          x_atual = x_robo;
          y_atual = y_robo;
          iniciar_movimento_robo();
          melhor_caminho();            
        }         
      }
      else{
        if(movimento_robo[m] == 'd'){
           //antes de o robô ir pra posição superior direita, verifica se tem um obstáculo.
          if(matriz[x_atual][y_atual+1].indice != 2){ //se não tiver um objeto.
            Serial.println("direita");
            x_atual = x_atual;
            y_atual = y_atual + 1;
            //direita();
            //parar();             
          }
          else{ //se tem um obstáculo, precisa atualizar os valores da matriz, para fazer o desvio.
              Serial.println("Para o robô e atualiza a tabela.");
             
              x_robo = x_atual; //salva a posição em que o robô achou o obstáculo.
              y_robo = y_atual;

              x_atual = x_atual - 1; //a posição atual é agora a posição que está o obstáculo.
              y_atual = y_atual + 1;

              //salva a posição do obstáculo, pois será usada posteriormente.
              obstaculo_x = x_atual;
              obstaculo_y = y_atual; 
              
              atualiza_matriz();  //envia como parâmetro a posição do robô.
           
              x_atual = x_robo;
              y_atual = y_robo;
              Serial.println("Chama novamente a função que procura pelo melhor caminho.");
              iniciar_movimento_robo();
              melhor_caminho();              
          }  
        }
        else{
          if(movimento_robo[m] == 'e'){
             //antes de o robô ir pra posição superior direita, verifica se tem um obstáculo.
            if(matriz[x_atual][y_atual-1].indice != 2){ //se não tiver um objeto.
              Serial.println("esquerda");
              x_atual = x_atual;
              y_atual = y_atual-1;
              //esquerda();
              //parar();              
            }
            else{ //se tem um obstáculo, precisa atualizar os valores da matriz, para fazer o desvio.
              Serial.println("Para o robô e atualiza a tabela.");             
              x_robo = x_atual; //salva a posição em que o robô achou o obstáculo.
              y_robo = y_atual;
              
              x_atual = x_atual - 1; //a posição atual é agora a posição que está o obstáculo.
              y_atual = y_atual + 1;

              //salva a posição do obstáculo, pois será usada posteriormente.
              obstaculo_x = x_atual;
              obstaculo_y = y_atual; 
              
              atualiza_matriz();  //envia como parâmetro a posição do robô.
                
              x_atual = x_robo;
              y_atual = y_robo;
              iniciar_movimento_robo();
              melhor_caminho();              
            }            
          } 
          else{
            if(movimento_robo[m] == 'w'){              
              //antes de o robô ir pra posição superior direita, verifica se tem um obstáculo.
              if(matriz[x_atual-1][y_atual+1].indice != 2){ //verifica se no nó adjacente tem um obstáculo, se tiver atualiza a matriz.
                Serial.println("superior direita");
                x_atual = x_atual - 1; 
                y_atual = y_atual + 1;                                
                //direita(); //celula da direita depois pra celula de cima
                //parar(); 
                //tras();
                //parar();                
              }
              else{ //se tem um obstáculo, precisa atualizar os valores da matriz, para fazer o desvio.
                Serial.println("Para o robô e atualiza a tabela.");
               
                x_robo = x_atual; //salva a posição em que o robô achou o obstáculo.
                y_robo = y_atual;
                
                x_atual = x_atual - 1; //a posição atual é agora a posição que está o obstáculo.
                y_atual = y_atual + 1;

                //salva a posição do obstáculo, pois será usada posteriormente.
                obstaculo_x = x_atual;
                obstaculo_y = y_atual; 
                
                atualiza_matriz();  //envia como parâmetro a posição do robô.
              
                x_atual = x_robo;
                y_atual = y_robo;
                iniciar_movimento_robo();
                melhor_caminho();                               
              }            
            }       
            else{
              if(movimento_robo[m] == 'x'){              
                if(matriz[x_atual+1][y_atual+1].indice != 2){ //se não tiver um objeto.
                  Serial.println("inferior direita");
                  x_atual = x_atual + 1;
                  y_atual = y_atual + 1;
                  //frente(); //celula da direita depois pra celula de baixo
                  //parar();                 
                  //direita(); 
                  //parar();               
                }
                else{ //se tem um obstáculo, precisa atualizar os valores da matriz, para fazer o desvio.
                  Serial.println("Para o robô e atualiza a tabela.");
                  //delay(25000);               
                  x_robo = x_atual; //salva a posição em que o robô achou o obstáculo.
                  y_robo = y_atual;
                  
                  x_atual = x_atual - 1; //a posição atual é agora a posição que está o obstáculo.
                  y_atual = y_atual + 1;
  
                  //salva a posição do obstáculo, pois será usada posteriormente.
                  obstaculo_x = x_atual;
                  obstaculo_y = y_atual; 
                  
                  atualiza_matriz();  //envia como parâmetro a posição do robô.
                
                  x_atual = x_robo;
                  y_atual = y_robo;
                  iniciar_movimento_robo();
                  melhor_caminho();          
                }            
              }
              else{
                if(movimento_robo[m] == 'y'){
                  if(matriz[x_atual-1][y_atual-1].indice != 2){ //se não tiver um objeto.
                    Serial.println("superior esquerda");                    
                    x_atual = x_atual - 1;
                    y_atual = y_atual - 1;
                    //esquerda(); //celula da esquerda depois pra celula de cima
                    //parar(); 
                    //tras();
                    //para();              
                  }
                  else{ //se tem um obstáculo, precisa atualizar os valores da matriz, para fazer o desvio.
                      Serial.println("Para o robô e atualiza a tabela.");
             
                      x_robo = x_atual; //salva a posição em que o robô achou o obstáculo.
                      y_robo = y_atual;
                      
                      x_atual = x_atual - 1; //a posição atual é agora a posição que está o obstáculo.
                      y_atual = y_atual + 1;
      
                      //salva a posição do obstáculo, pois será usada posteriormente.
                      obstaculo_x = x_atual;
                      obstaculo_y = y_atual; 
                      
                      atualiza_matriz();  //envia como parâmetro a posição do robô.
                   
                      x_atual = x_robo;
                      y_atual = y_robo;
                      iniciar_movimento_robo();
                      melhor_caminho();            
                  }                               
                }
                else{
                  if(movimento_robo[m] == 'z'){

                    if(matriz[x_atual+1][y_atual-1].indice != 2){ //se não tiver um objeto.
                      Serial.println("inferior esquerda");
                      x_atual = x_atual + 1;
                      y_atual = y_atual - 1;
                      //esquerda();
                      //para();
                      //frente();
                      //para();             
                    }
                    else{ //se tem um obstáculo, precisa atualizar os valores da matriz, para realizar o desvio.
                        Serial.println("Para o robô e atualiza a tabela.");
                        x_robo = x_atual; //salva a posição em que o robô achou o obstáculo.
                        y_robo = y_atual;
                     
                        x_atual = x_atual - 1; //a posição atual é agora a posição que está o obstáculo.
                        y_atual = y_atual + 1;
        
                        //salva a posição do obstáculo, pois será usada posteriormente.
                        obstaculo_x = x_atual;
                        obstaculo_y = y_atual; 
                        
                        atualiza_matriz(); 
               
                        x_atual = x_robo;
                        y_atual = y_robo;
                        iniciar_movimento_robo();
                        melhor_caminho();               
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

void iniciar_movimento_robo(){
  int i;
  for(i=0;i<(num_linhas*num_colunas);i++){
    movimento_robo[i] = ' ';
  }
}

void melhor_caminho(){
  byte i, j;
 
  for(i=0;i<num_linhas;i++){ 
    for(j=0;j<num_colunas;j++){
      if(matriz[x_atual][y_atual].pai==matriz[i][j].nome){ //se o nome da celula for igual ao pai da celula atual
        
        if((x_atual-1)==i && y_atual==j){
          movimento_robo[contador_movimento++]='c';
          x_atual=i;
          y_atual=j;
          
          if(x_atual==x_fim && y_atual==y_fim){
            x_atual = x_inicio;
            y_atual = y_inicio;
           
            mover();
                          
            tempo = millis();
            Serial.println(tempo);
            delay(90000);
          }
          melhor_caminho(); //x_atual, y_atual
        }
        else{
          if((x_atual+1)==i && y_atual==j){          
            movimento_robo[contador_movimento++]='b';
            x_atual=i;
            y_atual=j;
            
            if(x_atual==x_fim && y_atual==y_fim){
              x_atual = x_inicio;
              y_atual = y_inicio;
             
              mover();
                            
              tempo = millis();
              Serial.println(tempo);
              delay(90000);
            }
            melhor_caminho();
          }
          else{
            if(x_atual==i && (y_atual-1)==j){
              movimento_robo[contador_movimento++]='e'; 
              x_atual=i;
              y_atual=j;

              if(x_atual==x_fim && y_atual==y_fim){
                x_atual = x_inicio;
                y_atual = y_inicio;
               
                mover();
                              
                tempo = millis();

                Serial.println(tempo);
                delay(90000);
              }
              melhor_caminho();
            }
            else{
              if(x_atual==i && (y_atual+1)==j){
                movimento_robo[contador_movimento++]='d';
                x_atual=i;
                y_atual=j;
  
                if(x_atual==x_fim && y_atual==y_fim){
                  x_atual = x_inicio;
                  y_atual = y_inicio;
                 
                  mover();
                                
                  tempo = millis();
                  Serial.println(tempo);
                  delay(90000);
                }
                melhor_caminho();    
              }
              else{
                if(i==(x_atual-1) && j==(y_atual+1)){ //celula superior direita
                  movimento_robo[contador_movimento++]='w';
                  x_atual=i;
                  y_atual=j;
  
                  if(x_atual==x_fim && y_atual==y_fim){
                    x_atual = x_inicio;
                    y_atual = y_inicio;
                   
                    mover();
                                  
                    tempo = millis();
                    Serial.println(tempo);
                    delay(90000);
                  }
                  melhor_caminho();
                }
                else{
                  if(i==(x_atual+1) && j==(y_atual+1)){ //celula inferior direita
                    movimento_robo[contador_movimento++]='x';
                    x_atual=i;
                    y_atual=j;
  
                    if(x_atual==x_fim && y_atual==y_fim){
                      x_atual = x_inicio;
                      y_atual = y_inicio;
                     
                      mover();
                                    
                      tempo = millis();
                      Serial.println(tempo);
                      delay(90000);
                    }
                    melhor_caminho();
                  }
                  else{
                    if(i==(x_atual-1) && j==(y_atual-1)){ //celula superior esquerda
                      movimento_robo[contador_movimento++]='y';
                      x_atual=i;
                      y_atual=j;
  
                      if(x_atual==x_fim && y_atual==y_fim){
                        x_atual = x_inicio;
                        y_atual = y_inicio;
                       
                        mover();
                                      
                        tempo = millis();
                        Serial.println(tempo);
                        delay(90000);
                      }
                      melhor_caminho();
                    }
                    else{
                      
                      if(i==(x_atual+1) && j==(y_atual-1)){ //celula inferior esquerda
                        movimento_robo[contador_movimento++]='z';
                        x_atual=i;
                        y_atual=j;
  
                        if(x_atual==x_fim && y_atual==y_fim){
                          x_atual = x_inicio;
                          y_atual = y_inicio;
                         
                          mover();
                                        
                          tempo = millis();
                          Serial.println(tempo);
                          delay(90000);
                        }
                        melhor_caminho();                     
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

void detectar_vizinhos(){
  byte i, j, n;
  //nao pode calcular o custo de quem esta na lista aberta e fechada
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      
      if(i==(x_atual-1) && j==y_atual && matriz[i][j].tag==0){ //se for a celula de cima e for uma celula nova.
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
        Serial.println("Chama a função melhor caminho.");
        melhor_caminho();
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

void imprimir_matriz_pai(){
  byte i, j;
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      Serial.print(matriz[i][j].pai);
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
  for(i=primeiro;i<(num_linhas*num_colunas)+20;i++){
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
    //imprimir_matriz_indice(); 
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
  detectar_vizinhos();
  lista_fechada[it_fechada] = lista_aberta[primeiro];
  it_fechada++;
  primeiro++;
  insertion_sort(primeiro, ultimo);
  busca_celula_analisar();
  if(x_atual==x_inicio && y_atual==y_inicio){
    //retira da lista aberta, mas detecta os vizinhos normal e calcula os custos normal.
      lista_fechada[it_fechada] = lista_aberta[primeiro];
      it_fechada++;
      primeiro++;
  }
  Serial.println("");
}

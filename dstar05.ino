unsigned long tempo;
#define num_linhas 4
#define num_colunas 5
byte x_inicio = 0;
byte y_inicio = 0;
byte x_fim = 3;
byte y_fim = 4;

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

CELULA matriz[4][5];
CELULA *lista_aberta = (CELULA*)malloc(((num_linhas*num_colunas)+20)*sizeof(CELULA));
//CELULA lista_aberta[num_linhas*num_colunas+20];
byte it_aberta=0;
//CELULA *lista_fechada = (CELULA*)malloc(((num_linhas*num_colunas))*sizeof(CELULA));
CELULA lista_fechada[num_linhas*num_colunas];
byte it_fechada=0;
byte primeiro=0;
short ultimo=-1;
byte x_atual;
byte y_atual;
char movimento_robo[((num_linhas*num_colunas)+20)]; //'b' baixo, 'c' cima, 'd' direita, 'e' esquerda, 'w' superior direita, 'x' inferior direita, 'y' superior esquerda, 'z' inferior esquerda.
byte contador_movimento=0;
byte primeira_passada=1;
byte x_robo; 
byte y_robo;
byte obstaculo_x;
byte obstaculo_y;
byte x_pos_robo;
byte y_pos_robo;
byte replanejamento = 0;

void iniciar_matriz(){
  byte i, j;
  byte cont=1;
  byte x_pos_robo, y_pos_robo;
  
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      if((i==0 && j==2) || (i==2 && j==2)){    
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
  for(i=primeiro;i<(num_linhas*num_colunas)+20;i++){
    if(matriz[m][n].nome != lista_aberta[i].nome){
      flag=0;
    }
    else{
      flag=1;
      lista_aberta[i] = matriz[m][n];
      i= ((num_linhas*num_colunas)+20); //se tem um valor na lista aberta sai do for.
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


  imprime_lista_aberta(primeiro);
  
  matriz[x_atual][y_atual].h = 255; //a posição atual é um objeto
  matriz[x_atual][y_atual].indice = 2;
  Serial.println(x_atual);
  Serial.println(y_atual);
  imprimir_matriz_h();
  //add na lista aberta o nó em que está o obstáculo e seus nós adjacentes.


  add_lista_aberta(x_atual, y_atual);
  
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      
      if(i==(x_atual-1)&& j==y_atual){
        add_lista_aberta(x_atual-1, y_atual);
      }
      if(i==x_atual && j==(y_atual+1)){
        add_lista_aberta(x_atual, y_atual+1);
      }      
      if(i==(x_atual+1)&& j==y_atual){
        add_lista_aberta(x_atual+1, y_atual);
      }
      if(i==x_atual && j==(y_atual-1)){
        add_lista_aberta(x_atual, y_atual-1);
      }
      if(i==(x_atual-1)&& j==(y_atual+1)){
        add_lista_aberta(x_atual-1, y_atual+1);
      }
      if(i==(x_atual+1)&& j==(y_atual+1)){
        add_lista_aberta(x_atual+1, y_atual+1);
      }
      if(i==(x_atual-1)&& j==(y_atual-1)){
        add_lista_aberta(x_atual-1, y_atual-1);
      }
      if(i==(x_atual+1)&& j==(y_atual-1)){
        add_lista_aberta(x_atual+1, y_atual-1);
      }      
    }
  }
  
  
  
  
  //add_lista_aberta(x_atual, y_atual);
  /*
  byte flag = 0;
  for(i=primeiro;i<num_linhas*num_colunas;i++){
    if(matriz[x_atual][y_atual].nome != lista_aberta[i].nome){
    flag=0;
    }
    else{
      flag=1;
      lista_aberta[i] = matriz[x_atual][y_atual];
      i= num_linhas*num_colunas; //se tem um valor na lista aberta sai do for.
    }
  }

  if(flag == 0){
    //Serial.println("Este valor não está na lista aberta, portanto add ele.");
    lista_aberta[it_aberta] = matriz[x_atual][y_atual];//add obstaculo e nós adjacentes na lista aberta, para ordena-los depois.      
    it_aberta++;
    ultimo++;
    matriz[x_atual][y_atual].tag=1; //analisado de novo, ou seja permanece com 1.
  }
  else{
    //Serial.println("Este valor já esta na lista aberta, portanto não adiciona, apenas atualiza, se precisar");
  }
  //add_lista_aberta(x_atual-1, y_atual);
  flag = 0;
  for(i=primeiro;i<num_linhas*num_colunas;i++){
    if(matriz[x_atual-1][y_atual].nome != lista_aberta[i].nome){
    flag=0;
    }
    else{
      flag=1;
      lista_aberta[i] = matriz[x_atual-1][y_atual];
      i= num_linhas*num_colunas; //se tem um valor na lista aberta sai do for.
    }
  }

  if(flag == 0){
    //Serial.println("Este valor não está na lista aberta, portanto add ele.");
    lista_aberta[it_aberta] = matriz[x_atual-1][y_atual];//add obstaculo e nós adjacentes na lista aberta, para ordena-los depois.      
    it_aberta++;
    ultimo++;
    matriz[x_atual-1][y_atual].tag=1; //analisado de novo, ou seja permanece com 1.
  }
  else{
    //Serial.println("Este valor já esta na lista aberta, portanto não adiciona, apenas atualiza, se precisar");
  }
 
  //add_lista_aberta(x_atual, y_atual+1);
  flag = 0;
  for(i=primeiro;i<num_linhas*num_colunas;i++){
    if(matriz[x_atual][y_atual+1].nome != lista_aberta[i].nome){
    flag=0;
    }
    else{
      flag=1;
      lista_aberta[i] = matriz[x_atual][y_atual+1];
      i= num_linhas*num_colunas; //se tem um valor na lista aberta sai do for.
    }
  }

  if(flag == 0){
    //Serial.println("Este valor não está na lista aberta, portanto add ele.");
    lista_aberta[it_aberta] = matriz[x_atual][y_atual+1];//add obstaculo e nós adjacentes na lista aberta, para ordena-los depois.      
    it_aberta++;
    ultimo++;
    matriz[x_atual][y_atual+1].tag=1; //analisado de novo, ou seja permanece com 1.
  }
  else{
    //Serial.println("Este valor já esta na lista aberta, portanto não adiciona, apenas atualiza, se precisar");
  }
  //add_lista_aberta(x_atual+1, y_atual);
  flag = 0;
  for(i=primeiro;i<num_linhas*num_colunas;i++){
    if(matriz[x_atual+1][y_atual].nome != lista_aberta[i].nome){
    flag=0;
    }
    else{
      flag=1;
      lista_aberta[i] = matriz[x_atual+1][y_atual];
      i= num_linhas*num_colunas; //se tem um valor na lista aberta sai do for.
    }
  }

  if(flag == 0){
    //Serial.println("Este valor não está na lista aberta, portanto add ele.");
    lista_aberta[it_aberta] = matriz[x_atual+1][y_atual];//add obstaculo e nós adjacentes na lista aberta, para ordena-los depois.      
    it_aberta++;
    ultimo++;
    matriz[x_atual+1][y_atual].tag=1; //analisado de novo, ou seja permanece com 1.
  }
  else{
    //Serial.println("Este valor já esta na lista aberta, portanto não adiciona, apenas atualiza, se precisar");
  }
  //add_lista_aberta(x_atual, y_atual-1);
  flag = 0;
  for(i=primeiro;i<num_linhas*num_colunas;i++){
    if(matriz[x_atual][y_atual-1].nome != lista_aberta[i].nome){
    flag=0;
    }
    else{
      flag=1;
      lista_aberta[i] = matriz[x_atual][y_atual-1];
      i= num_linhas*num_colunas; //se tem um valor na lista aberta sai do for.
    }
  }

  if(flag == 0){
    //Serial.println("Este valor não está na lista aberta, portanto add ele.");
    lista_aberta[it_aberta] = matriz[x_atual][y_atual-1];//add obstaculo e nós adjacentes na lista aberta, para ordena-los depois.      
    it_aberta++;
    ultimo++;
    matriz[x_atual][y_atual-1].tag=1; //analisado de novo, ou seja permanece com 1.
  }
  else{
    //Serial.println("Este valor já esta na lista aberta, portanto não adiciona, apenas atualiza, se precisar");
  }
  //add_lista_aberta(x_atual-1, y_atual+1);
  flag = 0;
  for(i=primeiro;i<num_linhas*num_colunas;i++){
    if(matriz[x_atual-1][y_atual+1].nome != lista_aberta[i].nome){
    flag=0;
    }
    else{
      flag=1;
      lista_aberta[i] = matriz[x_atual-1][y_atual+1];
      i= num_linhas*num_colunas; //se tem um valor na lista aberta sai do for.
    }
  }

  if(flag == 0){
    //Serial.println("Este valor não está na lista aberta, portanto add ele.");
    lista_aberta[it_aberta] = matriz[x_atual-1][y_atual+1];//add obstaculo e nós adjacentes na lista aberta, para ordena-los depois.      
    it_aberta++;
    ultimo++;
    matriz[x_atual-1][y_atual+1].tag=1; //analisado de novo, ou seja permanece com 1.
  }
  else{
    //Serial.println("Este valor já esta na lista aberta, portanto não adiciona, apenas atualiza, se precisar");
  }
  //add_lista_aberta(x_atual+1, y_atual+1);
  flag = 0;
  for(i=primeiro;i<num_linhas*num_colunas;i++){
    if(matriz[x_atual+1][y_atual+1].nome != lista_aberta[i].nome){
    flag=0;
    }
    else{
      flag=1;
      lista_aberta[i] = matriz[x_atual+1][y_atual+1];
      i= num_linhas*num_colunas; //se tem um valor na lista aberta sai do for.
    }
  }

  if(flag == 0){
    //Serial.println("Este valor não está na lista aberta, portanto add ele.");
    lista_aberta[it_aberta] = matriz[x_atual+1][y_atual+1];//add obstaculo e nós adjacentes na lista aberta, para ordena-los depois.      
    it_aberta++;
    ultimo++;
    matriz[x_atual+1][y_atual+1].tag=1; //analisado de novo, ou seja permanece com 1.
  }
  else{
    //Serial.println("Este valor já esta na lista aberta, portanto não adiciona, apenas atualiza, se precisar");
  }
  //add_lista_aberta(x_atual-1, y_atual-1);
  flag = 0;
  for(i=primeiro;i<num_linhas*num_colunas;i++){
    if(matriz[x_atual-1][y_atual-1].nome != lista_aberta[i].nome){
    flag=0;
    }
    else{
      flag=1;
      lista_aberta[i] = matriz[x_atual-1][y_atual-1];
      i= num_linhas*num_colunas; //se tem um valor na lista aberta sai do for.
    }
  }

  if(flag == 0){
    //Serial.println("Este valor não está na lista aberta, portanto add ele.");
    lista_aberta[it_aberta] = matriz[x_atual-1][y_atual-1];//add obstaculo e nós adjacentes na lista aberta, para ordena-los depois.      
    it_aberta++;
    ultimo++;
    matriz[x_atual-1][y_atual-1].tag=1; //analisado de novo, ou seja permanece com 1.
  }
  else{
    //Serial.println("Este valor já esta na lista aberta, portanto não adiciona, apenas atualiza, se precisar");
  }
  //add_lista_aberta(x_atual+1, y_atual-1);
  flag = 0;
  for(i=primeiro;i<num_linhas*num_colunas;i++){
    if(matriz[x_atual+1][y_atual-1].nome != lista_aberta[i].nome){
    flag=0;
    }
    else{
      flag=1;
      lista_aberta[i] = matriz[x_atual+1][y_atual-1];
      i= num_linhas*num_colunas; //se tem um valor na lista aberta sai do for.
    }
  }

  if(flag == 0){
    //Serial.println("Este valor não está na lista aberta, portanto add ele.");
    lista_aberta[it_aberta] = matriz[x_atual+1][y_atual-1];//add obstaculo e nós adjacentes na lista aberta, para ordena-los depois.      
    it_aberta++;
    ultimo++;
    matriz[x_atual+1][y_atual-1].tag=1; //analisado de novo, ou seja permanece com 1.
  }
  else{
    //Serial.println("Este valor já esta na lista aberta, portanto não adiciona, apenas atualiza, se precisar");
  }
*/
  //analisa lista aberta
  insertion_sort(primeiro, ultimo);
  imprime_lista_aberta(primeiro);  
  busca_celula_analisar();
  Serial.println(matriz[x_atual][y_atual].nome);
  imprime_lista_aberta(primeiro); 
  imprimir_matriz_k();
  imprimir_matriz_pai();
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
  matriz[1][2].indice = 2;  
  //Serial.print("Posição obstáculo:");
  //Serial.println(matriz[0][2].indice);
  
   for(m=0;m<((num_linhas*num_colunas)+20);m++){
    if(movimento_robo[m] == 'b'){
      Serial.println(x_atual);
      Serial.println(y_atual);
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
        
        x_atual = x_atual + 1; //a posição atual é agora a posição que está o obstáculo.
        y_atual = y_atual;

        //salva a posição do obstáculo, pois será usada posteriormente.
        obstaculo_x = x_atual;
        obstaculo_y = y_atual; 
        
        
        atualiza_matriz();  
               
        x_atual = x_robo;
        y_atual = y_robo;
        iniciar_movimento_robo();
        replanejamento=1;
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
          y_atual = y_atual;

          //salva a posição do obstáculo, pois será usada posteriormente.
          obstaculo_x = x_atual;
          obstaculo_y = y_atual; 
          
          atualiza_matriz();  //envia como parâmetro a posição do robô.
               
          x_atual = x_robo;
          y_atual = y_robo;
          iniciar_movimento_robo();
          replanejamento=1;
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

              x_atual = x_atual; //a posição atual é agora a posição que está o obstáculo.
              y_atual = y_atual + 1;

              //salva a posição do obstáculo, pois será usada posteriormente.
              obstaculo_x = x_atual;
              obstaculo_y = y_atual; 

              Serial.println(obstaculo_x);
              Serial.println(obstaculo_y);
              
              atualiza_matriz();  //envia como parâmetro a posição do robô.
           
              x_atual = x_robo;
              y_atual = y_robo;
              
              Serial.println("Chama novamente a função que procura pelo melhor caminho.");
              Serial.println(x_atual);
              Serial.println(y_atual);
              
              imprimir_movimento_robo();
              iniciar_movimento_robo();
              imprimir_movimento_robo();
              imprimir_matriz_pai();
              replanejamento=1;
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
              
              x_atual = x_atual; //a posição atual é agora a posição que está o obstáculo.
              y_atual = y_atual - 1;

              //salva a posição do obstáculo, pois será usada posteriormente.
              obstaculo_x = x_atual;
              obstaculo_y = y_atual; 
              
              atualiza_matriz();  //envia como parâmetro a posição do robô.
                
              x_atual = x_robo;
              y_atual = y_robo;
              iniciar_movimento_robo();
              replanejamento=1;
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
                replanejamento=1;
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
                  
                  x_atual = x_atual + 1; //a posição atual é agora a posição que está o obstáculo.
                  y_atual = y_atual + 1;
  
                  //salva a posição do obstáculo, pois será usada posteriormente.
                  obstaculo_x = x_atual;
                  obstaculo_y = y_atual; 
                  
                  atualiza_matriz();  //envia como parâmetro a posição do robô.
                
                  x_atual = x_robo;
                  y_atual = y_robo;
                  iniciar_movimento_robo();
                  replanejamento=1;
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
                      y_atual = y_atual - 1;
      
                      //salva a posição do obstáculo, pois será usada posteriormente.
                      obstaculo_x = x_atual;
                      obstaculo_y = y_atual; 
                      
                      atualiza_matriz();  //envia como parâmetro a posição do robô.
                   
                      x_atual = x_robo;
                      y_atual = y_robo;
                      iniciar_movimento_robo();
                      replanejamento=1;
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
                     
                        x_atual = x_atual + 1; //a posição atual é agora a posição que está o obstáculo.
                        y_atual = y_atual - 1;
        
                        //salva a posição do obstáculo, pois será usada posteriormente.
                        obstaculo_x = x_atual;
                        obstaculo_y = y_atual; 
                        
                        atualiza_matriz(); 
               
                        x_atual = x_robo;
                        y_atual = y_robo;
                        iniciar_movimento_robo();
                        replanejamento=1;
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
  for(i=0;i<((num_linhas*num_colunas)+20);i++){
    movimento_robo[i] = 'f';
  }
}

void imprimir_movimento_robo(){
  int i;
  for(i=0;i<((num_linhas*num_colunas)+20);i++){
    Serial.println(movimento_robo[i]);
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
          Serial.println("cima");
          if(x_atual==x_fim && y_atual==y_fim){


            if(replanejamento==1){
              mover(); //a posição atual, é a posição que robô parou.
            }
            else{
              x_atual = x_inicio; //robô começa a percorrer desde o inicio, sem o replanemaneto
              y_atual = y_inicio;
              mover();
            }
            
                          
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
            Serial.println("baixo");
            if(x_atual==x_fim && y_atual==y_fim){
              //x_atual = x_inicio;
              //y_atual = y_inicio;
             
              if(replanejamento==1){
                mover(); //a posição atual, é a posição que robô parou.
              }
              else{
                x_atual = x_inicio; //robô começa a percorrer desde o inicio, sem o replanemaneto
                y_atual = y_inicio;
                mover();
              }
                            
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
              Serial.println("esquerda");
              if(x_atual==x_fim && y_atual==y_fim){
                //x_atual = x_inicio;
                //y_atual = y_inicio;
               
                if(replanejamento==1){
                  mover(); //a posição atual, é a posição que robô parou.
                }
                else{
                  x_atual = x_inicio; //robô começa a percorrer desde o inicio, sem o replanemaneto
                  y_atual = y_inicio;
                  mover();
                }
                              
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
                Serial.println("direita");
                if(x_atual==x_fim && y_atual==y_fim){
                  //x_atual = x_inicio;
                 // y_atual = y_inicio;
                 
                  if(replanejamento==1){
                    mover(); //a posição atual, é a posição que robô parou.
                  }
                  else{
                    x_atual = x_inicio; //robô começa a percorrer desde o inicio, sem o replanemaneto
                    y_atual = y_inicio;
                    mover();
                  }
                                
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
                  Serial.println("superior direita");
                  if(x_atual==x_fim && y_atual==y_fim){
                    //x_atual = x_inicio;
                    //y_atual = y_inicio;
                   
                    if(replanejamento==1){
                      mover(); //a posição atual, é a posição que robô parou.
                    }
                    else{
                      x_atual = x_inicio; //robô começa a percorrer desde o inicio, sem o replanemaneto
                      y_atual = y_inicio;
                      mover();
                    }
                                  
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
                    Serial.println("inferior direita");
                    if(x_atual==x_fim && y_atual==y_fim){
                      //x_atual = x_inicio;
                      //y_atual = y_inicio;
                     
                      if(replanejamento==1){
                        mover(); //a posição atual, é a posição que robô parou.
                      }
                      else{
                        x_atual = x_inicio; //robô começa a percorrer desde o inicio, sem o replanemaneto
                        y_atual = y_inicio;
                        mover();
                      }
                                    
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
                      Serial.println("superior esquerda");
                      if(x_atual==x_fim && y_atual==y_fim){
                        //x_atual = x_inicio;
                        //y_atual = y_inicio;
                       
                        if(replanejamento==1){
                          mover(); //a posição atual, é a posição que robô parou.
                        }
                        else{
                          x_atual = x_inicio; //robô começa a percorrer desde o inicio, sem o replanemaneto
                          y_atual = y_inicio;
                          mover();
                        }
                                      
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
                        Serial.println("inferior esquerda");
                        if(x_atual==x_fim && y_atual==y_fim){
                          //x_atual = x_inicio;
                          //y_atual = y_inicio;
                         
                          if(replanejamento==1){
                            mover(); //a posição atual, é a posição que robô parou.
                          }
                          else{
                            x_atual = x_inicio; //robô começa a percorrer desde o inicio, sem o replanemaneto
                            y_atual = y_inicio;
                            mover();
                          }
                                        
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
        iniciar_movimento_robo();
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

void imprimir_matriz_nome(){
  byte i, j;
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      Serial.print(matriz[i][j].nome);
      Serial.print(' ');
    }
    Serial.println(" ");
  }
  Serial.println(" ");  
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

void imprimir_matriz_k(){
  byte i, j;
  for(i=0;i<num_linhas;i++){
    for(j=0;j<num_colunas;j++){
      Serial.print(matriz[i][j].k);
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
    //imprimir_matriz_nome();
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

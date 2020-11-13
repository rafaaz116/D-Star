typedef struct cel{
  byte h;
  byte k;
  byte b; //pai
  byte tag; //1 novo, 2 aberto e 3 fechado
  byte c; //custo do caminho de y pra x 
  byte r; //custo do caminho de y pra x baseado no sensor
}CELULA;

CELULA matrix[6][7]; 

void setup() {
  // put your setup code here, to run once:

}

/********************************
Autora: Amanda Gabriela Val�rio
********************************/

#include <iostream>
#include <string>
#include <string.h>
#include <cctype>
#include <omp.h>

using namespace std;


/*************************************************************************/
//c�digos para cria��o e manipula��o da estrutura de dados Fila encadeada, 
//inspirados no livro Estruturas de Dados, Ziviane


struct TItem{
    string linha;
};

struct TCelula{
    TItem item;
    TCelula *proximo;
};

struct TFila{
    TCelula *frente;
    TCelula *tras;
};

/// Opera��es do TAD Fila
void criarFilaVazia(TFila &f){
   f.frente = new TCelula;
   f.tras = f.frente;
   f.frente->proximo = NULL;
}

bool estaVazia(TFila &f){
    return (f.tras == f.frente);
}

void enfileirar(TItem i, TFila &f){// igual ao inserir da lista por ponteiros...
    f.tras->proximo = new TCelula;
    f.tras = f.tras->proximo;
    f.tras->item = i;
    f.tras->proximo = NULL;
}

void mostrarFila(TFila f){

	TCelula *celAux;

    if (estaVazia(f) == false){
        celAux = f.frente->proximo;

        while (celAux != NULL)
        {
            cout << celAux->item.linha  << endl;
            celAux = celAux->proximo;
        }
    }
}

TItem desenfileirarEPegar(TFila &f){
    TCelula *celAux;
    TItem iAux;

    if (estaVazia(f)){
        iAux.linha = " ";
        return iAux;
    }
    else{
        iAux = f.frente->proximo->item;
        celAux = f.frente;
        f.frente = f.frente->proximo;
        delete celAux;
        return iAux;
    }
}

/************************************************************************************/
int main(){
	
	//variaveis declaradas antes da paraleliza��o s�o usadas por todas as threads
	//enquanto as variaveis declaradas dentro da thread tem um pouco mais de prote��o
	string palavra; //palavra a ser buscada
	int repeticoes = 0;
	//int vetRepet[50]; //vari�vel para debug
	int idProdutor;
	bool finalizado = false, liberado = true;
	
	//criando a fila
	TFila fila;
    TItem iAux;
    criarFilaVazia(fila);
	
	//pegando a palavra para buscar
	cout << "Digite a palavra que voce quer buscar: ";
	cin >> palavra;
	
	//abre arquivo
	FILE *arq;
	arq = fopen("texto.txt", "r");
	if (arq == NULL){
		cout << "Problemas com o arquivo!" << endl;
		return -1;
	}
	
	//se arquivo aberto com sucesso
	cout << "Buscando..." << endl;
	
	//inicia a paralelisa��o
	//quando n�o identifica a quantidade de threads, o computador utiliza a quantidade de processadores f�sicos
	#pragma omp parallel
	{	
		#pragma omp single
		{
			//thread produtora
			idProdutor = omp_get_thread_num();
			//faz a primeira leitura
			char linhaLida[100]; //linha lida do arquivo
			char *result = fgets (linhaLida, 100, arq);
			//entra em um la�o que l� todo o arquivo
			while (result != NULL){
				//adiciona na fila, realizando o controle
				iAux.linha = linhaLida;
				liberado = false;
				//a diretiva critical ajuda a proteger o endere�o de mem�ria
				#pragma omp critical
				enfileirar(iAux, fila);
				liberado = true;
				//le a linha do arquivo
				result = fgets (linhaLida, 100, arq);
			}
			//ao terminar confirma
			finalizado = true;
		}
		
		//threads dos consumidores
		//depois de encontrar a diretiva single, o restante das threads segue
		// para o restante do c�digo, copia entre as threads e executa
		//a verifica��o acontece pelo n�mero do ID
		if (omp_get_thread_num() != idProdutor){
			int posPalavra = 0;
			bool ok = false;
			//enquanto ainda tiver texto para ler e apenas quando a fila conter elementos
			while (finalizado =! true || estaVazia(fila) == false){
				//se a fila n�o estiver sendo usada
				if (liberado == true){
					//desenfileira
					//a diretiva critical ajuda a proteger o endere�o de mem�ria
					#pragma omp critical
					iAux = desenfileirarEPegar(fila);
					//armazena em uma outra vari�vel para evitar bugs
					string bufferLido = iAux.linha;
					//enquanto ainda h� caracteres na linha para ser lido
					for (int pos = 0; pos < bufferLido.length(); pos++){
						//se os caracteres correspondem...
						if (tolower(bufferLido[pos]) == tolower(palavra[posPalavra])){
							//esta pode ser a palavra
							ok = true;
							//ent�o incrementa o index
							posPalavra++;
						} 
						else{
							//se corresponde inteira
							if(ok == true && posPalavra == palavra.length()){
								//vetor para debug do c�digo
								//vetRepet[repeticoes] = omp_get_thread_num();
								//ent�o incrementa o numero de repeti��es
								repeticoes++;
							}
							//reseta o contador, pois ou n�o era correspondente ou j� foi confirmada
							ok = false;
							//reseta o index
							posPalavra = 0;
						}
					}
				}
			}
		}
	}
	
	//exibe a quantidade de vezes que aquela palavra foi encontrada
	cout << "Essa palavra foi encontrada " << repeticoes << " vezes" << endl;
	//codigo para debug
	//cout << idProdutor;
	
	return 0;
}

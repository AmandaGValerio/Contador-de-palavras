/********************************
Autora: Amanda Gabriela Valério
********************************/

#include <iostream>
#include <string>
#include <string.h>
#include <cctype>
#include <omp.h>

using namespace std;


/*************************************************************************/
//códigos para criação e manipulação da estrutura de dados Fila encadeada, 
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

/// Operações do TAD Fila
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
	
	//variaveis declaradas antes da paralelização são usadas por todas as threads
	//enquanto as variaveis declaradas dentro da thread tem um pouco mais de proteção
	string palavra; //palavra a ser buscada
	int repeticoes = 0;
	//int vetRepet[50]; //variável para debug
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
	
	//inicia a paralelisação
	//quando não identifica a quantidade de threads, o computador utiliza a quantidade de processadores físicos
	#pragma omp parallel
	{	
		#pragma omp single
		{
			//thread produtora
			idProdutor = omp_get_thread_num();
			//faz a primeira leitura
			char linhaLida[100]; //linha lida do arquivo
			char *result = fgets (linhaLida, 100, arq);
			//entra em um laço que lê todo o arquivo
			while (result != NULL){
				//adiciona na fila, realizando o controle
				iAux.linha = linhaLida;
				liberado = false;
				//a diretiva critical ajuda a proteger o endereço de memória
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
		// para o restante do código, copia entre as threads e executa
		//a verificação acontece pelo número do ID
		if (omp_get_thread_num() != idProdutor){
			int posPalavra = 0;
			bool ok = false;
			//enquanto ainda tiver texto para ler e apenas quando a fila conter elementos
			while (finalizado =! true || estaVazia(fila) == false){
				//se a fila não estiver sendo usada
				if (liberado == true){
					//desenfileira
					//a diretiva critical ajuda a proteger o endereço de memória
					#pragma omp critical
					iAux = desenfileirarEPegar(fila);
					//armazena em uma outra variável para evitar bugs
					string bufferLido = iAux.linha;
					//enquanto ainda há caracteres na linha para ser lido
					for (int pos = 0; pos < bufferLido.length(); pos++){
						//se os caracteres correspondem...
						if (tolower(bufferLido[pos]) == tolower(palavra[posPalavra])){
							//esta pode ser a palavra
							ok = true;
							//então incrementa o index
							posPalavra++;
						} 
						else{
							//se corresponde inteira
							if(ok == true && posPalavra == palavra.length()){
								//vetor para debug do código
								//vetRepet[repeticoes] = omp_get_thread_num();
								//então incrementa o numero de repetições
								repeticoes++;
							}
							//reseta o contador, pois ou não era correspondente ou já foi confirmada
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

/********************************
Autora: Amanda Gabriela Valério
********************************/

#include <iostream>
#include <string>
#include <string.h>
#include <cctype>
#include <omp.h>

using namespace std;

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
   cout << "Fila criada com sucesso!" << endl;
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

    if (estaVazia(f)){
        cout << "Não há nada para mostrar!" << endl;
    }
    else{
        celAux = f.frente->proximo;

        while (celAux != NULL)
        {
            cout << celAux->item.linha  << endl;
            celAux = celAux->proximo;
        }
        cout << "-----------------------------" << endl;
    }
}

TItem desenfileirarEPegar(TFila &f){
    TCelula *celAux;
    TItem iAux;

    if (estaVazia(f)){
        cout << "Não há itens para desenfileirar!" << endl;
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
	
	string palavra; //palavra a ser buscada
	char linhaLida[100]; //linha lida do arquivo
	int repeticoes = 0;
	int vetRepet[50];
	bool finalizado = false, liberado = true;
	
	//criando a fila
	TFila fila;
    TItem iAux;
    criarFilaVazia(fila);
	
	//pegando a palavra para buscar
	cout << "Digite a palavra que você quer buscar: ";
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
			vetRepet[repeticoes] = omp_get_thread_num();
			char *result = fgets (linhaLida, 100, arq);
			while (result != NULL){
				iAux.linha = linhaLida;
				//adiciona na fila
				liberado = false;
				enfileirar(iAux, fila);
				liberado = true;
				//le a linha do arquivo
				result = fgets (linhaLida, 100, arq);
			}
			finalizado = true;
		}
		
		//função para buscar na string
		int posPalavra = 0;
		bool ok = false;
		while (finalizado =! true || estaVazia(fila) == false){
			if (liberado == true){
				#pragma omp critical
				iAux = desenfileirarEPegar(fila);
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
						if(ok == true && posPalavra == palavra.length()){
							//se corresponde inteira, então incrementa o numero de repetições
							vetRepet[repeticoes] = omp_get_thread_num();
							repeticoes++;
							cout << omp_get_thread_num() << endl;
						}
						//a palavra não era correspondente
						ok = false;
						//reseta o index
						posPalavra = 0;
					}
				}
			}
		}
		printf(" terminei %i\n", omp_get_thread_num());
	}
	
	//exibe a quantidade de vezes que aquela palavra foi encontrada
	cout << "Essa palavra foi encontrada " << repeticoes << " vezes" << endl;
	
	return 0;
}

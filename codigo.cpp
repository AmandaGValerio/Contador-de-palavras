/**********************
Autora: Amanda Gabriela Valério
**********************/

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
	int vetRepet[30];
	string bufferLido;
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
		#pragma omp master 
		{
			//thread produtora
			char *result = fgets (linhaLida, 100, arq);
			while (result != NULL){
				//cout << "entrei" << endl;
				iAux.linha = linhaLida;
				//adiciona na fila
				//iAux.linha = linha;
				liberado = false;
				enfileirar(iAux, fila);
				liberado = true;
				//le a linha do arquivo
				result = fgets (linhaLida, 100, arq);
			}
			finalizado = true;
		}
		
		//cout << "sai" << endl;
		//cout << estaVazia(fila) << true << endl;
		//função para buscar na string
		int posPalavra = 0;
		bool ok = false;
		//cout << "f = " << finalizado << "v = " << estaVazia(fila) << endl;
		while (finalizado =! true || estaVazia(fila) == false){
		if (liberado == true){
		
			#pragma omp critical
			iAux = desenfileirarEPegar(fila);

			bufferLido = iAux.linha;
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
						//cout << omp_get_thread_num() << endl;
					}
					//a palavra não era correspondente
					ok = false;
					//reseta o index
					posPalavra = 0;
				}
			}
		}}
		//cout << "terminei " << omp_get_thread_num() << endl;
		printf(" terminei %i", omp_get_thread_num());
			
	}
	
	//exibe a quantidade de vezes que aquela palavra foi encontrada
	//	mostrarFila(fila);
	cout << "Essa palavra foi encontrada " << repeticoes << " vezes" << endl;
	
	for (int i = 0; i < repeticoes; i++){
		cout << vetRepet[i];
	}
	
	return 0;
}

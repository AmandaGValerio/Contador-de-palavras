/**********************
Autora: Amanda Gabriela Val�rio
**********************/

#include <iostream>
#include <string>
#include <string.h>
#include <cctype>
#include "fila.h"

using namespace std;

/************************************************************************************/
struct TItem{
    string linha;
};

struct TCelula
{
    TItem item;
    TCelula *proximo;
};

struct TFila
{
    TCelula *frente;
    TCelula *tras;
};

/// Opera��es do TAD Fila
void criarFilaVazia(TFila &f)
{
   f.frente = new TCelula;
   f.tras = f.frente;
   f.frente->proximo = NULL;
   cout << "Fila criada com sucesso!" << endl;

}

bool estaVazia(TFila &f)
{
    return (f.tras == f.frente);
}

void enfileirar(TItem i, TFila &f)// igual ao inserir da lista por ponteiros...
{
    f.tras->proximo = new TCelula;
    f.tras = f.tras->proximo;
    f.tras->item = i;
    f.tras->proximo = NULL;
}

void mostrarFila(TFila f){

TCelula *celAux;

    if (estaVazia(f)){
        cout << "N�o h� nada para mostrar!" << endl;
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

void desenfileirar(TFila &f){
    TCelula *celAux;

    if (estaVazia(f)){
        cout << "N�o h� itens para desenfileirar!" << endl;
    }
    else{
        celAux = f.frente;
        f.frente = f.frente->proximo;
        delete celAux;
    }
}

TItem desenfileirarEPegar(TFila &f){
    TCelula *celAux;
    TItem iAux;

    if (estaVazia(f)){
        cout << "N�o h� itens para desenfileirar!" << endl;
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

void converterletras(string &linha){
	//converte todas as letras para minusculo
	for (int i = 0; i < linha.size(); i++){
		linha[i] = tolower(linha[i]);
	}
}

int main(){
	
	string palavra; //palavra a ser buscada
	char linhaLida[100]; //linha lida do arquivo
	int repeticoes = 0;
	
	//pegando a palavra para buscar
	cout << "Digite a palavra que voc� quer buscar: ";
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
	
	char *result;
	do {
		//le a linha do arquivo
		result = fgets (linhaLida, 100, arq);
		string linha = linhaLida;
		
		//exibe na tela
		//trocar por escrever na fila
		cout << linha << endl; 
		
		//colocar td em caixa baixa
		converterletras(linha);
		converterletras(palavra);
		
		//fun��o para buscar na string
		int posPalavra = 0;
		bool ok = false;
		//enquanto ainda h� caracteres na linha para ser lido
		for (int pos = 0; pos < linha.length(); pos++){
			//se os caracteres correspondem...
			if (linha[pos] == palavra[posPalavra]){
				//esta pode ser a palavra
				ok = true;
				//ent�o incrementa o index
				posPalavra++;
			} 
			else{
				if(ok == true && posPalavra == palavra.length()){
					//se corresponde inteira, ent�o incrementa o numero de repeti��es
					repeticoes++;
				}
				//a paravra n�o era correspondente
				ok = false;
				//reseta o index
				posPalavra = 0;
			}
		}
		
			
	}while (result != NULL); // ESTA REPETINDO A ULTIMA LINHA
	
	//exibe a quantidade de vezes que aquela palavra foi encontrada
	cout << "Essa palavra foi encontrada " << repeticoes << " vezes" << endl;
	return 0;
}

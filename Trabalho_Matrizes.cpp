#include <iostream>
#include <stdio.h>
#include <vector>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <windows.h>
#include <process.h>
#include <locale.h>

#define SEED 100401

using namespace std;

bool isPrime(int number);
void menu();
void fillMatrix(long long int lines, long long int columns);
void createThreads(int numberThreads, int switchThread);
void thread(void* matrizParametros);
void thread2(void* matrizParametros);
void preencherParametros(int numberThreads, long long int lines, long long int columns);
void esvaziarParametros();
void esvaziarMatriz();

typedef struct {
    int id;

    int startLine, startColumn, endLine, endColumn;
} PARAMETRO;

HANDLE secaoCritica;

long long int lines = -1, columns = -1, totalPrimos = 0;
time_t startTime, endTime, totalTime;

vector<HANDLE> idThread;
vector<HANDLE> idThread2;
vector<vector<long long int>> matriz;
vector<vector<long long int>> matriz2;
vector<PARAMETRO> vetorParametro;
vector<PARAMETRO> vetorParametro2;

int main()
{
    srand(SEED);
    setlocale(LC_ALL, "pt_BR.UTF-8");
    secaoCritica = CreateMutex(NULL, FALSE, NULL);
    menu();
    CloseHandle(secaoCritica);
}

void menu() {
    int option = 0, n_Threads = 1, matrizPreenchida = -1, executado = 0;

    do {
        cout << "Escolha entre as opções:" << endl;
        cout << "(1) Definir tamanho da matriz" << endl;
        cout << "(2) Preencher a matriz com números aleatórios" << endl;
        cout << "(3) Definir o número de Threads" << endl;
        cout << "(4) Executar" << endl;
        cout << "(5) Visualizar o tempo de execução e quantidade de números primos" << endl;
        cout << "(6) Encerrar" << endl;
        cin >> option;
        cout << "\n";

        switch (option)
        {
        case 1:
            esvaziarMatriz();
            cout << "Entre com o numero de linhas: ";
            cin >> lines;
            cout << "Entre com o numero de colunas: ";
            cin >> columns;
            cout << "\n";
            matrizPreenchida = -1;
            matriz.resize(lines, vector<long long int>(columns));
            break;

        case 2:
            if (lines == -1 || columns == -1) cout << "\nTamanho da matriz não definido!!" << endl;
            else {
                cout << "Aguarde, populando matriz..." << endl;
                fillMatrix(lines, columns);
                matrizPreenchida = 1;
                cout << "Matriz Preenchida!\n" << endl;
            }
            break;

        case 3:
            totalPrimos = 0;
            cout << "Entre com o numero de Threads: ";
            cin >> n_Threads; 
            cout << "\n";
            break;

        case 4:
            if (matrizPreenchida <= 0) cout << "Matriz ainda não preenchida" << endl;
            else {
                cout << "Aguarde enquanto procuramos os números primos..." << endl;
                preencherParametros(n_Threads, lines, columns);
                cout << "Terminado!" << endl;
                executado = 1;
            }
            cout << "\n";
            break;

        case 5:
            if (executado == 0)cout << "Programa não executado anteriormente!" << endl;
            else {
                cout << "A quantidade total de primos desta matriz é: " << totalPrimos << endl;
                cout << "O tempo de execução deste programa foi: " << totalTime << endl;
            }
            cout << "\n";
            break;

        default:
            break;
        }
    } while (option != 6);
}

void fillMatrix(long long int lines, long long int columns) {
    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < columns; j++) {
            matriz[i][j] = (static_cast<long long>(rand())) << 8 | rand();
        }
    }
}

void createThreads(int numberThreads, int switchThread) {
    if (switchThread == 1) {
        for (int i = 0; i < numberThreads; i++) {
            idThread.push_back(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread, &vetorParametro[i], CREATE_SUSPENDED, NULL));
        }
        startTime = time(0);
        for (int i = 0; i < numberThreads; i++) {
            ResumeThread(idThread[i]);
        }
        WaitForMultipleObjects(numberThreads, idThread.data(), TRUE, INFINITE);
        endTime = time(0);
    }
    else {  
        for (int i = 0; i < numberThreads; i++) {
            idThread.push_back(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread2, &vetorParametro[i], CREATE_SUSPENDED, NULL));
        }
        startTime = time(0);
        for (int i = 0; i < numberThreads; i++) {
            ResumeThread(idThread[i]);
        }
        WaitForMultipleObjects(numberThreads, idThread.data(), TRUE, INFINITE);
        endTime = time(0);
    }
    totalTime = endTime - startTime;
}

void preencherParametros(int numberThreads, long long int lines, long long int columns) {
    PARAMETRO addDataMatriz;
    int totalNumbers = 0, totalEachThread = 0, firstLine = 0, lastLine = 0 , firstColumn = 0, lastColumn = 0, decider = 0;

    totalNumbers = lines * columns;
    totalEachThread = totalNumbers / numberThreads;
    lastLine = totalEachThread;
    lastColumn = totalEachThread;

    if (lines >= columns) {
        if ((lines * columns) % numberThreads == 0) {
            for (int i = 0; i < numberThreads; i++) {
                addDataMatriz.id = i;
                addDataMatriz.startColumn = firstColumn;
                addDataMatriz.startLine = firstLine;
                addDataMatriz.endLine = lastLine;
                vetorParametro.push_back(addDataMatriz);

                for (int j = firstLine; j < lastLine; j++) {
                    if (j % lines == 0 && j != 0) firstColumn++;
                }

                firstLine = lastLine;
                lastLine = lastLine + totalEachThread;
            }
        }
        else {
            for (int i = 0; i < numberThreads; i++) {
                addDataMatriz.id == i;
                if (i == (numberThreads - 1)) {
                    addDataMatriz.startColumn = firstColumn;
                    addDataMatriz.startLine = firstLine;
                    addDataMatriz.endLine = lines * columns;
                    vetorParametro.push_back(addDataMatriz);
                }
                else {
                    addDataMatriz.startColumn = firstColumn;
                    addDataMatriz.startLine = firstLine;
                    addDataMatriz.endLine = lastLine;
                    vetorParametro.push_back(addDataMatriz);
                }

                for (int j = firstLine; j < lastLine; j++) {
                    if (j % lines == 0 && j != 0)firstColumn++;
                }

                firstLine = lastLine;
                lastLine = lastLine + totalEachThread;
            }
        }
        decider = 1;
    }
    else {
        if ((lines * columns) % numberThreads == 0) {
            for (int i = 0; i < numberThreads; i++) {
                addDataMatriz.id = i;
                addDataMatriz.startColumn = firstColumn;
                addDataMatriz.startLine = firstLine;
                addDataMatriz.endColumn = lastColumn;
                vetorParametro.push_back(addDataMatriz);

                for (int j = firstColumn; j < lastColumn; j++) {
                    if (j % columns == 0 && j != 0) firstLine++;
                }

                firstColumn = lastColumn;
                lastColumn = lastColumn + totalEachThread;
            }
        }
        else {
            for (int i = 0; i < numberThreads; i++) {
                if (i == numberThreads - 1) {
                    addDataMatriz.id = i;
                    addDataMatriz.startColumn = firstColumn;
                    addDataMatriz.startLine = firstLine;
                    addDataMatriz.endColumn = lines * columns;
                    vetorParametro.push_back(addDataMatriz);
                }
                else {
                    addDataMatriz.id = i;
                    addDataMatriz.startColumn = firstColumn;
                    addDataMatriz.startLine = firstLine;
                    addDataMatriz.endColumn = lastColumn;
                    vetorParametro.push_back(addDataMatriz);
                }

                for (int j = firstColumn; j < lastColumn; j++) {
                    if (j % columns == 0 && j != 0) firstLine++;
                }

                firstColumn = lastColumn;
                lastColumn = lastColumn + totalEachThread;
            }
        }
        decider = 2;
    }

    createThreads(numberThreads, decider);
    esvaziarParametros();
}

void esvaziarMatriz() {
    matriz.swap(matriz2);
    matriz2.clear();
    idThread.swap(idThread2);
    idThread2.clear();

    esvaziarParametros();
}

void esvaziarParametros() {
    vetorParametro.swap(vetorParametro2);
    vetorParametro2.clear();
}

void thread(void* matrizParametros){
    PARAMETRO* dataMatriz;
    dataMatriz = (PARAMETRO*)matrizParametros;

    int travessiaLinha = 0, contadorPrimos = 0, travessiaColunas = 0;

    travessiaColunas = dataMatriz->startColumn;
    travessiaLinha = dataMatriz->startLine % lines;

    for (int i = dataMatriz->startLine; i < dataMatriz->endLine; i++) {       
        if (i % lines == 0 && i != 0) travessiaLinha = 0;

        if (travessiaLinha % lines == 0 && i != 0)travessiaColunas++;

        if (isPrime(matriz[travessiaLinha][travessiaColunas])) contadorPrimos++;

        travessiaLinha++;
    }

    WaitForSingleObject(secaoCritica, INFINITE);

    totalPrimos += contadorPrimos;

    ReleaseMutex(secaoCritica);

    _endthread();
}

void thread2(void* matrizParametros) {
    PARAMETRO* dataMatriz;
    dataMatriz = (PARAMETRO*)matrizParametros;

    int travessiaLinha = 0, contadorPrimos = 0, travessiaColunas = 0;

    travessiaColunas = dataMatriz->startColumn % columns;
    travessiaLinha = dataMatriz->startLine;

    for (int i = dataMatriz->startColumn; i < dataMatriz->endColumn; i++) {
        if (i % columns == 0 && i != 0) travessiaColunas = 0;

        if (travessiaColunas % columns == 0 && i != 0)travessiaLinha++;

        if (isPrime(matriz[travessiaLinha][travessiaColunas])) contadorPrimos++;

        travessiaColunas++;
    }

    WaitForSingleObject(secaoCritica, INFINITE);

    totalPrimos += contadorPrimos;

    ReleaseMutex(secaoCritica);

    _endthread();
}

bool isPrime(int number) {
    if (number <= 1) return false;
    if (number <= 3) return true;
    if (number % 2 == 0 || number % 3 == 0) return false;
    for (int i = 5; i * i <= number; i = i + 6) {
        if (number % i == 0 || number % (i + 2) == 0) return false;
    }
    return true;
}
#include<bits/stdc++.h>
using namespace std;

// Valores Iniciales de Aplha y Beta
const int MAX = 1000;
const int MIN = -1000;

int minimax(int depth, int nodeIndex, bool maximizingPlayer,
            int values[], int alpha, int beta, int h){
    // Condición de terminación. Es decir, el nodo hoja es alcanzado
    if (depth == h)
        return values[nodeIndex];

    if (maximizingPlayer){
        int best = MIN;
        // Recorre los nodos hijos de izquierda a derecha
        for (int i=0; i<2; i++){
            int val = minimax(depth+1, nodeIndex*2+i,false, values, alpha, beta, h);
            best = max(best, val);
            alpha = max(alpha, best);
            // Alpha Beta Pruning
            if (beta <= alpha)
                break;
        }
        return best;
    }else{
        int best = MAX;
        // Recorre los nodos hijos de izquierda a derecha
        for (int i=0; i<2; i++){
            int val = minimax(depth+1, nodeIndex*2+i,true, values, alpha, beta, h);
            best = min(best, val);
            beta = min(beta, best);
            // Alpha Beta Pruning
            if (beta <= alpha)
                break;
        }
        return best;
    }
}

int main(){
    int values[8] = { 3, 5, 6, 9, 1, 2, 0, -1 };
    int n = sizeof(values)/sizeof(values[0]);
    int h = log2(n);
    cout << "The optimal value is : "
         << minimax(0, 0, true, values, MIN, MAX, h)<<endl;
    return 0;
}

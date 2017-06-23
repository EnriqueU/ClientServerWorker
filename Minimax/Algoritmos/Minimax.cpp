#include<bits/stdc++.h>
using namespace std;

// Devuelve el valor óptimo que un maximizador puede obtener.
// depth es la profundidad actual en el árbol de juego.
// nodeIndex es el índice del nodo actual en las puntuaciones [].
// isMax es true si el movimiento actual es del maximizador, sino false
// scores[] almacena las hojas de Game tree.
// h es la altura máxima del árbol de juego
int minimax(int depth, int nodeIndex, bool isMax, int scores[], int h){
    // Condición de terminación. Es decir, el nodo hoja es alcanzado
    if (depth == h)
        return scores[nodeIndex];

    // Si el desplazamiento actual es maximizar, se busca el valor máximo alcanzable
    if (isMax)
       return max(minimax(depth+1, nodeIndex*2, false, scores, h),
            minimax(depth+1, nodeIndex*2 + 1, false, scores, h));

    // Si el movimiento actual es Minimizar, se busca el valor mínimo alcanzable
    else
        return min(minimax(depth+1, nodeIndex*2, true, scores, h),
            minimax(depth+1, nodeIndex*2 + 1, true, scores, h));
}

int main(){
    int scores[] = {3, 5, 2, 9, 12, 5, 23, 23};
    int n = sizeof(scores)/sizeof(scores[0]);
    int h = log2(n);
    int res = minimax(0, 0, true, scores, h);
    cout << "The optimal value is : " << res << endl;
    return 0;
}

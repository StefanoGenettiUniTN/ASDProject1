/**
 * SweetTangos - Progetto ASD 1 "Lettere da Powarts"
 *
 * Anno accademico: 2020-21
 *
 * Francesco Mascotti - Pietro Fronza - Stefano Genetti
 *
 * Soluzione avanzata : esistono cammini minimi multipli - ottimizzazioni tattiche
 *
 * */

#include <bits/stdc++.h>

using namespace std;

struct nodo {
    vector<pair<int, int>> vic; //vicino - peso
    //vector< vector<int> > vic; //alternativa con vector invece che pair
    bool visited = false;
};

/*Struttura cmp per priority queue*/
struct cmp{
    bool operator()(vector<int> a, vector<int> b){
        return a[1] > b[1];
    }
};

vector<nodo> grafo;
vector<nodo> albero;    //albero di copertura popolato eseguendo Dijkstra

//vector<vector<int>> duplicati;  //vettore che memorizza i nodi ai quali arrivo due volte con la stessa distanza, STRUTTURA [nodo, padre1, padre2, distanza]
queue<vector<int>> duplicati;

int main() {
    ifstream in("input.txt");
    ofstream out("output.txt");
    int N, M, P;    //P indica dove si trova powarts
    in >> N >> M >> P;
    grafo.resize(N);

    //Lettura grafo non orientato con peso
    for (int j = 0; j < M; j++) {
        //w = weight
        int from, to, w;
        in >> from >> to >> w;
        grafo[from].vic.push_back(make_pair(to, w));
        grafo[to].vic.push_back(make_pair(from, w));
    }

    /**DIJKSTRA**/

    albero.resize(N);   //taglia albero di copertura = taglia grafo (NB è connesso)

    priority_queue<vector<int>, vector<vector <int>>, cmp> pq;   //priority queue

    vector<int> distanze(N, INT_MAX);    //per Dijkstra inizializzo a infinito - nota: infinito = 60000
    vector<int> parent(N, P);   //array dei padri per albero di copertura
    vector<bool> visitati(N, false);    //tiene traccia dei nodi visitati
    vector<int> colore(N); //0-->bianco ; 1-->nero

    distanze[P] = 0;

    vector<int> insertInto_pq = {P, 0, P};  //inizializzazione: da sorgente a sorgente costo 0 - nota: sorgente = Powarts
    pq.push(insertInto_pq);

    while (!pq.empty()) {
        vector<int> prossimoNodo = pq.top(); //{current node, costo, parent}
        pq.pop();

        int u = prossimoNodo[0];    //current node
        int wu = prossimoNodo[1];

        if(visitati[u]==false) {
            visitati[u] = true;

            //cout << "dalla coda --> " << u << "\n";

            for (pair<int, int> vicini : grafo[u].vic) {

                int v = vicini.first;   //vicino
                int w = vicini.second;  //peso

                //int alt = distanze[u] + w;
                int alt = wu + w;

                if (alt < distanze[v]) {
                    insertInto_pq = {v, alt, u};

                    //cout << "SONO " << u << " inserito " << v << "\n";

                    pq.push(insertInto_pq);
                    distanze[v] = alt;
                    //visitati[v] = false;
                    parent[v] = u;
                    //grafo[v].visited = true;
                    //cout << "PRIMA " << u << " - " << v << " - " << "alt = " << alt << "\n";
                } else if (alt == distanze[v]) { //Distanze uguali --> parto a cercare il padre in comune
                    /*Inserisco in dupplicati con coda*/
                    int padre1 = u;
                    int padre2 = parent[v];
                    vector<int> duplicato = {v, padre1, padre2, alt};
                    duplicati.push(duplicato);
                    /*---*/
                }
            }
        }
    }
    /**---*/

    /*Controllo duplicati e correggo risalendo l'albero in ordine LIFO (queue duplicati)*/
    /*CODA*/
    while(!duplicati.empty()) {

        vector<int> dup = duplicati.front();
        duplicati.pop();

        int dupNode = dup[0];
        //int padre1 = dup[1];
        //int padre2 = dup[2];
        int dupDistance = dup[3];
        //cout << " --> " << dupNode << " - " << dupDistance << "\n";
        if (dupDistance == distanze[dupNode]) { //in questo caso c'è più di un cammino minimo

            int antenatoComune = P;

            /*
            for(int i=0; i<N; i++){ //inizialmente tutti i nodi binachi
                colore[i] = 0;
            }
            */
            colore.assign(N, 0);

            int padre1 = dup[1];
            int padre2 = dup[2];

            bool trovato = false;
            colore[padre1] = 1;
            colore[padre2] = 1;

            while (trovato == false) {
                padre1 = parent[padre1];

                if (colore[padre1] == 1) {
                    if (padre1 != P) {
                        trovato = true;
                        antenatoComune = padre1;
                        continue;
                    } else if (padre2 == P) {  //qui padre 1 è uguale a P
                        trovato = true;
                        antenatoComune = padre1;
                        continue;
                    }
                } else {
                    colore[padre1] = 1;
                }

                padre2 = parent[padre2];
                if (colore[padre2] == 1) {
                    if (padre2 != P) {
                        trovato = true;
                        antenatoComune = padre2;
                    } else if (padre1 == P) {
                        trovato = true;
                        antenatoComune = padre2;
                    }
                } else {
                    colore[padre2] = 1;
                }
            }
            parent[dupNode] = antenatoComune;
        }
    }
    /*---*/


    /*
    //SERVE PER STAMPARE LE DISTANZE
    for(int i=0; i<N; i++){
        cout << i << " - " << distanze[i] << "\n";
    }
    */


    /*
    //Stampo vettore dei padri
    for(int i=0; i<N; i++){
        cout << i << " - "<< parent[i] << "\n";
    }
    */

    //Riempio albero di copertura
    for(int i=0; i<N; i++){
        if(i!=P){   //powarts è radice --> no padre
            albero[i].vic.push_back(make_pair(parent[i], 1));
            albero[parent[i]].vic.push_back(make_pair(i, 1));
        }
    }


    //Stampa albero
    /*
    cout << "STAMPO ALBERO" << "\n";
    for(int i=0; i<N; i++) {
        cout << "Nodo " << i << " ha " << albero[i].vic.size() <<" vicini" << endl;
        for(pair<int,int> vicino: albero[i].vic) {
            int v = vicino.first;
            cout << "  " << v << endl;
        }
    }
    cout << "\n\n";
     */


    //Adesso applico la soluzione minima
    vector<vector<int>> rami(albero[P].vic.size());   //un vettore di dimensione pari al numero di adiacenti di P e ogni elemento è un vettore ospitante tutti i nodi raggiungibili in quella direzione

    int i = 0;
    int max = 0;    //tengo traccia del vicino con più nodi raggiungibili
    queue<int> queue;
    for(pair<int,int> vicini : albero[P].vic){   //per ogni vicino di P parte una BFS
        /*
        for (nodo &n : albero) {
            n.visited = false;
        }
        */


        int vicino = vicini.first;

        albero[vicino].visited = true;
        rami[i].push_back(vicino);
        albero[P].visited = true;    //metto a visited anche P

        queue.push(vicino);

        while (!queue.empty()) {
            int u = queue.front();
            queue.pop();

            for (pair<int,int> vic : albero[u].vic) {
                int v = vic.first;
                if (albero[v].visited == false) {
                    queue.push(v);
                    albero[v].visited = true;

                    //Inserisco in rami di i
                    rami[i].push_back(v);
                }
            }
        }

        //Aggiorno massimo
        if(rami[i].size() > rami[max].size()){
            max=i;
        }

        i++;    //<-- passo al prossimo vicino
    }

    //cout << rami[max].size() << "\n";
    out << rami[max].size() << "\n";
    for(int i : rami[max]){
        //cout << i << "\n";
        out << i << "\n";
    }

    return 0;
}

#include <bits/stdc++.h>
using namespace std;

int evaluate_poly(const std::vector<bool>& bits, int r, int p) {
    int val = 0;
    for (bool b : bits) {
        //p(x) = ((an*r)+an-1)*r...
        val = (val * r + b) % p;
    }
    return val;
}

bool is_prime(int x) {
    if (x < 2) return false;
    if (x == 2) return true;
    if (x % 2 == 0) return false;
    for (int d = 3; d * d <= x; d += 2)
        if (x % d == 0) return false;
    return true;
}

int find_prime_in_interval(int low, int high) {
    for (int p = low; p <= high; ++p)
        if (is_prime(p)) return p;
    return -1; //nunca acontece
}


bool rabin_yao(const std::vector<bool>& alice_bits,
               const std::vector<bool>& bob_bits,
               int n) 
{
    int p = find_prime_in_interval(3*n, 6*n);
    
    // 2. Gera r aleatório uniforme em [0, p-1]
    static std::random_device rd;
    static std::mt19937 gen(696742);
    std::uniform_int_distribution<int> dis(0, p-1);
    int r = dis(gen);

    // 3. Calcula fingerprints
    int fa = evaluate_poly(alice_bits, r, p);
    int fb = evaluate_poly(bob_bits, r, p);

    // 4. Comparação
    return (fa == fb);
}
int32_t main() {
    // Parâmetros da simulação
    int n = 20;                // número de bits (entradas de tamanho n)
    int num_tests = 100000;     // quantidade de pares a testar

    // Geradores para produzir vetores de bits aleatórios
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> bit_dist(0, 1);

    // Estatísticas
    int false_positives = 0;  // x != y mas protocolo disse iguais
    int false_negatives = 0;  // x == y mas protocolo disse diferentes, sempre resultara em nulo devido 
                              //a propriedades matematicas descritas no artigo 
    int total_equal_pairs = 0;
    int total_different_pairs = 0;

    //Gera menor primo no intervalo
    int p = find_prime_in_interval(3 * n, 6 * n);


    cout << "Simulacao do protocolo Rabin Yao\n";
    cout << "n = " << n << " bits (N = " << (1LL << n) << " entradas possiveis)\n";
    cout << "Primo p = " << p << " (intervalo [" << 3*n << ", " << 6*n << "])\n";
    cout << "Executando " << num_tests << " testes...\n";

    for (int t = 0; t < num_tests; t++) {
        // Gera vetor de bits para Alice
        vector<bool> x(n);
        for (int i = 0; i < n; i=i+1) x[i] = bit_dist(gen);

        // Decide se Bob terá a mesma string (metade das vezes) ou uma diferente
        bool equal_input = (t % 2 == 0);  // alterna para ter balanceamento
        vector<bool> y;
        if (equal_input) {
            y = x;  // mesmo vetor
        } else {
            // Gera um vetor diferente (pode coincidir por acaso, mas improvável)
            y.resize(n);
            for (int i = 0; i < n; ++i) y[i] = bit_dist(gen);
            // Garante que seja realmente diferente (se por acaso for igual, troca um bit)
            if (y == x) y[0] = !y[0];
        }

        // Executa o protocolo
        bool result = rabin_yao(x, y, n);

        // Contagem de erros
        if (equal_input) {
            total_equal_pairs++;
            if (!result) false_negatives++;   // eram iguais, mas protocolo disse diferente
        } else {
            total_different_pairs++;
            if (result) false_positives++;    // eram diferentes, mas protocolo disse iguais
        }
    }

    // Exibe estatísticas
    cout << "\nResultados:" << "\n";
    cout << "Pares iguais testados:      " << total_equal_pairs << "\n";
    cout << "Pares diferentes testados:  " << total_different_pairs << "\n";
    cout << "Falsos negativos: " << false_negatives 
         << " (" << (total_equal_pairs > 0 ? (100.0 * false_negatives / total_equal_pairs) : 0) << "%)\n";
    cout << "Falsos positivos:  " << false_positives 
         << " (" << (total_different_pairs > 0 ? (100.0 * false_positives / total_different_pairs) : 0) << "%)\n";

    return 0;
}

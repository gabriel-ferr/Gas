//
//            Script geral para um gás de partículas idênticas confinadas em uma caixa.
//    * Gás dentro de uma caixa com as paredes oscilando.
//    * Simulação completa para a prova =D
// ....................................................................................................................
//      → Bibliotecas necessárias:
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
// ....................................................................................................................
#include "mt19937-64.c"
// ....................................................................................................................
//      → Configurações globais:
#define MAX_PARTICLES 6000                  //  Número máximo de partículas que o programa aceita.
#define N_DIMS 2                            //  Número de dimensões da caixa.
// ....................................................................................................................
//      → Função principal:
int main(int argc, const char * argv[]) {
    // ................................................................................................................
    //      → Define as variáveis que serão utilizadas:
    int N;                                                  // [       ] - Número de partículas na caixa.
    int i;                                                  // [       ] - Contador de partículas.
    int j;                                                  // [       ] - Contador de partículas.
    int k;                                                  // [       ] - Contador de dimensões.
    int f;                                                  // [       ] - Contador de passos para saída de dados.
    int f2;                                                 // [       ] - Contador de passos de saída secundário.
    int f3;                                                 // [       ] - Contador de passos de saída macro.
    double t;                                               // [   s   ] - Tempo de integração.
    double dt;                                              // [   s   ] - Step de integração.
    double LL;                                              // [   m   ] - Tamanho da caixa. (Estado)
    double v0;                                              // [  m/s  ] - Velocidade inicial das partículas.
    double aux;                                             // [       ] - Variável de cálculo auxiliar.
    double freq;                                            // [ rad/s ] - Frequência de oscilação da onda.
    double theta;                                           // [  rad  ] - Direção angular das partículas.
    double t_fim;                                           // [   s   ] - Tempo total de integração.
    double L_max;                                           // [   m   ] - Tamanho máximo da caixa.
    double radius;                                          // [   m   ] - Raio das partículas.
    double periodo;                                         // [   s   ] - Período de oscilação da onda.
    double distance;                                        // [   m   ] - Distância entre partículas.
    double L_old;                                           // [   m   ] - Posição velha da parede.
    double v_old;                                           // [  m/s  ] - Velocidade antiga.
    double L[N_DIMS + 1];                                   // [   m   ] - Tamanho da caixa.
    double vL[N_DIMS + 1];                                  // [   m²  ] - "Volume" da caixa (no caso, é a área por ser 2D)
    double dr[N_DIMS + 1];                                  // [   m   ] - Variação da posição das partículas.
    double dv[N_DIMS + 1];                                  // [  m/s  ] - Variação da velocidade das partículas.
    double dp_p[N_DIMS + 1];                                // [  Pa   ] - Variação de pressão nas paredes positivas.
    double dp_n[N_DIMS + 1];                                // [  Pa   ] - Variação de pressão nas paredes negativas.
    double press_p[N_DIMS + 1];                             // [  Pa   ] - Pressão nas paredes positivas.
    double press_n[N_DIMS + 1];                             // [  Pa   ] - Pressão nas paredes negativas.
    double m[MAX_PARTICLES + 1];                            // [  kg   ] - Peso das partículas.
    double r[MAX_PARTICLES + 1][N_DIMS + 1];                // [   m   ] - Posição das partículas.
    double v[MAX_PARTICLES + 1][N_DIMS + 1];                // [  m/s  ] - Velocidade das partículas.
    double a[MAX_PARTICLES + 1][N_DIMS + 1];                // [  m/s² ] - Aceleração das partículas.

    char nome[100];                                         // [       ] - Nome do arquivo / pasta de saída.
    unsigned long long seed;                                // [       ] - Semente para o código pseudo-random.

    FILE *fo;			                                    // [       ] - Arquivo de saída das imagens.
    FILE *fodat;			                                // [       ] - Dados globais.
    // ................................................................................................................
    //      → Inicializa as variáveis.
    sscanf(argv[1], "%lf", &L[1]);
    sscanf(argv[2], "%lf", &L[2]);
    sscanf(argv[3], "%d", &N);
    sscanf(argv[4], "%lf", &v0);
    sscanf(argv[5], "%lf", &t_fim);
    sscanf(argv[6], "%lf", &dt);
    sscanf(argv[7], "%llu", &seed);
    mkdir(argv[8], 0700);
    sscanf(argv[9], "%lf", &radius);
    sscanf(argv[10], "%lf", &periodo);

    init_genrand64(seed);
    L_max = L[1];

    freq = 2.0 * M_PI / periodo;

    vL[1] = 0.0;
    vL[2] = 0.0;

    f = 0;
    f2 = 0;
    f3 = 0;
    for (i = 1; i <= N; i++) {
        m[i] = 2.0;
        for (k = 1; k <= N_DIMS; k++) {
            r[i][k] = L[k] * genrand64_real2();
        }
        theta = 2 * M_PI * genrand64_real2();
        v[i][1] = v0 * cos(theta);
        v[i][2] = v0 * sin(theta);
    }

    sprintf(nome, "%s.dat", argv[8]);
    fodat = fopen(nome, "w");
    // ................................................................................................................
    //      → Simulação
    for (t = 0; t <= t_fim; t += dt) {
        if (f <= 0) {
            sprintf(nome, "%s/gas-%06d.dat", argv[8], f2);
            fo = fopen(nome, "w");
            for(i = 1; i <= N; i++) {
                fprintf(fo, "%lf %d %lf ", t, i, m[i]);
                for (k = 1; k <= N_DIMS; k++) fprintf(fo, "%lf ", r[i][k]);
                for (k = 1; k <= N_DIMS; k++) fprintf(fo, "%lf ", v[i][k]);
                fprintf(fo, "%lf ", v[i][1] * v[i][1] + v[i][2] * v[i][2]);
                fprintf(fo, "%lf ", 0.5 * m[i] * (v[i][1] * v[i][1] + v[i][2] * v[i][2]));
                fprintf(fo, "\n");
            }
            f = 1;
            fclose(fo);
            f2++;
            f3--;

            if (f3 <= 0) {

                f3 = 100;

                //	Calcula as grandezas macroscopicas.
                for (k = 1; k <= N_DIMS; k++) {
                    LL = k == 1 ? L[2] : L[1];
                    press_p[k] = dp_p[k] / (f * dt * LL);
                    press_n[k] = -dp_n[k] / (f * dt * LL);
                }

                //	Salva o arquivo.
                fprintf(fodat, "%lf %lf %lf %lf %lf %lf\n", t, press_p[1], press_p[2], press_n[1], press_n[2], L[1] * L[2]);

                //	Reseta as variáveis.
                dp_p[1] = 0;
                dp_p[2] = 0;
                dp_n[1] = 0;
                dp_n[2] = 0;
            }
        }

        f--;

        //	Zera as acelerações
        for (i = 1; i <= N; i++) for (k = 1; k <= N_DIMS; k++) a[i][k] = 0.0;

        //	Método de Euler
        for (i = 1; i <= N; i++) for (k = 1; k <= N_DIMS; k++) r[i][k] += v[i][k] * dt;
        for (i = 1; i <= N; i++) for (k = 1; k <= N_DIMS; k++) v[i][k] += a[i][k] * dt;

        //	Calcula as paredes
        L_old = L[1];
        if (t < (t_fim / 2.0)) {
            L[1] = (L_max - 5.0) + 5.0 * cos(freq * t);
            vL[1] = (L[1] - L_old) / dt;
        } else {
            vL[1] = 0.0;
        }

        //	Aplica as interações
        for (i = 1; i <= N; i++) {
            //	Paredes
            for (k = 1; k <= N_DIMS; k++) {
                if(r[i][k] >= L[k] && v[i][k] > 0.0){
                    r[i][k] = L[k] - 1e-6;
                    v_old = v[i][k];
                    v[i][k] = (-1) * v[i][k] + 2.0 * vL[k];
                    dp_p[k] += -m[i] * (v[i][k] - v_old);
                }
                if (r[i][k] > L[k]) r[i][k] = L[k];
                if(r[i][k] <= 0.0) {
                    r[i][k] = 0.0 + 1e-6;
                    v[i][k] = (-1) * v[i][k];
                    dp_n[k] += -m[i] * 2.0 * v[i][k];
                }
            }
        }

        for (i = 1; i <= N; i++) {
            //	Entre partículas
            for (j = i + 1; j <= N; j++){
                distance = 0.0;
                for (k = 1; k <= N_DIMS; k++) distance += pow(r[j][k] - r[i][k], 2);
                distance = sqrt(distance);

                if (distance > 2 * radius) continue;
                if (distance < 1e-6) continue;

                aux = 0.0;
                for (k = 1; k <= N_DIMS; k++) {
                    dr[k] = r[j][k] - r[i][k];
                    dv[k] = v[j][k] - v[i][k];
                    aux += dv[k] * dr[k];
                }

                aux *= 2.0 / (distance * distance);
                aux /= (m[i] + m[j]);

                for (k = 1; k <= N_DIMS; k++) {
                    v[i][k] += m[j] * aux * dr[k];
                    v[j][k] -= m[i] * aux * dr[k];
                }
            }
        }
    }

    return 0;
}
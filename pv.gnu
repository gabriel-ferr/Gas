# Arquivo de saída
set terminal pngcairo size 1200,800 enhanced font "Arial,12"
set output "pv.png"

input = "gas_cont_2.dat"

# Ajuste de curvas
A = 1
gamma = 1.5

f(x) = A - gamma * x    # A = ln(a)
fit f(x) input using (log($6)):(log(($2+$3+$4+$5)/4.0)) via A, gamma

a = exp(A)
f(v) = a * v**(-gamma)

# Configuração de legenda
set xlabel "Volume (u.a.)"
set ylabel "Pressão (u.p.)"
set grid

# Plot
plot input u ($6):(($2 + $3 + $4 + $5)/4.0) with points pt 7 ps 0.8 title "Dados", \
     f(x) w l lw 2 title sprintf("ajuste: PV^{%.1f} = cte", gamma)

# Fecha o arquivo
unset output

##  Lembrando que C = N * m * V^2 / 2 !! -> V = sqrt(2 C / (N * m)). Note que PV = Nk_b T = a; logo T = a / N.
##  O V é o v_rms, dado pela raiz quadrada da média da velocidades ao quadrado.
##  Também dá pra usar a relação C = P V, que funciona relativamente bem para volumes grandes ....
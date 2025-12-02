# Arquivo de saída
set terminal pngcairo size 1200,800 enhanced font "Arial,12"
set output "maxwell.png"

#   Configurações
m = 2.0
N = 500

global_data = "gas_cont_2.dat"
input = "gas_cont_2/gas-002000.dat"

# Vou tentar pegar os dados da última linha (essa parte é true ChatGPT, e o bixo sofreu pra fazer isso funcionar kkkk)
# AWK lê a última linha real
lastline = system(sprintf("awk 'END{print}' %s", global_data))

print "Última linha bruta =", lastline

# Separar os campos da última linha lida
t_last    = real(word(lastline,1))
p_px      = real(word(lastline,2))
p_py      = real(word(lastline,3))
p_nx      = real(word(lastline,4))
p_ny      = real(word(lastline,5))
V_last    = real(word(lastline,6))

P_last = (p_px + p_py + p_nx + p_ny)/4.0

# Calcula a temperatura
T = P_last * V_last / N
velocity = sqrt(2 * T / m)
# NkT = mv^2/2 -> v^2 = NkT / m

print "P_final =", P_last
print "V_final =", V_last
print "T =", T
print "vel_mp =", sqrt(T / m)
print "vel_mean =", sqrt(3.14 * T /(2 * m))
print "vel_rms =", velocity

# Binagem
binwidth = 0.2
bin(x,width) = width * floor(x / width + 0.5)

# Distribuição de Maxwell-Boltzmann 2D
f(v) = (m * v/(T)) * exp( -m * v*v/(2*T) )
f_scaled(v) = N * binwidth * f(v)

# Estilo
set style fill solid 0.5
set xlabel "Velocidade v (u.a./s)"
set ylabel "Contagem de partículas"

# Plota
plot input using (bin(sqrt($6*$6 + $7*$7), binwidth)):(1.0) \
     smooth freq with boxes lc rgb "skyblue" title "Histograma",\
     f_scaled(x) lw 2 lc rgb "green" title "Maxwell-Boltzmann 2D"

binwidth = 0.5
Ly = 20.0
A = binwidth * Ly

# 1) histograma
set table "hist.dat"
plot "gas_cont_2/gas-002000.dat" using (floor($5/binwidth)):(1.0) smooth freq
unset table

# 2) cria dens.dat COM SOMENTE NÚMEROS (sem formatação)
cmd = sprintf("awk -v A=%.12g '{print $2/A}' hist.dat > dens.dat", A)
system(cmd)

# 3) pega média
stats "dens.dat" using 1 nooutput
mean  = STATS_mean
Nbins = STATS_records

# 4) soma manual das diferenças ao quadrado (via awk)
cmd2 = sprintf("awk -v m=%.12g '{d=$1-m; s+=d*d} END{print s}' dens.dat", mean)
sum_sq_str = system(cmd2)
sum_sq = real(sum_sq_str)

# variância e rms
var = sum_sq / Nbins
rms = sqrt(var)

print "mean =", mean
print "var  =", var
print "rms  =", rms
print "flut_rel =", rms/mean

# 5) plot
set terminal pngcairo size 1200,800 enhanced font "Arial,12"
set output "fluc.png"
set style fill solid 0.5

set xlabel "y (u.a.)"
set ylabel "Contagem de partículas"

upper = mean + rms
lower = mean - rms

plot \
  "hist.dat" using 1:($2/A) with boxes lc rgb "skyblue" title "densidade", \
  mean         with lines lc rgb "red" lw 2 title "média", \
  upper        with lines lc rgb "dark-red" dt 2 lw 1 title "+RMS", \
  lower        with lines lc rgb "dark-red" dt 2 lw 1 title "-RMS"

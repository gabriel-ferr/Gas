#!/bin/bash

# ajuste estes parâmetros conforme sua caixa
xmin=0
ymin=0
width_x=0.5      # tamanho da célula
width_y=0.1
X=80           # número de células em x (opcional)
Y=10            # número de células em y (opcional)

if [ $# -ne 1 ]; then
	echo "usage: ./corda.sh dir"
	exit 1
fi

mkdir -p $1/png
cd $1

for i in gas-*.dat; do
	echo "Processando $i"
	cat "$i" > dados.dat
	
	gnuplot -persist &> /dev/null <<EOF
set term png
set output "png/${i}.png"
set title "${i}"
set xlabel "x (ua)"
set ylabel "y (ua)"
set xrange [-0.5:40.5]
set yrange [-0.5:1.5]
plot "dados.dat" using 4:5 with points pt 7 ps 0.5
EOF

	awk -v xmin=$xmin -v ymin=$ymin -v w_x=$width_x -v w_y=$width_y -v X=$X -v Y=$Y '
	{
		x = $4; y = $5;
  		ix = int((x - xmin) / w_x);
  		iy = int((y - ymin) / w_y);
  		if (ix >= 0 && ix < X && iy >= 0 && iy < Y) counts[ix","iy]++;
	}
	END {
  		for (iy = 0; iy < Y; iy++) {         # note: varrendo iy por linha melhora o visual quando plotamos
    			for (ix = 0; ix < X; ix++) {
      				x = xmin + (ix + 0.5) * w_x;
      				y = ymin + (iy + 0.5) * w_y;
      				key = ix","iy;
      				c = counts[key] + 0;
      				print x, y, c;
    			}
    		print "";   # bloco em branco entre linhas (pm3d aceita)
  	}
	}' "$i" > grid.dat


	gnuplot -persist &> /dev/null <<EOF
set term png
set output "png/${i}-heatmap.png"
set title "Densidade ${i}"
set xlabel "x (ua)"
set ylabel "y (ua)"
set xrange [-0.5:40.5]
set yrange [-0.5:1.5]
set view map
set pm3d map
set palette rgbformulae 33,13,10
set colorbox
set cbrange [0:10]
set cblabel "Contagem"
splot 'grid.dat' using 1:2:3 with pm3d notitle
EOF

  gnuplot -persist << EOF
set term png
set output "png/${i}-histo.png"
set title "Distribuição de partículas no eixo x, frame = ${i}"
set xlabel "x (ua)"
set ylabel "Número de partículas"
set xrange[-0.5:40.5]
set yrange[-0.5:25.5]

binwidth = 0.5
bin(x,width) = width * floor(x / width + 0.5)

set boxwidth binwidth
set style fill solid 0.5

plot 'grid.dat' using (bin(\$1, binwidth)):(\$3) smooth freq with boxes lc rgb "skyblue" notitle
EOF

done

ffmpeg -y -r 25 -i png/gas-%06d.dat.png -c:v libx264 -r 30 -pix_fmt yuv420p ../$1.mp4
ffmpeg -y -r 25 -i png/gas-%06d.dat-heatmap.png -c:v libx264 -r 30 -pix_fmt yuv420p ../$1-heatmap.mp4
ffmpeg -y -r 25 -i png/gas-%06d.dat-histo.png -c:v libx264 -r 30 -pix_fmt yuv420p ../$1-histo.mp4
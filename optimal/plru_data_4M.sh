LIST="milc lbm soplex sphinx3 bwaves mcf leslie3d libquantum astar gcc xalancbmk hmmer bzip2 perlbench gobmk h264"
echo "\t \t all\t \t \t \t p4\t \t \t \t p8\t \t \t \t p16\t \t\t \t lru"
for a in $LIST
	do
	printf "%s \t \t" $a

	
	cat oracle_LW0_r0_trace-$a'_sec4_4M_32way_f8.txt' | grep "evicted_bits" | awk '{printf "%d\t", $2}'
	cat oracle_LW0_r0_trace-$a'_sec4_4M_32way_f8.txt' | grep "hit rate" | awk '{printf "%f\t", $4}'

	cat ../pusedo_lru/plru_trace-$a'_4M_32way_f8.txt' | grep "evicted_bits" | awk '{printf "%d\t %d\t", $2, $3}'
	cat ../pusedo_lru/plru_trace-$a'_4M_32way_f8.txt' | grep "hit rate" | awk '{printf "%f\t", $4}'
	echo ""
	echo ""
	done

	

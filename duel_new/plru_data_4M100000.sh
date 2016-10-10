LIST="gobmk astar mcf bzip2 h264 hmmer libquantum perlbench gcc xalancbmk lbm milc sphinx3 soplex leslie3d bwaves"
echo "\t \t all\t \t \t \t p4\t \t \t \t p8\t \t \t \t p16\t \t\t \t lru"
for a in $LIST
	do
	printf "%s \t \t" $a

	
	cat duelnew_LW32_r4_trace-$a'_4M_32way_f8s64d100000.txt' | grep "evicted_bits" | awk '{printf "%d\t %d\t", $2, $3}'
	cat duelnew_LW32_r4_trace-$a'_4M_32way_f8s64d100000.txt' | grep "hit rate" | awk '{printf "%f\t", $4}'
	cat duelnew_LW48_r4_trace-$a'_4M_32way_f8s64d100000.txt' | grep "evicted_bits" | awk '{printf "%d\t %d\t", $2, $3}'
	cat duelnew_LW48_r4_trace-$a'_4M_32way_f8s64d100000.txt' | grep "hit rate" | awk '{printf "%f\t", $4}'
	cat duelnew_LW64_r4_trace-$a'_4M_32way_f8s64d100000.txt' | grep "evicted_bits" | awk '{printf "%d\t %d\t", $2, $3}'
	cat duelnew_LW64_r4_trace-$a'_4M_32way_f8s64d100000.txt' | grep "hit rate" | awk '{printf "%f\t", $4}'
	

	cat ../pusedo_lru/plru_trace-$a'_4M_32way_f8.txt' | grep "evicted_bits" | awk '{printf "%d\t %d\t", $2, $3}'
	cat ../pusedo_lru/plru_trace-$a'_4M_32way_f8.txt' | grep "hit rate" | awk '{printf "%f\t", $4}'
	echo ""
	echo ""
	done

	

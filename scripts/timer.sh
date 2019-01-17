start_time=$(date +%s)
echo "Press any key to report time elapsed"
read -s -n 1 readret
end_time=$(date +%s)
elapsed_time=$(( $end_time - $start_time ))
echo "Test took $(date --date=@$elapsed_time +%M\'%S\")"


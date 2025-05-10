
function display_file() {
    local screen_height=$(tput lines)  # Υπολογίζει τις γραμμές της οθόνης
    local current_line=0               # Μετρητής γραμμών

    echo "Προβολή όλων των εγγραφών από το αρχείο $DATA_FILE"
    
    if [[ -f "$DATA_FILE" ]]; then
        while IFS= read -r line; do
            echo "$line"
            ((current_line++))

            # Ελέγχει αν γεμίζει η οθόνη
            if (( current_line % (screen_height - 1) == 0 )); then
                echo -n "Πατήστε <space> για συνέχεια ή <e> για έξοδο: "
                read -n 1 user_input
                echo ""

                if [[ "$user_input" == "e" ]]; then
                    echo "Τερματισμός Λειτουργίας"
                    break
                fi
            fi
        done < "$DATA_FILE"
    else
        echo "Το αρχείο $DATA_FILE δεν υπάρχει ή είναι κενό."
    fi
}

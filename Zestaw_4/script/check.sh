  GNU nano 6.2                              check.sh                                        
#!/bin/bash

best_original="10000"
best_modified="10000"
best_file="10000"

# Loop through all files with the given structure
for file in /cluster-home/rzeszute_1206727/4_zadanie/5/slurm*.out; do
  echo "Processing file: $file"

  # Extract lines containing "Kod oryginalny" and "Kod zmodyfikowany"
  original=$(grep -oP 'Kod oryginalny\s+:\s+\K[0-9]+msec' "$file" | tr -d 'msec')
  modified=$(grep -oP 'Kod zmodyfikowany:\s+(\K[0-9]+)+msec' "$file" | tr -d 'msec')

  # Display the extracted values
  echo "Kod oryginalny: $original msec"
  echo "Kod zmodyfikowany: $modified msec"

  # Compare values and update the best result if necessary
  if ((modified < best_modified)); then
    best_original="$original"
    best_modified="$modified"
    best_file="$file"
    echo "ede"

  fi

  # Check for the "BLAD" clause and display a warning in red if found
  if grep -q 'BLAD' "$file"; then
    echo -e "\e[91mUWAGA: Znaleziono klauzulę 'BLAD' w pliku $file\e[0m"
  fi

  echo "----------------------------------"
done

# Display the best result and the corresponding file
echo "Najlepszy wynik:"
echo "Kod oryginalny: $best_original msec"
echo "Kod zmodyfikowany: $best_modified msec (plik: $best_file)"


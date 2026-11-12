import os

# Configuration
TARGET_DIR = "."  
OUTPUT_FILE = "project_dump.txt"

def dump_directory():
    # Get the name of this running script automatically
    current_script = os.path.basename(__file__)
    
    with open(OUTPUT_FILE, 'w', encoding='utf-8') as outfile:
        for root, dirs, files in os.walk(TARGET_DIR):
            # Skip hidden folders and build artifacts
            dirs[:] = [d for d in dirs if not d.startswith('.') and d not in ['__pycache__', 'build', 'dist']]
            
            for file in files:
                # EXCLUDE: The output file OR this script (whatever it's named)
                if file == OUTPUT_FILE or file == current_script:
                    continue 
                
                file_path = os.path.join(root, file)
                
                # Write Header
                outfile.write(f"\n{'='*60}\n")
                outfile.write(f"FILE: {file_path}\n")
                outfile.write(f"{'='*60}\n\n")
                
                # Write Content
                try:
                    with open(file_path, 'r', encoding='utf-8') as infile:
                        outfile.write(infile.read())
                except Exception as e:
                    outfile.write(f"[ERROR: Could not read file (likely binary): {e}]\n")
                
                outfile.write("\n")

    print(f"✅ Successfully dumped contents to '{OUTPUT_FILE}'")
    print(f"   (Excluded: '{OUTPUT_FILE}' and '{current_script}')")

if __name__ == "__main__":
    dump_directory()
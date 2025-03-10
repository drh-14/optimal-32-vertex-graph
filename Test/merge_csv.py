#chmod +x merge_csv.py
#./merge_csv.py <j>

import os
import glob
import re
import sys
import argparse
import math

TOLERANCE = 1e-6

def parse_graph_block(block):
    """
    Given a block of text (graph block), parse and return the ASPL value.
    Assumes that the second line contains "minASPL: <value>".
    Returns a tuple (aspl, block_text). If parsing fails, returns (None, block_text).
    """
    lines = block.strip().splitlines()
    if len(lines) < 2:
        return None, block
    # Try to extract the ASPL value from the second line
    m = re.search(r"minASPL:\s*([\d\.]+)", lines[1])
    if m:
        try:
            aspl = float(m.group(1))
            return aspl, block
        except ValueError:
            return None, block
    return None, block

def merge_csv_files(total_j):
    # Get a list of files matching pattern "*_<total_j>.csv"
    pattern = f"*_{total_j}.csv"
    files = glob.glob(pattern)
    if not files:
        print(f"No files found matching pattern {pattern}")
        return

    global_best = math.inf
    best_graphs = []  # list of graph blocks (strings)
    
    # Loop over each CSV file
    for filename in files:
        print(f"Processing file: {filename}")
        try:
            with open(filename, "r") as f:
                content = f.read()
        except Exception as e:
            print(f"Error reading {filename}: {e}")
            continue
        
        # Split content into graph blocks. 
        # We assume that blocks are separated by one or more blank lines.
        blocks = re.split(r'\n\s*\n', content)
        for block in blocks:
            block = block.strip()
            if not block:
                continue
            if not block.startswith("Graph"):
                continue
            aspl, graph_text = parse_graph_block(block)
            if aspl is None:
                continue
            # Compare ASPL with current best
            if aspl < global_best - TOLERANCE:
                # New best found: flush current list and update global_best
                global_best = aspl
                best_graphs = [graph_text]
            elif math.isclose(aspl, global_best, abs_tol=TOLERANCE):
                # Equal within tolerance, append
                best_graphs.append(graph_text)
            # else: aspl is higher than current best, ignore

        # Delete the processed file
        try:
            os.remove(filename)
            print(f"Deleted file: {filename}")
        except Exception as e:
            print(f"Error deleting {filename}: {e}")

    # Write merged output to merged.csv
    if best_graphs:
        with open("merged.csv", "w") as out:
            # Optionally, print the global best ASPL as header
            out.write(f"Global minASPL: {global_best:.6f}\n\n")
            for graph in best_graphs:
                out.write(graph + "\n\n")
        print(f"Merged CSV created as merged.csv with global minASPL = {global_best:.6f}")
    else:
        print("No valid graphs found to merge.")

def main():
    parser = argparse.ArgumentParser(description="Merge CSV files with names <i>_<j>.csv")
    parser.add_argument("j", type=int, help="The second parameter in CSV file names (<i>_<j>.csv)")
    args = parser.parse_args()
    
    merge_csv_files(args.j)

if __name__ == "__main__":
    main()

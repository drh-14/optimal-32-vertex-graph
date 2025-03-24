import polars as pl
import uuid

with open('Test/merged.csv', "r") as csvFile:
    csvFile.readlines(2)
    while line := csvFile.readline():
        if line.strip() == 'Adjacency list:':
            data = {"Source": [], "Target": []}
            while line := csvFile.readline().strip():
                index = line.index(":")
                parent = line[:index].strip()
                for child in line[index + 1:].strip().split(' '):
                    data["Source"].append(parent)
                    data["Target"].append(child)
            df = pl.DataFrame(data)
            randHex = uuid.uuid4().hex
            with open(f'{randHex}.csv', "w") as file:
                pl.DataFrame.write_csv(df, f'Test/{randHex}.csv')
                
        

    
    
    
import csv
import pandas as pd
import numpy as np

# Parameters
input_file = 'lidar.csv'          
output_file = 'lidar_data_noisy.csv'  
noise_std_dev = 0.00001              


def count_file_size() -> None:
    with open(input_file, mode="r") as file:
        csv_reader = csv.reader(file)

        counter: int = 1
        readings: int = 0
        for row in csv_reader:
            number_of_elemts = row[0].split(";")
            readings = len(number_of_elemts)
            counter += 1
        print(f"file size: {readings}x{counter}")

def add_noise() -> None:
    lidar_data = pd.read_csv(input_file, delimiter=';')

    noise = np.random.normal(0, noise_std_dev, lidar_data.shape)
    lidar_data_noisy = lidar_data + noise

    lidar_data_noisy.to_csv(output_file, index=False)
    print(f"Noisy data saved to {output_file}")

def main() -> None:
    count_file_size()
    add_noise()



if __name__ == "__main__":
    main()
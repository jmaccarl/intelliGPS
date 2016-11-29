import csv
import numpy

gps_data_file = 'test-data/gps_coordinates_loop.csv'
gps_distance_file = 'test-data/distance_change_loop.csv'

def get_coordinates(row):
    coordinates = row[2].split(',')
    latitude = float(coordinates[0].strip())
    longitude = float(coordinates[1].strip())
    return numpy.array((latitude,longitude))

def get_euclidean_distance(a,b):
    return numpy.linalg.norm(a-b)

def gps_distances():
    read_file = open(gps_data_file,'rb')
    write_file = open(gps_distance_file,'w')
    reader = list(csv.reader(read_file))
    writer = csv.writer(write_file)
    writer.writerow(['Euclidean Distance Between Successive GPS Coordinates'])
    curr_coordinates = None
    prev_coordinates = get_coordinates(reader[0])
    reader = reader[1:]
    for row in reader:
        curr_coordinates = get_coordinates(row)
        distance = get_euclidean_distance(curr_coordinates,prev_coordinates)
        writer.writerow([distance])
        prev_coordinates = curr_coordinates
    read_file.close()
    write_file.close()

gps_distances()

import csv
import numpy
import argparse
import sys

def get_coordinates(row):
    coordinates = row[2].split(',')
    latitude = float(coordinates[0].strip())
    longitude = float(coordinates[1].strip())
    return numpy.array((latitude,longitude))

def get_euclidean_distance(a,b):
    return numpy.linalg.norm(a-b)

def gps_distances(gps_data_file):
    read_file = open(gps_data_file,'rb')
    reader = list(csv.reader(read_file))
    curr_coordinates = None
    prev_coordinates = get_coordinates(reader[0])
    reader = reader[1:]
    total_dist = 0
    for row in reader:
        curr_coordinates = get_coordinates(row)
        distance = get_euclidean_distance(curr_coordinates,prev_coordinates)
        prev_coordinates = curr_coordinates
        total_dist += distance
    read_file.close()
    print "Total Dist: ", total_dist

def main():
    if sys.argc < 2:
        print "GPS data file not provided"
        return

    gps_distances(sys.argv[1])

main()

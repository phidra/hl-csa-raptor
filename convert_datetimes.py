#!/usr/bin/env python3

import sys


def hours_minutes_seconds_to_total_seconds(time_in_hms):
    """
    INPUT  = "05:06:26"
    OUTPUT = 18386 = 5*3600 + 6*60 + 26
    """
    hours, minutes, seconds = [int(x) for x in time_in_hms.split(":")]
    return 3600 * hours + 60 * minutes + seconds


assert(hours_minutes_seconds_to_total_seconds("05:06:26") == 18386)


if __name__ == "__main__":
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    print("INPUT  = {}".format(input_file))
    print("OUTPUT = {}".format(output_file))
    counter = 0
    with open(input_file, "r") as in_f, open(output_file, "w") as out_f:
        out_f.write("trip_id,arrival_time,departure_time,stop_id,stop_sequence\n")
        for line in in_f:
            counter += 1
            if counter == 1:
                continue
            line = line.rstrip("\n")
            trip_id, arrival_time, departure_time, stop_id, stop_sequence, *_ = line.split(",")
            out_f.write("{},{},{},{},{}\n".format(
                trip_id,
                hours_minutes_seconds_to_total_seconds(arrival_time),
                hours_minutes_seconds_to_total_seconds(departure_time),
                stop_id,
                stop_sequence,
            ))
            # if counter > 10:
            #     break
    print("Done processing {} lines + 1 header".format(counter))

SAMPLE = """
trip_id                          , arrival_time , departure_time , stop_id , stop_sequence , pickup_type , drop_off_type , stop_headsign
23358248-2020-TRAM_A1-Lun-Mer-45 , 05:06:26     , 05:06:26       , 7440    , 1             , 0           , 0             ,
23358248-2020-TRAM_A1-Lun-Mer-45 , 05:08:26     , 05:08:26       , 7438    , 2             , 0           , 0             ,
"""

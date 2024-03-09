import numpy as np
from objects import Robot
from software import Software_1


#initiliaze game objects

all_game_objects = []

sensor_dict = {
    "receiver": {
        "number_of_receivers": 4,
        "receiver_placement_radius": 0.05,
        "receiver_placement_offset_angle":0,
        "view_angle": 45
    },
    "transmitter": {
        "number_of_transmitters": 4,
        "transmitter_placement_radius": 0.05,
        "transmitter_placement_offset_angle":0,
        "beam_angle": 45
    },
    "ultrasonic_sensor": {
        "placement_angles": [-90,0 , 90],
        "ultrasonic_sensor_placement_radius": 0.10,
        "view_angle": 20,
        "range":1.0
    }
}

robot_object_1 = Robot( id=1, collision_radius = 0.1, position = np.array([0.0, 0.0]), sensor_dict= sensor_dict, verbose = False)
all_game_objects.append(robot_object_1)

robot_object_1.set_all_game_objects(all_game_objects = all_game_objects)

software_1 = Software_1(robot=robot_object_1)
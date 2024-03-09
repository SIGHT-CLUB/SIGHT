import numpy as np

def ____rotate_vector(vector: np.ndarray, angle_degrees: float) -> np.ndarray:
    """
    Rotates a vector by a given angle in degrees in the clockwise direction
    """
    angle_radians = np.radians(angle_degrees)
    rotation_matrix = np.array([[np.cos(angle_radians), -np.sin(angle_radians)],
                                    [np.sin(angle_radians), np.cos(angle_radians)]])
    return rotation_matrix @ vector    
np.ext_rotate_vector = ____rotate_vector # Add the function to the numpy module

def ensure_updated_decorator(f):
    # A decorator that ensures that the sensor output is updated before reading it
    def wrapper(self, *args, **kwargs):
        self.update()
        return f(self, *args, **kwargs)
    return wrapper

#======================================================================================================================
class Obstacle():
    def __init__(self, id:int = None, position: np.ndarray = None, collision_radius:float=None) -> None:
        self.ID = id
        self.POSITION = position
        self.COLLISION_RADIUS = collision_radius # The radius of the obstacle in meters

    def get_id(self)->int:
        """
        Returns the id of the obstacle
        """
        return self.ID
    
    def get_position(self)->np.ndarray:
        """
        Returns the current (x,y) position of the obstacle in meters as a numpy array
        """
        return self.POSITION
    
    def get_collision_radius(self)->float:
        """
        Returns the collision radius of the obstacle in meters as a float
        """
        return self.COLLISION_RADIUS

class Robot():
    def __init__(self, id:int = None, collision_radius:float = None, position: np.ndarray = None, sensor_dict: dict = None, verbose = False) -> None:
                self.ID = id
                self.SENSOR_DICT = sensor_dict   
                self.COLLISION_RADIUS = collision_radius # The radius of the robot in meters 
                self.VERBOSE = verbose
                
                self.position = position # x,y position of the robot w.r.t the world frame in meters
                self.current_rotation = 0 # The angle of rotation of the robot w.r.t the world frame in degrees in clockwise direction
                self.facing_direction = np.array([1.0,0.0]) # The direction in which the robot is facing as a unit vector

                self.robot_speed = np.array([0.0, 0.0]) #change of distance per second
                self.rotation_speed = 0 #rotation angle of the robot per second
                self.initiliaze_sensors()

    def set_robot_movements(self, v_x:float=0, v_y:float=0, angle_per_sec:float = 0):
        self.robot_speed[0] = v_x
        self.robot_speed[1] = v_y
        self.rotation_speed = angle_per_sec

    def update_robots_position(self, del_time:float = 0):
        del_x = self.robot_speed[0]*del_time
        del_y = self.robot_speed[1]*del_time
        self.__move_robot(del_x= del_x, del_y= del_y)

        del_angle = self.rotation_speed*del_time
        self.__rotate_robot(rotate_angle=del_angle)

    def initiliaze_sensors(self):
        """
        initiliazes the IR receivers-transmitters and ultrasonic sensors of the robot based on the sensor_dict
        """
        self.receivers_list:Receiver = []
        self.transmitters_list:Transmitter = []
        self.ultrasonic_sensors_list:UltrasonicSensor = []

        if "receiver" in self.SENSOR_DICT:
                #the dictionary contains: number_of_receivers, receiver_placement_radius, receiver_placement_offset_angle, view_angle              

                NUMBER_OF_RECEIVERS = self.SENSOR_DICT["receiver"]["number_of_receivers"]                    
                PLACEMENT_RADIUS = self.SENSOR_DICT["receiver"]["receiver_placement_radius"]
                OFFSET_ANGLE= self.SENSOR_DICT["receiver"]["receiver_placement_offset_angle"]
                VIEW_ANGLE = self.SENSOR_DICT["receiver"]["view_angle"]
                _ANGLE_PER_RECEIVER = 360.0 / NUMBER_OF_RECEIVERS

                for i in range(NUMBER_OF_RECEIVERS):                        
                    rotate_angle = i*_ANGLE_PER_RECEIVER + OFFSET_ANGLE
                    rotated_relative_position_vector = np.ext_rotate_vector(  np.array([PLACEMENT_RADIUS, 0.0]), rotate_angle)      
                    receiver_i = Receiver(robot_object=self, sensor_id = i, relative_position_vector=rotated_relative_position_vector, view_angle= VIEW_ANGLE)
                    if(self.VERBOSE): print(receiver_i)
                    self.receivers_list.append(receiver_i)
         
        if "transmitter" in self.SENSOR_DICT:
                #the dictionary contains: number_of_transmitters, transmitter_placement_radius, transmitter_placement_offset_angle
                NUMBER_OF_TRANSMITTERS = self.SENSOR_DICT["transmitter"]["number_of_transmitters"]                    
                PLACEMENT_RADIUS = self.SENSOR_DICT["transmitter"]["transmitter_placement_radius"]
                OFFSET_ANGLE= self.SENSOR_DICT["transmitter"]["transmitter_placement_offset_angle"]
                BEAM_ANGLE = self.SENSOR_DICT["transmitter"]["beam_angle"]
                _ANGLE_PER_TRANSMITTER = 360.0 / NUMBER_OF_TRANSMITTERS

                for i in range(NUMBER_OF_TRANSMITTERS):
                    rotate_angle = i*_ANGLE_PER_TRANSMITTER + OFFSET_ANGLE
                    rotated_relative_position_vector = np.ext_rotate_vector(  np.array([PLACEMENT_RADIUS, 0.0]), rotate_angle)
                    transmitter_i = Transmitter(robot_object=self, sensor_id = i, relative_position_vector=rotated_relative_position_vector, beam_angle= BEAM_ANGLE)
                    if(self.VERBOSE): print(transmitter_i)
                    self.transmitters_list.append(transmitter_i)

        if "ultrasonic_sensor" in self.SENSOR_DICT:
                #the dictionary contains: placement_angles, ultrasonic_sensor_placement_radius
                ULTRASONIC_RANGE = self.SENSOR_DICT["ultrasonic_sensor"]["range"]
                VIEW_ANGLE = self.SENSOR_DICT["ultrasonic_sensor"]["view_angle"]
                PLACEMENT_ANGLES = self.SENSOR_DICT["ultrasonic_sensor"]["placement_angles"]                    
                PLACEMENT_RADIUS = self.SENSOR_DICT["ultrasonic_sensor"]["ultrasonic_sensor_placement_radius"]


                for i, placement_angle in enumerate(PLACEMENT_ANGLES):                 
                    rotated_relative_position_vector = np.ext_rotate_vector(  np.array([PLACEMENT_RADIUS, 0.0]), placement_angle)
                    ultrasonic_sensor_i = UltrasonicSensor(robot_object=self, sensor_id = i, relative_position_vector=rotated_relative_position_vector, range = ULTRASONIC_RANGE, view_angle= VIEW_ANGLE)
                    if(self.VERBOSE): print(ultrasonic_sensor_i)
                    self.ultrasonic_sensors_list.append(ultrasonic_sensor_i)

    def get_id(self)->int:
        """
        Returns the id of the robot
        """
        return self.ID
    
    def get_colision_radius(self)->float:
        """
        Returns the collision radius of the robot in meters as a float
        """
        return self.COLLISION_RADIUS
    
    def get_current_rotation_angle(self)->float:
        """
        Returns the current rotation angle of the robot in degrees as a float
        """
        return self.current_rotation    
    
    def get_position(self)->np.ndarray:
        """
        Returns the current (x,y) position of the robot in meters as a numpy array
        """
        return self.position
    
    def get_receivers_list(self):
        """
        Returns the list of receivers of the robot
        """
        return self.receivers_list
    
    def get_transmitters_list(self):    
        """
        Returns the list of transmitters of the robot
        """    
        return self.transmitters_list
    
    def get_ultrasonic_sensors_list(self):
        """
        Returns the list of ultrasonic sensors of the robot
        """
        return self.ultrasonic_sensors_list
    
    def __move_robot(self, del_x: float=0, del_y:float=0):
        """
        Moves the robot by the given distance in meters in the x and y direction
        """
        self.position[0] += del_x
        self.position[1] += del_y        
    
    def __rotate_robot(self,rotate_angle: float=0):
        """
        rotates the robot by the given angle in degrees in the clockwise direction
        """
        self.current_rotation = (self.current_rotation + rotate_angle)%360
    
    def set_all_game_objects(self,all_game_objects:list = []):
        """
        Refer to the list that contains all game objects. 
        """
        self.ALL_GAME_OBJECTS = all_game_objects

        for receiver in self.get_receivers_list():
            receiver.set_all_game_objects(all_game_objects = all_game_objects)

        for transmitter in self.get_transmitters_list():
            transmitter.set_all_game_objects(all_game_objects = all_game_objects)

        for ultrasonic_sensor in self.get_ultrasonic_sensors_list():
            ultrasonic_sensor.set_all_game_objects(all_game_objects = all_game_objects)

    def test(self):
        for receiver in self.get_receivers_list():
            r = receiver.read_receiver_output()
            print(f"Receiver {receiver.get_sensor_id()} output: {r}")

        for ultrasonic_sensor in self.get_ultrasonic_sensors_list():
            r = ultrasonic_sensor.measure_distance()
            print(f"Ultrasonic sensor {ultrasonic_sensor.get_sensor_id()} output: {r}")
                                                        
class Sensor():

    def __init__(self,  robot_object: Robot = None, sensor_id:int = None, relative_position_vector:np.ndarray = None) -> None:
        """
        robot_object: The robot object to which the sensor is attached
        sensor_id: The id of the sensor
        relative_position_vector: A vector starting from the center of the robot and ending at the position of the sensor in meters
        """
        self.ROBOT_OBJECT = robot_object
        self.SENSOR_ID = sensor_id
        self.RELATIVE_POSITION_VECTOR = relative_position_vector
    
    def set_all_game_objects(self,all_game_objects:list = []):
        """
        Sets the list of all game objects in the environment
        """
        self.ALL_GAME_OBJECTS = all_game_objects

    def get_sensor_id(self) -> int:
        """
        Returns the id of the sensor
        """
        return self.SENSOR_ID
    
    def get_relative_position_vector(self) -> np.ndarray:
        """
        Returns the relative position vector of the sensor w.r.t the robot in meters as a numpy array
        """
        return self.RELATIVE_POSITION_VECTOR
    
    def get_position(self) -> np.ndarray:
        """
        Returns the current position of the sensor in meters as a numpy array
        """

        #apply the rotation matrix to the relative position vector
        current_rotation_angle = self.ROBOT_OBJECT.get_current_rotation_angle()
        rotated_relative_position_vector = np.ext_rotate_vector(self.RELATIVE_POSITION_VECTOR, current_rotation_angle)

        #apply the translation to the rotated position vector
        current_position = self.ROBOT_OBJECT.get_position()        
        return current_position + rotated_relative_position_vector       
        
class Transmitter(Sensor):
    def __init__(self, robot_object: Robot = None, sensor_id:int = None, relative_position_vector: np.ndarray = None, beam_angle:float = None) -> None:
        #initilize the parent class
        super().__init__(robot_object=robot_object, sensor_id=sensor_id, relative_position_vector=relative_position_vector)

        self.BEAM_ANGLE = beam_angle
        self.transmitter_state = 1 #  0: not transmitting any signal, 1: transmitting a signal
    
    def __str__(self):
        return f"Robot {self.ROBOT_OBJECT.get_id()} | Transmitter: {self.get_sensor_id()}, relative position: {self.get_relative_position_vector()}"
    
    def set_transmitter_state(self, state: int):
        self.transmitter_state = state

    def get_transmitter_state(self)->int:
        return self.transmitter_state
    
    def get_beam_angle(self)->float:        
        return self.BEAM_ANGLE
    
class Receiver(Sensor):
    def __init__(self, robot_object: Robot = None, sensor_id:int = None, relative_position_vector: np.ndarray = None, view_angle:float =  None ) -> None:
        #initilize the parent class
        super().__init__(robot_object=robot_object, sensor_id=sensor_id, relative_position_vector=relative_position_vector)
        self.VIEW_ANGLE = view_angle

        self.receiver_output = 1 #  0: detecting a signal, 1: not detecting any signal,

    def __str__(self):
        return f"Robot {self.ROBOT_OBJECT.get_id()} | Receiver: {self.get_sensor_id()}, relative position: {self.get_relative_position_vector()}"
    
    @ensure_updated_decorator
    def read_receiver_output(self):
        return self.receiver_output
    

    def set_receiver_output(self, output: int):
        self.receiver_output = output

    def update(self) -> bool:
        """
        updates the receiver output according to whether the receiver is receiving a signal from the other robot considering the view angle and the obstacles
        """   
        self.set_receiver_output(1) # 1: not detecting any signal
        for game_object in self.ALL_GAME_OBJECTS:
            if isinstance(game_object, Robot) and game_object != self.ROBOT_OBJECT:                
                is_receiver_receiving_signal = False
                
                receiver_position = self.get_position()
                receiver_lagging_vector = np.ext_rotate_vector(self.get_relative_position_vector(), -self.VIEW_ANGLE/2)
                receiver_leading_vector = np.ext_rotate_vector(self.get_relative_position_vector(), self.VIEW_ANGLE/2)

                for transmitter in game_object.get_transmitters_list():
                    if transmitter.get_transmitter_state() == 0: continue #transmitter is not transmitting any signal

                    #CHECK 1 - Line of sight check
                    transmitter_position = transmitter.get_position()
                    transmitter_lagging_vector = np.ext_rotate_vector(transmitter.get_relative_position_vector(), -transmitter.get_beam_angle()/2)
                    transmitter_leading_vector = np.ext_rotate_vector(transmitter.get_relative_position_vector(), transmitter.get_beam_angle()/2)

                    #check if the transmitter is within the view angle of the receiver (i.e positive linear combination of the right and left vectors of the receiver can be used to reach the position of the transmitter)
                    is_transmitter_within_view_angle = False
                    receiver_to_transmitter_vector = transmitter_position - receiver_position
                    A = np.array([
                        [receiver_lagging_vector[0], receiver_leading_vector[0]],
                        [receiver_lagging_vector[1], receiver_leading_vector[1]]
                    ]
                    )
                    B = np.array([
                        receiver_to_transmitter_vector[0],
                        receiver_to_transmitter_vector[1]
                    ])
                    a, b = np.linalg.solve(A, B)
                    if a>=0 and b>=0:
                        is_transmitter_within_view_angle = True

                    #check if the receiver is within the beam angle of the transmitter (i.e positive linear combination of the right and left vectors of the transmitter can be used to reach the position of the receiver)
                    is_receiver_within_beam_angle = False
                    transmitter_to_receiver_vector = receiver_position - transmitter_position
                    A = np.array([
                        [transmitter_lagging_vector[0], transmitter_leading_vector[0]],
                        [transmitter_lagging_vector[1], transmitter_leading_vector[1]]
                    ]
                    )
                    B = np.array([
                        transmitter_to_receiver_vector[0],
                        transmitter_to_receiver_vector[1]
                    ])
                    a, b = np.linalg.solve(A, B)
                    if a>=0 and b>=0:
                        is_receiver_within_beam_angle = True

                    #CHECK 2 - Obstacle check
                    if is_transmitter_within_view_angle and is_receiver_within_beam_angle:
                        is_receiver_receiving_signal = True

                        for game_object_2 in self.ALL_GAME_OBJECTS:
                            if game_object_2 == self.ROBOT_OBJECT or game_object_2 == game_object:continue

                            vector_length = np.linalg.norm(transmitter_to_receiver_vector)
                            unit_transmitter_to_receiver_vector = transmitter_to_receiver_vector / vector_length                                            
                            obstacle_radius = game_object_2.get_collision_radius()                            

                            for k in  np.linspace(0, vector_length, 100): #This is not efficient, but it is a simple way to check for obstacles
                                point_on_line = transmitter.get_position() + k*unit_transmitter_to_receiver_vector
                                distance_to_obstacle = np.linalg.norm(point_on_line - game_object_2.get_position())
                                if distance_to_obstacle <= obstacle_radius:
                                    is_receiver_receiving_signal = False
                                    break

                            if not is_receiver_receiving_signal:
                                break                                

                if is_receiver_receiving_signal:
                    self.set_receiver_output(0)    
                    break        
                       
class UltrasonicSensor(Sensor):
    def __init__(self, robot_object: Robot = None, sensor_id:int = None, relative_position_vector: np.ndarray = None , range:float=None, view_angle:float=None) -> None:
        #initilize the parent class
        super().__init__(robot_object=robot_object, sensor_id=sensor_id, relative_position_vector=relative_position_vector)
        
        self.MAX_DETECTION_RANGE = range #  the maximum distance in meters that the sensor can detect
        self.VIEW_ANGLE = view_angle

        self.ultrasonic_sensor_measured_distance = None #  the detected distance in meters

    @ensure_updated_decorator
    def measure_distance(self):
        return self.ultrasonic_sensor_measured_distance

    def update(self) -> bool:
        """
        updates the receiver output according to whether the receiver is receiving a signal from the other robot considering the view angle and the obstacles
        """   
        self.ultrasonic_sensor_measured_distance = self.MAX_DETECTION_RANGE
        ultrasonic_sensor_position = self.get_position()

        ultrasonic_sensor_lagging_vector = np.ext_rotate_vector(self.get_relative_position_vector(), -self.VIEW_ANGLE/2)
        ultrasonic_sensor_leading_vector = np.ext_rotate_vector(self.get_relative_position_vector(), self.VIEW_ANGLE/2)

        for game_object in self.ALL_GAME_OBJECTS:
            if (isinstance(game_object, Robot) and game_object != self.ROBOT_OBJECT) or isinstance(game_object, Obstacle):                
            
                object_position = game_object.get_position()        
                sensor_to_object_vector = object_position - ultrasonic_sensor_position
                vector_length = np.linalg.norm(sensor_to_object_vector)

                # check if the object is within the view angle of the sensor (i.e positive linear combination of the right and left vectors of the sensor can be used to reach the position of the object)
                A = np.array([
                    [ultrasonic_sensor_lagging_vector[0], ultrasonic_sensor_leading_vector[0]],
                    [ultrasonic_sensor_lagging_vector[1], ultrasonic_sensor_leading_vector[1]]
                ]
                )
                B = np.array([
                    sensor_to_object_vector[0],
                    sensor_to_object_vector[1]
                ])
                a, b = np.linalg.solve(A, B)
                if a>=0 and b>=0:
                    if vector_length < self.ultrasonic_sensor_measured_distance:
                        self.ultrasonic_sensor_measured_distance = vector_length
                    else:
                        #there is already an object that is closer to the sensor
                        pass

    def __str__(self):
        return f"Robot {self.ROBOT_OBJECT.get_id()} | UltrasonicSensor: {self.get_sensor_id()}, relative position: {self.get_relative_position_vector()}"

#======================================================================================================================    
if __name__ == "__main__":
    all_game_objects = []

    robot_initial_position = np.array([0.0, 0.0])

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

    robot_object_2 = Robot( id=2, collision_radius = 0.1, position = np.array([0.5, 0.0]), sensor_dict= sensor_dict, verbose = False)
    all_game_objects.append(robot_object_2)

    obstacle_1= Obstacle(id=1, collision_radius = 0.05, position = np.array([0.25, 50]), )
    all_game_objects.append(obstacle_1)

    for game_object in all_game_objects:
        if isinstance(game_object, Robot):
            game_object.set_all_game_objects(all_game_objects = all_game_objects)

    robot_object_1.test()
    robot_object_2.test()



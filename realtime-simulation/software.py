from objects import Robot

class Software_1():
    def __init__(self, robot:Robot = None):
        self.ROBOT = robot
        self.TRANSMITTERS = robot.get_transmitters_list()
        self.IR_RECEIVERS = robot.get_receivers_list()
        self.ULTRASONIC_SENSORS = robot.get_ultrasonic_sensors_list()

        r = self.__read_ultrasonic_sensor(sensor_id = 0)
        print(r)

    def __set_robot_movements(self, v_x:float=0, v_y:float=0, angle_per_sec:float=0):
        self.ROBOT.set_robot_movements( v_x=v_x, v_y=0, angle_per_sec = angle_per_sec)

    def __turn_on_transmitter(self, transmitter_id:int=0):
        if transmitter_id >= len(self.TRANSMITTERS):
            raise Exception("Sensor id is out of range")
        
        transmitter_object = self.TRANSMITTERS[transmitter_id]
        transmitter_object.set_transmitter_state(state = 1)

    def __turn_off_transmitter(self, transmitter_id:int=0):
        if transmitter_id >= len(self.TRANSMITTERS):
            raise Exception("Sensor id is out of range")
        
        transmitter_object = self.TRANSMITTERS[transmitter_id]
        transmitter_object.set_transmitter_state(state = 0)

    def __read_IR_receiver_sensor(self, sensor_id:int = 0) -> int:
        if sensor_id >= len(self.IR_RECEIVERS):
            raise Exception("Sensor id is out of range")
        
        IR_receiver_sensor_object = self.IR_RECEIVERS[sensor_id]
        state = IR_receiver_sensor_object.read_receiver_output()
        return state

    def __read_ultrasonic_sensor(self, sensor_id:int = 0)->float:
        if sensor_id >= len(self.ULTRASONIC_SENSORS):
            raise Exception("Sensor id is out of range")
        
        ultrasonic_sensor_object = self.ULTRASONIC_SENSORS[sensor_id]
        distance = ultrasonic_sensor_object.measure_distance()
        return distance

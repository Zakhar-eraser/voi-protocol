from pymavlink import mavutil

the_connection = mavutil.mavlink_connection('udpin:localhost:14551')

print("Heartbeat from system (system %u component %u)" %
      (the_connection.target_system, the_connection.target_component))
a: int = 10
exclude_mass = ["ATTITUDE", "ATTITUDE_TARGET", "ATTITUDE_QUATERNION", "POSITION_TARGET_LOCAL_NED", "GLOBAL_POSITION_INT"
    , "SERVO_OUTPUT_RAW", "LOCAL_POSITION_NED", "ALTITUDE", "BATTERY_STATUS", "EXTENDED_SYS_STATE"
    , "GPS_RAW_INT", "SYS_STATUS", "VFR_HUD", "HOME_POSITION", "ESTIMATOR_STATUS", "UTM_GLOBAL_POSITION"
    , "HEARTBEAT", "VIBRATION"]
while True:
    # the_connection.mav.command_long_send(the_connection.target_system, the_connection.target_component,
    #                                      0, 0, 6, 8, 0, 0, 0, 3, 0)
    # the_connection.mav.command_long_send(the_connection.target_system, the_connection.target_component,
    #                                      410, 0, 0, 8, 0, 0, 0, 0, 0)

    msg = the_connection.recv_match()
    if not msg:
        continue
     else:
         print(msg)
    #if msg.get_type() not in exclude_mass:
    #    print("Got message %s" % msg.get_type())
    #    print(msg)
    # if msg.get_type() == "GPS_STATUS":
    #     print("Got message %s" % msg.get_type())
    # if msg.get_type() == "COMMAND_ACK":
    #     print(" ACK Got message %s" % msg.get_type())
    #     print(msg)

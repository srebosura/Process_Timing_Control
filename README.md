# Process_Timing_Control
Silicon wafer frame inspection/detection by IR and brushing process time-out control
The system is consist of an IR sensor which detects the first edge of the frame with user set time to trigger the next edge. Failure to detect the other edge will trigger an alarm.
When a frame detection on both sides is sucess, a countdown starts for this frame. The first IR sensor is on the brusher entrance. A second IR sensor is on the brusher output.
Up to 4 frames can be simultaneously process by the countdown timer. IR sensor time-out detection and the brushing countdown timer can be user set in the system menu. These data are all save in eeprom. 
